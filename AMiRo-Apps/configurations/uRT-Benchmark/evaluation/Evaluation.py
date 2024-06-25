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
import shutil
import math
import multiprocessing as mp
import enum
import re

import commonclasses as µRT
import ExecuteBenchmark
import ParseBenchmarkLog
import StitchParsedData
import PlotParsedData
import EvaluateParsedData
import PlotEvaluatedData

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

class CallingPolicy(enum.Enum):
    INLINE          = enum.auto()
    ASYNC           = enum.auto()

    def fromString(s: str):
        if s.lower() == 'inline':
            return CallingPolicy.INLINE
        elif s.lower() == 'async':
            return CallingPolicy.ASYNC
        else:
            raise ValueError('invalid argument')

    def __str__(self):
        if self == CallingPolicy.INLINE:
            return 'inline'
        elif self == CallingPolicy.ASYNC:
            return 'async'
        else:
            raise ValueError('invalid argument')

class BunchOfWorkers:
    def __init__(self):
        self.manager = mp.Manager()
        self.pool = None

    def setup(self):
        # create new thread pool
        self.pool = mp.Pool()

    def addTask(self, func, args=(), kwargs={}):
        # sanity check
        if self.pool == None:
            raise Exception('no threads initialized')
        # add task to the pool and execute asynchronously
        self.pool.apply_async(func=func, args=args, kwds=kwargs)

    def wait(self):
        # sanity check
        if self.pool == None:
            raise Exception('no threads initialized')
        # wait for workers to finish
        self.pool.close()
        self.pool.join()
        return

class DeviceInitialization(enum.Enum):
    NONE  = enum.auto()
    FIRST = enum.auto()
    ALL   = enum.auto()

    def fromString(s: str):
        if s.lower() == 'none':
            return DeviceInitialization.NONE
        elif s.lower() == 'first':
            return DeviceInitialization.FIRST
        elif s.lower() == 'all':
            return DeviceInitialization.ALL
        else:
            raise ValueError('invalid argument')

def __parseArguments__(args: list[str] = None):
    # setup parser
    parser = argparse.ArgumentParser(description='''Perform a complete evaluation of µRT.
Depending on the specified input argument, this includes a benchmark run on a device, parsing of raw output and/or evaluation of performance metrics.
Any further arguments are passed to the acording subroutines.
Please refer to the other scripts in this folder for further information about available arguments.''',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('input',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='INPUT',
                        help='''path to the device to interact with, the log file to parse or the parsed data to evaluate
If the path points to a serial device, a benchmark is executed and the resulting output is parsed and evaluated.
If the path points to a file, that file is assumed to contain raw benchmark output, which is parsed and evaluated.
If the path points to a directory, the folder is scanned for files containing raw bechnhmark output, which is combined for evaluation.''')
    parser.add_argument('--runs',
                        action='store',
                        nargs=1,
                        type=int,
                        default=[1],
                        metavar='N',
                        help='''number of execution runs
Only applies if input path points to a serial device.''')
    parser.add_argument('-i', '--init-device',
                        action='store',
                        nargs=1,
                        type=str.lower,
                        choices=['first', 'none', 'all'],
                        default=['first'],
                        metavar='RUN',
                        help='''initialize/reset device before executing benchmarks
The following arguments are available:
- 'none'
  Never initialize the device.
- 'first'
  Initialize before the first benchmark run only (default).
- 'all'
  Initialize device before every benchmark run.''')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs='?',
                        type=pathlib.Path,
                        const=pathlib.Path('__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__'),
                        metavar='DIR',
                        help='''directory to store the generated files to''')
    parser.add_argument('--plot',
                        action='store',
                        nargs='+',
                        type=str.lower,
                        choices=['parsed', 'evaluated'],
                        default=[],
                        metavar='DATA',
                        help='generate plots of the parsed and/or evaluated data')
    parser.add_argument('--mode',
                        action='store',
                        nargs=1,
                        type=str.lower,
                        choices=[str(CallingPolicy.INLINE), str(CallingPolicy.ASYNC)],
                        default=[str(CallingPolicy.ASYNC)],
                        metavar='MODE',
                        help='''mode how to process data
The following arguments are availabe:
- 'inline'
  All data is processed within a single thread.
- 'async'
  Whenever possible, data processing is executed in multiple parallel threads.
  If not specified, this is the default.''')
    parser.add_argument('forward',
                        action='store',
                        nargs='*',
                        type=str,
                        metavar='ARG',
                        help='arguments to be  forwarded to the subroutines')

    # run parser and do some postprocessing
    parsed, unknown = parser.parse_known_args(args)
    if parsed.output and str(parsed.output) == '__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__':
        if parsed.input[0].is_file():
            parsed.output = parsed.input[0].parent
        elif parsed.input[0].is_dir():
            parsed.output = parsed.input[0]
        else:
            raise Exception('output path must be specified')

    return parsed, unknown

