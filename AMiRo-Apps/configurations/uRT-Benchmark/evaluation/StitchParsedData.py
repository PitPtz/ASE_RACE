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
import numpy as np
import pathlib
import pandas
import os

import commonclasses as µRT

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    # setup parser
    parser = argparse.ArgumentParser(description='Combine multiple parsed µRT benchmark data files to a single file.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('infiles',
                        action='store',
                        nargs='+',
                        type=pathlib.Path,
                        metavar='INFILE',
                        help='path to a data file')
    parser.add_argument('-s', '--sort',
                        action='store',
                        nargs='+',
                        type=str,
                        default=[],
                        metavar='LABEL',
                        help='column labels to sort the data')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs='?',
                        type=pathlib.Path,
                        const=pathlib.Path('__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__'),
                        metavar='OUTFILE',
                        help='path to store the benchmark output log to')
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
    return stitch(infiles=args.infiles,
                  sortlabels=args.sort,
                  outfile=args.output if args.output else None,
                  force=args.force)

def stitch(infiles: list[pathlib.Path], sortlabels: list[str] = [], outfile: pathlib.Path = None, force: bool = False):

    def readCSV(file: pathlib.Path, dtype = object):
        read_state = {'result': False, 'footer': 0}
        while read_state['result'] != True:
            try:
                data = pandas.read_csv(filepath_or_buffer=file, sep='\t', dtype=dtype, skipfooter=read_state['footer'])
                read_state['result'] = True
            except:
                read_state['footer'] = read_state['footer'] + 1
        return data

    # sanity check arguments
    if len(infiles) == 0:
        raise FileExistsError('no input specified')
    for file in infiles:
        if not file.exists():
            raise FileExistsError('input "' + file + '" does not exist')
        elif not file.is_file():
            raise Exception('input is no file')

    # read first file to pandas
    data = readCSV(file=infiles[0], dtype=np.int32)
    # check labels
    for label in sortlabels:
        if not label in data.columns:
            raise Exception('label "' + label + '" not found')

    # for all other files
    for file in infiles[1:]:
        filedata = readCSV(file=file, dtype=np.int32)
        if not (filedata.columns == data.columns).all():
            raise Exception('incompatible input files')
        data = pandas.concat([data, filedata], ignore_index=True)

    # sort data
    if len(sortlabels) > 0:
        data.sort_values(sortlabels, inplace=True, kind='mergesort', ignore_index=True)

    # write to file
    if outfile:
        µRT.log('writing stitched data to file: ' + str(outfile), flush=True)
        # create directory if it does not exist yet
        outfile.parent.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outfile.parent)
        if force == False and outfile.exists():
            raise FileExistsError('output file "' + str(outfile) + '" already exists')
        data.to_csv(outfile, sep='\t', header=True, index=False)
        µRT.waitForFilesystem(outfile)
        return outfile
    else:
        print(data.to_csv(None, sep='\t', header=True, index=False), end='', flush=True)
        return None

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
