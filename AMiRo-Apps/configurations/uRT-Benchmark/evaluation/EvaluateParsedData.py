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
import re
import numpy as np
import pandas
import os

import commonclasses as µRT

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    def str2range(s):
        if not isinstance(s, str):
            raise ValueError('argument is no string')
        # check whether the value string looks like "[int]:[int]"
        if re.match('^\s*-?\d*:-?\d*\s*$', s):
            # initialize default
            r = IterationRange()
            # find all values
            values = np.array(re.findall('-?\d+', s), dtype=np.int32)
            # check for 'first' integer
            if re.match('^\s*-?\d+:.*$', s):
                r.first = values[0]
            # check for 'last' integer
            if re.match('^.*:-?\d+\s*$', s):
                r.last = values[-1]
            return r
        else:
            raise argparse.ArgumentError('invalid input')

    # setup parser
    parser = argparse.ArgumentParser(description='Evaluate parsed µRT benchmark data.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('infile',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='FILE',
                        help='path to a data file to evaluate')
    parser.add_argument('-i', '--iteration-column',
                        action='store',
                        nargs=1,
                        type=str,
                        default=['ITERATION'],
                        metavar='LABEL',
                        help='''label to identify the iteration column
If not specified this defaults to "ITERATION".''')
    parser.add_argument('-r', '--iteration-range',
                        action='store',
                        nargs=1,
                        type=str2range,
                        default=[IterationRange()],
                        help='''range of iterations to evaluate
A value of 0 specifies the first iteration, a value of -1 the last iteration.
If not specified, all iterations are considered (equivalent to '0:-1').''',
                        metavar='[first]:[last]')
    parser.add_argument('-e', '--evaluation-columns',
                        action='extend',
                        nargs='+',
                        type=str,
                        default=[],
                        metavar='LABEL',
                        help='''labels of the columns to evaluate
If not specified, the last column is assumed to be the only dependent variable.''')
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
    parser.add_argument('-n', '--normalize',
                        action='extend',
                        nargs='+',
                        type=pathlib.Path,
                        default=[],
                        metavar='FILE LABEL ...',
                        help='''normalize data by another evaluated file
Data is normalized by the \'mean\' column, but ignoring the initial offset.''')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs='?',
                        type=pathlib.Path,
                        const=pathlib.Path('__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__'),
                        metavar='DIR',
                        help='path to a directory to store evaluation file to')
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

class IterationRange:
    def __init__(self, first: int = None, last: int = None):
        self.first = first
        self.last = last

def runWithArgs(args: list[str] = None, ignore_unknown_args: bool = False):
    args = __parseArguments__(args=args, ignore_unknown=ignore_unknown_args)
    return evaluate(infile = args.infile[0],
                    outdir = args.output if args.output else None,
                    force = args.force,
                    it_label = args.iteration_column[0],
                    it_range = args.iteration_range[0],
                    depvar_labels = args.evaluation_columns,
                    indepvar_labels = args.independent_variables,
                    normalize = args.normalize)

