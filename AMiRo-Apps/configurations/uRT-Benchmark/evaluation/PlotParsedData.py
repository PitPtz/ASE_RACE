#!/usr/bin/env python3

"""
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import sys
import argparse
import pathlib
import os
import numpy as np
import pandas
import matplotlib.pyplot as plt
import math

import commonclasses as µRT

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    # setup parser
    parser = argparse.ArgumentParser(description='Plot parsed µRT benchmark data.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('infile',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='FILE',
                        help='path to a data file to plot')
    parser.add_argument('-c', '--columns',
                        action='extend',
                        nargs='+',
                        type=str,
                        default=[],
                        metavar='LABEL',
                        help='''labels of the columns to plot
If not specified, only the last column is utilized.''')
    parser.add_argument('-v', '--independent-variables',
                        action='extend',
                        nargs='+',
                        type=str,
                        default=[],
                        metavar='LABEL',
                        help='''labels of columns containing independent variables.
The given labels specify columns that contain independant variables.
Each value combination is evaluated separately.
If not specified, the first column is assummed to be the only independent variable.''')
    parser.add_argument('-N', '--max-value',
                        action='store',
                        nargs=1,
                        type=int,
                        default=None,
                        metavar='VAL',
                        help='value to force as maximum of the value axis (y-axsis)')
    parser.add_argument('-L', '--max-latency',
                        action='store',
                        nargs=1,
                        type=int,
                        default=None,
                        metavar='VAL',
                        help='value to force as maximum of the latency axis (x-axsis)')
    parser.add_argument('-t', '--transparent-background',
                        action='store_true',
                        help='make background of plots transparent')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs='?',
                        type=pathlib.Path,
                        const=pathlib.Path('__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__'),
                        metavar='DIR',
                        help='path to a directory to write plot file to')
    parser.add_argument('-f', '--force',
                        action='store_true',
                        help='overwrite existing file')

    # run parser and do some postprocessing
    if ignore_unknown == True:
        parsed = parser.parse_known_args(args)[0]
    else:
        parsed = parser.parse_args(args)
    if parsed.output and str(parsed.output) == '__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__':
        parsed.output = parsed.infile[0].parent

    return parsed

#-------------------------------------------------------------------------------
# interface functions and classes
#-------------------------------------------------------------------------------

def runWithArgs(args: list[str] = None, ignore_unknown_args: bool = False):
    args = __parseArguments__(args=args, ignore_unknown=ignore_unknown_args)
    return plot(infile = args.infile[0],
                outdir = args.output if args.output else None,
                force = args.force,
                plot_labels = args.columns,
                indepvar_labels = args.independent_variables,
                max_value = args.max_value[0] if args.max_value else None,
                max_latency = args.max_latency[0] if args.max_latency else None,
                transparent_background = args.transparent_background)

def plot(infile: pathlib.Path, outdir: pathlib.Path = None, force: bool = False,
         plot_labels: list[str] = [],
         indepvar_labels: list[str] = [],
         max_value: int = None,
         max_latency: int = None,
         transparent_background: bool = False):

    # check whether input file exists
    if not infile.exists():
        raise FileExistsError('input file does not exist')
    elif not infile.is_file():
        raise Exception('input is no file')
    # load file (last lines may contain some meta data, which causes issues with pandas)
    read_state = {'result': False, 'footer': 0}
    while read_state['result'] != True:
        try:
            indata = pandas.read_csv(filepath_or_buffer=infile, sep='\t', dtype=np.int32, skipfooter=read_state['footer'])
            read_state['result'] = True
        except:
            read_state['footer'] = read_state['footer'] + 1

    # if specified, check for plot column label and set it otherwise
    if len(plot_labels) != 0:
        for plabel in plot_labels:
            if not plabel in indata.columns:
                raise Exception('invalid column label "' + plabel + '"')
    else:
        plot_labels = indata.columns[-1]

    # check independent variable lables
    if len(indepvar_labels) == 0:
        indepvar_labels = [indata.columns[0]]
    if len(indepvar_labels) != len(set(indepvar_labels)):
        raise Exception('independent variables conatain duplicates')
    for ivlabel in indepvar_labels:
        if not ivlabel in indata.columns:
            raise Exception('column "' + ivlabel + '" does not exist')
        for plabel in plot_labels:
            if ivlabel == plabel:
                raise Exception('column "' + ivlabel + '" must not be independent variable')

    # extract independent variabel combinations
    indepvar_combinations = indata[indepvar_labels].drop_duplicates()

    # repeat for all columns to plot
    plotfiles = []
    plt.ioff()
    for plabel in plot_labels:
    # iterate over all independent variable combinations
        for row in indepvar_combinations.index:
            # generate plot name
            plotname = infile.stem + '_' + plabel + '_by'
            for var in indepvar_labels:
                plotname = plotname + '_' + str(var) + '=' + str(indepvar_combinations[var].loc[row]).zfill(math.ceil(math.log10(indepvar_combinations[var].max()+1)))

            # filter by all independent variables
            evaldata = indata
            for ivlabel in indepvar_labels:
                evaldata = evaldata.loc[evaldata[ivlabel] == indepvar_combinations[ivlabel].loc[row]]

            # plot histogram
            if outdir != None:
                plt.switch_backend('svg')
            if max_value == None:
                factor = pow(10, math.floor(math.log10(len(evaldata.index))))
                ymax = math.ceil(len(evaldata.index) / factor) * factor
            else:
                ymax = max_value
            if max_latency == None:
                xmin = math.floor(max(evaldata[plabel].min() - 1, 0) / 10) * 10
                xmax = math.ceil((evaldata[plabel].max() + 1) / 10) * 10
            else:
                xmin = 0
                xmax = max_latency
            fig, ax = plt.subplots()
            ax.set_xlabel('latency (us)')
            ax.set_ylabel('samples')
            ax.hist(evaldata[plabel], bins=xmax-xmin, range=(xmin,xmax), align='left', log=True, bottom=1)
            ax.set_xlim(xmin=xmin, xmax=xmax)
            ax.set_ylim(ymax=ymax)
            ax.grid(axis='both', which='both')

            # show or save the plot
            if outdir != None:
                outfile = outdir.joinpath(plotname + '.svg')
                if force == False and outfile.exists():
                    raise FileExistsError('output file "' + str(outfile) + '" already exists')
                outdir.mkdir(parents=True, exist_ok=True)
                µRT.waitForFilesystem(outdir)
                µRT.log('saving plot to file: ' + str(outfile), flush=True)
                plt.savefig(str(outfile), transparent=transparent_background)
                plt.close(fig)
                plotfiles.append(outfile)
            else:
                ax.set_title(plotname)
                plt.show()

    plt.ion()
    µRT.waitForFilesystem(plotfiles)
    return plotfiles

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