def __removeOutputArgument__(args: list[str], path_only: bool = False):
    outpath = None
    for arg in range(len(args)):
        if args[arg].lower() == '-o' or args[arg].lower() == '--output':
            outpath = args[arg+1]
            args.pop(arg+1)
            if path_only == False:
                args.pop(arg)
            break
    return args, pathlib.Path(outpath)

def __retrieveFilesByFilter__(dir: pathlib.Path, regexp: str = '.*'):
    if dir == None:
        return []
    files = []
    for file in dir.iterdir():
        if file.is_file() and re.search(regexp, file.name):
            files.append(file)
    return files

def __executeBenchmark__(device: pathlib.Path, outdir: pathlib.Path = None, runs: int = 1, init: DeviceInitialization = DeviceInitialization.FIRST, uargs: list[str] = [], policy: CallingPolicy = CallingPolicy.INLINE):
    # check whether device is valid
    if not device.exists():
        raise FileExistsError('device path does not exist')
    if not device.is_char_device():
        raise ValueError('device is not valid')

    # prepare
    args = [str(device)] + uargs
    if outdir != None:
        args.extend(['--output', str(outdir)])
    if policy == CallingPolicy.ASYNC:
        workers = BunchOfWorkers()

    # execute all benchmark runs
    for run in range(0,runs):
        µRT.log('### EXECUTING BENCHMARK ' + str(run+1) + '/' + str(runs) + ' ###', flush=True)
        if (init == DeviceInitialization.FIRST and run == 0) or (init == DeviceInitialization.ALL):
            callargs = args + ['--initialize']
        else:
            callargs = args
        if policy == CallingPolicy.INLINE:
            ExecuteBenchmark.runWithArgs(args=callargs, ignore_unknown_args=True)
        elif policy == CallingPolicy.ASYNC:
            workers.setup()
            workers.addTask(func=ExecuteBenchmark.runWithArgs, kwargs={'args':callargs, 'ignore_unknown_args':True})
            workers.wait()
    rawfiles = __retrieveFilesByFilter__(outdir, '^.*\.log$')

    # if only a single log file was generated
    if len(rawfiles) == 1:
        # move log file to an according folder
        outpath = outdir.joinpath(rawfiles[0].stem)
        outpath.mkdir(exist_ok=True)
        µRT.waitForFilesystem(outpath)
        shutil.move(rawfiles[0], outpath.joinpath(rawfiles[0].name))
        rawfiles[0] = outpath.joinpath(rawfiles[0].name)
    # if multiple log files were generated
    elif len(rawfiles) > 1:
        for run in range(0,runs):
            # if this is the first file
            if run == 0:
                # create according folder
                outpath = outdir.joinpath(rawfiles[run].stem)
                outpath.mkdir(exist_ok=True)
                µRT.waitForFilesystem(outpath)
            # move file and rename
            rawfile = rawfiles[run]
            rawfiles[run] = outpath.joinpath('benchmark' + str(run+1).zfill(math.ceil(math.log10(runs+1))) + rawfile.suffix)
            shutil.move(rawfile, rawfiles[run])
    return rawfiles

