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
import pandas
import matplotlib.pyplot as plt

import commonclasses as µRT

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    # setup parser
    parser = argparse.ArgumentParser(description='Plot evaluated µRT benchmark data.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('infile',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='FILE',
                        help='path to a data file to plot')
    parser.add_argument('-m', '--max-value',
                        action='store',
                        nargs=1,
                        type=int,
                        metavar='VAL',
                        help='value to force as maximum of the y-axis')
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
                max_value = args.max_value[0] if args.max_value else None,
                transparent_background = args.transparent_background,
                outdir = args.output if args.output else None,
                force = args.force)

def plot(infile: pathlib.Path, max_value: int = None, transparent_background: bool = False, outdir: pathlib.Path = None, force: bool = False):

    # check whether input file exists
    if not infile.exists():
        raise FileExistsError('input file does not exist')
    elif not infile.is_file():
        raise Exception('input is no file')
    # load file
    indata = pandas.read_csv(filepath_or_buffer=infile, sep='\t')

    # verify compatibility of the file
    depvarlabels = {'min', 'max', 'mean', 'median', 'range'}
    for label in depvarlabels:
        if not label in indata.columns:
            raise Exception('incompatible input file')

    # extract labels of all independent variables
    indepvarlabels = []
    for label in indata.columns:
        if not label in depvarlabels:
            indepvarlabels = indepvarlabels + [label]
    if len(indepvarlabels) == 0:
        raise Exception('invalid input file')

    # extract worst case data from input
    plotdata = pandas.DataFrame(columns=list({indepvarlabels[0]}.union(depvarlabels)))
    indepvarvalues = sorted(list(set(indata[indepvarlabels[0]].to_list())))
    for value in indepvarvalues:
        filtereddata = indata.loc[indata[indepvarlabels[0]] == value].copy()
        filtereddata.sort_values(['max', 'range', 'median', 'mean'], ascending=False, inplace=True, ignore_index=True)
        plotdata.loc[len(plotdata.index)] = pandas.Series(dtype=object)
        plotdata.at[plotdata.index[-1], indepvarlabels[0]] = value
        for label in depvarlabels:
            plotdata.at[plotdata.index[-1], label] = filtereddata.at[0, label]

    # plot the data
    if outdir != None:
        plt.switch_backend('svg')
    x = plotdata[indepvarlabels[0]].to_list()
    fig, ax = plt.subplots()
    ax.set_xlabel(indepvarlabels[0].lower())
    ax.set_ylabel('latency (us)')
    ax.fill_between(x=x, y1=plotdata['min'].to_list(), y2=plotdata['max'].to_list(), color=(0,0,1,0.25))
    ax.plot(x, plotdata['max'].to_list(), linestyle='solid', color=(0,0,1,1), label='max')
    ax.plot(x, plotdata['mean'].to_list(), linestyle='solid', color=(0,1,0,1), label='mean')
    ax.plot(x, plotdata['median'].to_list(), linestyle='dashed', color=(1,0,0,1), label='median')
    ax.set_xlim(xmin=0)
    ax.set_ylim(ymin=0, ymax=max_value)
    ax.grid(which='major', axis='both')
    ax.legend()

    # show or save the plot
    if outdir != None:
        outfile = outdir.joinpath(infile.stem + '.svg')
        if force == False and outfile.exists():
            raise FileExistsError('output file "' + str(outfile) + '" already exists')
        outdir.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outdir)
        µRT.log('saving plot to file: ' + str(outfile), flush=True)
        plt.ioff()
        plt.savefig(str(outfile), transparent=transparent_background)
        plt.close(fig)
        µRT.waitForFilesystem(outfile)
        return outfile
    else:
        ax.set_title(infile.name)
        plt.show()
        return None


#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