def evaluate(infile: pathlib.Path, outdir: pathlib.Path = None, force: bool = False,
             it_label: str = 'ITERATION', it_range: IterationRange = IterationRange(),
             depvar_labels: list[str] = [],
             indepvar_labels: list[str] = [],
             normalize: list[pathlib.Path] = []):

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

    # check iteration label
    if not it_label in indata.columns:
        raise Exception('column "' + it_label + '" does not exist')
    iterations = sorted(set(indata[it_label]))

    # check range settings
    if it_range.first == None:
        it_range.first = 0
    elif it_range.first < 0:
        it_range.first = it_range.first + len(iterations) - 1
    if it_range.last == None:
        it_range.last = len(iterations) - 1
    elif it_range.last < 0:
        it_range.last = it_range.last + len(iterations) - 1
    if it_range.first < 0 or it_range.first >= len(iterations) or it_range.last < 0 or it_range.last >= len(iterations) or it_range.first > it_range.last:
        raise Exception('invalid iteration range')

    # check dependent variable label
    if len(depvar_labels) == 0:
        depvar_labels = [indata.columns[-1]]
    else:
        for dvlabel in depvar_labels:
            if not dvlabel in indata.columns:
                raise Exception('column "' + dvlabel + '" does not exist')

    # check independent variable lables
    if len(indepvar_labels) == 0:
        indepvar_labels = [indata.columns[0]]
    if len(indepvar_labels) != len(set(indepvar_labels)):
        raise Exception('independent variables conatain duplicates')
    for ivlabel in indepvar_labels:
        if not ivlabel in indata.columns:
            raise Exception('column "' + label + '" does not exist')
        for dvlabel in depvar_labels:
            if ivlabel == it_label or ivlabel == dvlabel:
                raise Exception('column "' + ivlabel + '" must not be independent variable')

    # create outpt directory
    if outdir != None:
        # create output directory as needed
        outdir.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outdir)

    # repeat for all dependent variables
    outfiles = []
    for dvlabel in depvar_labels:

        # read and check normalization data
        normdata = []
        normlen = None
        normvalues = None
        for path in normalize:
            # read all files
            if not path.exists():
                raise Exception('file "' + path + '" does not exist')
            data = pandas.read_csv(filepath_or_buffer=path, sep='\t')
            if not 'mean' in data.columns or len(data['mean']) == 0:
                raise Exception('file "' + path + '" is not compatible')
            normdata.append(data['mean'].tolist())
        for data in normdata:
            # varify identical length of all normalization files
            if normlen == None:
                normlen = len(data)
            elif len(data) != normlen:
                raise Exception('normalization files have varying lengths')
        if normlen != None and normlen > 0:
            normvalues = [0] * normlen
            for data in normdata:
                # subtract initial offset
                minval = min(data)
                for i in range(0, len(data)):
                    data[i] = data[i] - minval
                    normvalues[i] = normvalues[i] + data[i]

        # generate output file name
        outfilename = infile.stem + '_' + dvlabel + '_by'
        for var in indepvar_labels:
            outfilename = outfilename + '_' + str(var)
        outfilename = outfilename + '_considering_' + it_label + '_' + str(iterations[it_range.first]) + '-' + str(iterations[it_range.last])
        if normvalues:
            outfilename = outfilename + '_normalized'
        outfilename = outfilename + infile.suffix

        # generate output file path
        if outdir != None:
            outfile = outdir.joinpath(outfilename)
            # check whether the file already exists
            if outfile.exists():
                # overwrite?
                if force == True:
                    if outfile.is_file():
                        outfile.unlink()
                    else:
                        raise FileExistsError('output file "' + str(outfile) + '" exists and cannot be removed')
                else:
                    raise FileExistsError('output file "' + str(outfile) + '" already exists')
        else:
            outfile = None

        # apply iteration range
        indata = indata.loc[(indata[it_label] >= iterations[it_range.first]) & (indata[it_label] <= iterations[it_range.last])]

        # prepare output data
        statistics_labels = ['min', 'max', 'mean', 'median', 'range']
        outdata = pandas.DataFrame(columns=indepvar_labels + statistics_labels)

        # extract independent variabel combinations
        indepvar_combinations = indata[indepvar_labels].drop_duplicates()

        # iterate over all independent variable combinations
        for row in indepvar_combinations.index:
            # filter by all independent variables
            evaldata = indata
            for ivlabel in indepvar_labels:
                evaldata = evaldata.loc[evaldata[ivlabel] == indepvar_combinations[ivlabel].loc[row]]

            # calculate statistics and append to output data
            outdata.loc[len(outdata.index)] = indepvar_combinations.loc[row].to_list() + [evaldata[dvlabel].min(),
                                                                                          evaldata[dvlabel].max(),
                                                                                          evaldata[dvlabel].mean(),
                                                                                          evaldata[dvlabel].median(),
                                                                                          evaldata[dvlabel].max() - evaldata[dvlabel].min()]

        # normalize data if requested
        if normvalues != None:
            if len(outdata.index) != len(normvalues):
                raise Exception('size of normalization data does not match')
            for row in range(0, len(normvalues)):
                for label in statistics_labels:
                    if label == 'range':
                        continue
                    outdata.loc[outdata.index[row], label] = outdata.loc[outdata.index[row], label] - normvalues[row]

        # force integer types
        for label in outdata.columns:
            if label != 'mean' and label != 'median':
                outdata = outdata.astype({label: np.int32})

        # log evaluated data
        if outfile:
            # write to file
            µRT.log('writing evaluated data to file: ' + str(outfile), flush=True)
            outdata.to_csv(path_or_buf=outfile, sep='\t', header=True, index=False)
            outfiles.append(outfile)
        else:
            # print to stdout
            print('>>>\t' + outfilename)
            print(outdata.to_csv(None, sep='\t', header=True, index=False), end='', flush=True)

    µRT.waitForFilesystem(outfiles)
    return outfiles

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