def __parseBenchmarkLogs__(rawfiles: list[pathlib.Path], outdir: pathlib.Path = None, uargs: list[str] = [], policy: CallingPolicy = CallingPolicy.INLINE):
    # if no log files are specified
    if rawfiles == None or len(rawfiles) == 0:
        # return immediately
        return []

    # prepare
    if policy == CallingPolicy.ASYNC:
        workers = BunchOfWorkers()
        workers.setup()

    # if there is only a single log file
    if len(rawfiles) == 1:
        µRT.log('### PARSING BENCHMARK LOG ###', flush=True)
        args = [str(rawfiles[0])] + uargs
        if outdir != None:
            args.extend(['--output', str(outdir)])
        if policy == CallingPolicy.INLINE:
            ParseBenchmarkLog.runWithArgs(args=args, ignore_unknown_args=True)
        elif policy == CallingPolicy.ASYNC:
            workers.addTask(func=ParseBenchmarkLog.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
            workers.wait()
        parsedfiles = __retrieveFilesByFilter__(outdir, '^.*\.tsv$')

    # if there are multiple log files
    elif len(rawfiles) > 1:
        µRT.log('### PARSING BENCHMARK LOGS ###', flush=True)
        for rawfile in rawfiles:
            args = [str(rawfile)] + uargs
            if outdir != None:
                args.extend(['--output', str(outdir.joinpath(rawfile.stem))])
            if policy == CallingPolicy.INLINE:
                ParseBenchmarkLog.runWithArgs(args=args, ignore_unknown_args=True)
            elif policy == CallingPolicy.ASYNC:
                workers.addTask(func=ParseBenchmarkLog.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
        if policy == CallingPolicy.ASYNC:
            workers.wait()
        parsedfolders = []
        for rawfile in rawfiles:
            if outdir.joinpath(rawfile.stem).is_dir():
                parsedfolders.append(__retrieveFilesByFilter__(outdir.joinpath(rawfile.stem), '^.*\.tsv$'))

        µRT.log('### STITCHING EVALUATION FILES ###', flush=True)
        # sanity check number of files
        for folder in parsedfolders:
            if len(folder) != len(parsedfolders[0]):
                raise Exception('number of parsed files differs')
        # prepare
        if policy == CallingPolicy.ASYNC:
            workers.setup()
        # stitch all parsed files
        for f in range(0,len(parsedfolders[0])):
            # select files to stitch
            stitchfiles = []
            for folder in parsedfolders:
                stitchfiles.append(folder[f])
            # sanity check selected files
            for file in stitchfiles:
                if file.name != stitchfiles[0].name:
                    raise Exception('evaluation file names differ')
            # generate argument list
            args = []
            for file in stitchfiles:
                args.append(str(file))
            if outdir != None:
                args.extend(['--output', str(outdir.joinpath(stitchfiles[0].name))])
            if stitchfiles[0].stem.startswith(str(µRT.Benchmark.SYSTEM_KERNEL)):
                continue
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.SYSTEM_MODULE)):
                continue
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_MEMORY)):
                continue
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_NODE)):
                args.extend(['--sort', 'NODES', 'NODE'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_HOPS)):
                args.extend(['--sort', 'HOPS'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_PAYLOAD)):
                args.extend(['--sort', 'PAYLOAD'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_MESSAGE)):
                args.extend(['--sort', 'MESSAGES'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_PUBLISHER)):
                args.extend(['--sort', 'PUBLISHERS', 'PUBLISHER'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_TOPIC)):
                args.extend(['--sort', 'TOPICS', 'TOPIC'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_SUBSCRIBER)):
                args.extend(['--sort', 'SUBSCRIBERS', 'SUBSCRIBER'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE)):
                args.extend(['--sort', 'STEP'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_JITTER)):
                args.extend(['--sort', 'STEP'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_RATE)):
                args.extend(['--sort', 'STEP'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_HOPS)):
                args.extend(['--sort', 'HOPS'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_PAYLOAD)):
                args.extend(['--sort', 'PAYLOAD'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_REQUEST)):
                args.extend(['--sort', 'REQUESTS', 'REQUEST'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_SERVICE)):
                args.extend(['--sort', 'SERVICES', 'SERVICE'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_QOS_DEADLINE)):
                args.extend(['--sort', 'STEP'])
            elif stitchfiles[0].stem.startswith(str(µRT.Benchmark.URT_RPC_QOS_JITTER)):
                args.extend(['--sort', 'STEP'])
            args.extend(uargs)

            if policy == CallingPolicy.INLINE:
                StitchParsedData.runWithArgs(args=args, ignore_unknown_args=True)
            elif policy == CallingPolicy.ASYNC:
                workers.addTask(func=StitchParsedData.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
        if policy == CallingPolicy.ASYNC:
            workers.wait()
        parsedfiles = __retrieveFilesByFilter__(outdir, '^.*\.tsv$')

    return parsedfiles

def __plotParsedData__(parsedfiles: list[pathlib.Path], outdir: pathlib.Path = None, uargs: list[str] = [], policy: CallingPolicy = CallingPolicy.INLINE):
    # if no parsed files are specified
    if parsedfiles == None or len(parsedfiles) == 0:
        # return immediately
        return []

    µRT.log('### PLOTTING PARSED BENCHMARK DATA ###', flush=True)
    # prepare
    if policy == CallingPolicy.ASYNC:
        workers = BunchOfWorkers()
        workers.setup()

    # plot data
    for file in parsedfiles:
        # generate argument list
        args = [str(file)]
        if outdir != None:
            args.extend(['--output', str(outdir.joinpath(file.stem))])
        if file.name.startswith(str(µRT.Benchmark.SYSTEM_KERNEL)):
            pass
        elif file.name.startswith(str(µRT.Benchmark.SYSTEM_MODULE)):
            pass
        elif file.name.startswith(str(µRT.Benchmark.URT_MEMORY)):
            pass
        elif file.name.startswith(str(µRT.Benchmark.URT_NODE)):
            args.extend(['--independent-variables', 'NODES', 'NODE', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_HOPS)):
            args.extend(['--independent-variables', 'HOPS', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_PAYLOAD)):
            args.extend(['--independent-variables', 'PAYLOAD', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_MESSAGE)):
            args.extend(['--independent-variables', 'MESSAGES', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_PUBLISHER)):
            args.extend(['--independent-variables', 'PUBLISHERS', 'PUBLISHER', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_TOPIC)):
            args.extend(['--independent-variables', 'TOPICS', 'TOPIC', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_SUBSCRIBER)):
            args.extend(['--independent-variables', 'SUBSCRIBERS', 'SUBSCRIBER', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE)):
            args.extend(['--independent-variables', 'STEP', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_JITTER)):
            args.extend(['--independent-variables', 'STEP', '--columns', 'LATENCY', 'JITTER'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_RATE)):
            args.extend(['--independent-variables', 'STEP', '--columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_HOPS)):
            args.extend(['--independent-variables', 'HOPS', '--columns', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_PAYLOAD)):
            args.extend(['--independent-variables', 'PAYLOAD', '--columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_REQUEST)):
            args.extend(['--independent-variables', 'REQUESTS', 'REQUEST', '--columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_SERVICE)):
            args.extend(['--independent-variables', 'SERVICES', 'SERVICE', '--columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_QOS_DEADLINE)):
            args.extend(['--independent-variables', 'STEP', '--columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_QOS_JITTER)):
            args.extend(['--independent-variables', 'STEP', '--columns', 'DISPATCH', 'RETRIEVE', 'JITTER'])
        args.extend(uargs)

        if policy == CallingPolicy.INLINE:
            PlotParsedData.runWithArgs(args=args, ignore_unknown_args=True)
        elif policy == CallingPolicy.ASYNC:
            workers.addTask(func=PlotParsedData.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
    if policy == CallingPolicy.ASYNC:
        workers.wait()

    plotfiles = []
    for file in parsedfiles:
        if outdir.joinpath(file.stem).is_dir():
            plotfiles.extend(__retrieveFilesByFilter__(outdir.joinpath(file.stem), '^.*\.svg$'))
    return plotfiles

def __evaluateParsedData__(parsedfiles: list[pathlib.Path], outdir: pathlib.Path = None, uargs: list[str] = [], policy: CallingPolicy = CallingPolicy.INLINE):
    # if no parsed files are specified
    if parsedfiles == None or len(parsedfiles) == 0:
        # return immediately
        return []

    µRT.log('### EVALUATING BENCHMARK DATA ###', flush=True)
    # prepare
    if policy == CallingPolicy.ASYNC:
        workers = BunchOfWorkers()
        workers.setup()

    # evaluate data
    for file in parsedfiles:
        # generate argument list
        args = [str(file)]
        if outdir != None:
            args.extend(['--output', str(outdir)])
        if file.name.startswith(str(µRT.Benchmark.SYSTEM_KERNEL)):
            continue
        elif file.name.startswith(str(µRT.Benchmark.SYSTEM_MODULE)):
            continue
        elif file.name.startswith(str(µRT.Benchmark.URT_MEMORY)):
            continue
        elif file.name.startswith(str(µRT.Benchmark.URT_NODE)):
            args.extend(['--independent-variables', 'NODES', 'NODE', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_HOPS)):
            args.extend(['--independent-variables', 'HOPS', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_PAYLOAD)):
            args.extend(['--independent-variables', 'PAYLOAD', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_MESSAGE)):
            args.extend(['--independent-variables', 'MESSAGES', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_PUBLISHER)):
            args.extend(['--independent-variables', 'PUBLISHERS', 'PUBLISHER', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_TOPIC)):
            args.extend(['--independent-variables', 'TOPICS', 'TOPIC', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_SUBSCRIBER)):
            args.extend(['--independent-variables', 'SUBSCRIBERS', 'SUBSCRIBER', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE)):
            args.extend(['--independent-variables', 'STEP', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_JITTER)):
            args.extend(['--independent-variables', 'STEP', '--evaluation-columns', 'LATENCY', 'JITTER'])
        elif file.name.startswith(str(µRT.Benchmark.URT_PUBSUB_QOS_RATE)):
            args.extend(['--independent-variables', 'STEP', '--evaluation-columns', 'LATENCY'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_HOPS)):
            args.extend(['--independent-variables', 'HOPS', '--evaluation-columns', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_PAYLOAD)):
            args.extend(['--independent-variables', 'PAYLOAD', '--evaluation-columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_REQUEST)):
            args.extend(['--independent-variables', 'REQUESTS', 'REQUEST', '--evaluation-columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_SERVICE)):
            args.extend(['--independent-variables', 'SERVICES', 'SERVICE', '--evaluation-columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_QOS_DEADLINE)):
            args.extend(['--independent-variables', 'STEP', '--evaluation-columns', 'DISPATCH', 'RETRIEVE'])
        elif file.name.startswith(str(µRT.Benchmark.URT_RPC_QOS_JITTER)):
            args.extend(['--independent-variables', 'STEP', '--evaluation-columns', 'DISPATCH', 'RETRIEVE', 'JITTER'])
        args.extend(uargs)

        if policy == CallingPolicy.INLINE:
            EvaluateParsedData.runWithArgs(args=args, ignore_unknown_args=True)
        elif policy == CallingPolicy.ASYNC:
            workers.addTask(func=EvaluateParsedData.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
    if policy == CallingPolicy.ASYNC:
        workers.wait()

    return __retrieveFilesByFilter__(outdir, '^.*\.tsv$')

def __plotEvaluatedData__(evalfiles: list[pathlib.Path], outdir: pathlib.Path = None, uargs: list[str] = [], policy: CallingPolicy = CallingPolicy.INLINE):
    # if no evaluated files are specified
    if evalfiles == None or len(evalfiles) == 0:
        # return immediately
        return []

    µRT.log('### PLOTTING EVALUATED BENCHMARK DATA ###', flush=True)
    # prepare
    if policy == CallingPolicy.ASYNC:
        workers = BunchOfWorkers()
        workers.setup()

    # plot data
    for file in evalfiles:
        # generate argument list
        args = [str(file)]
        if outdir != None:
            args.extend(['--output', str(outdir)])
        args.extend(uargs)

        if policy == CallingPolicy.INLINE:
            PlotEvaluatedData.runWithArgs(args=args, ignore_unknown_args=True)
        elif policy == CallingPolicy.ASYNC:
            workers.addTask(func=PlotEvaluatedData.runWithArgs, kwargs={'args':args, 'ignore_unknown_args':True})
    if policy == CallingPolicy.ASYNC:
        workers.wait()

    return __retrieveFilesByFilter__(outdir, '^.*\.svg$')

#-------------------------------------------------------------------------------
# interface functions and classes
#-------------------------------------------------------------------------------

def runWithArgs(args: list[str] = None):
    args, forward_args = __parseArguments__(args=args)
    return execute(input=args.input[0],
                   outdir=args.output,
                   runs=args.runs[0],
                   init=DeviceInitialization.fromString(args.init_device[0]),
                   plots=args.plot,
                   mode=CallingPolicy.fromString(args.mode[0]),
                   uargs=forward_args)

def execute(input: pathlib.Path, outdir: pathlib.Path = None, runs: int = 1, init: DeviceInitialization = DeviceInitialization.FIRST, plots: list[str] = [], mode: CallingPolicy = CallingPolicy.INLINE, uargs: list[str] = []):

    # check whether input exists
    if not input.exists():
        raise FileExistsError('input path does not exist')
    # if input is a device: execute benchmarks
    elif input.is_char_device():
        rawfiles = __executeBenchmark__(device=input, outdir=outdir, runs=runs, init=init, uargs=uargs, policy=mode)
        if len(rawfiles) > 0:
            outdir = rawfiles[0].parent
    # if input is a file: load it
    elif input.is_file():
        rawfiles = [input]
    # if input is a directory: load all .log files
    elif input.is_dir():
        rawfiles = []
        for file in input.iterdir():
            if file.is_file() and file.suffix == '.log':
                rawfiles.append(file)
    # if input in incompatible
    else:
        raise ValueError('invalid input path')

    # parse log files
    if outdir == None:
        parseddir = None
    else:
        parseddir = outdir.joinpath('parsed')
    parsedfiles = __parseBenchmarkLogs__(rawfiles=rawfiles, outdir=parseddir, uargs=uargs, policy=mode)

    if 'parsed' in plots:
        # plot parsed data
        if outdir == None:
            parsedplotsdir = None
        else:
            parsedplotsdir = outdir.joinpath('parsed').joinpath('plots')
        __plotParsedData__(parsedfiles=parsedfiles, outdir=parsedplotsdir, uargs=uargs, policy=mode)

    # evaluate parsed data
    if outdir == None:
        evaldir = None
    else:
        evaldir = outdir.joinpath('evaluated')
    evalfiles = __evaluateParsedData__(parsedfiles=parsedfiles, outdir=evaldir, uargs=uargs, policy=mode)

    if 'evaluated' in plots:
        # plot evaluated data
        if outdir == None:
            evalplotsdir = None
        else:
            evalplotsdir = outdir.joinpath('evaluated').joinpath('plots')
        __plotEvaluatedData__(evalfiles=evalfiles, outdir=evalplotsdir, uargs=uargs, policy=mode)

    return rawfiles, parsedfiles, evalfiles

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
