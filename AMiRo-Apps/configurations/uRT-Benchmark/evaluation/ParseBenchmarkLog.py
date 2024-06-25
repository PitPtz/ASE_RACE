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
import re
import numpy as np
import pathlib
import enum
import commonclasses as µRT
import pandas
import os

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    # setup parser
    parser = argparse.ArgumentParser(description='Parse raw µRT benchmark output.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('infile',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='LOGFILE',
                        help='path to a benchmark log file to parse')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs='?',
                        type=pathlib.Path,
                        const=pathlib.Path('__UNSPECIFIED_OUTPUT_DIRECTORY_PATH__'),
                        metavar='DIR',
                        help='path to a directory to store data files in')
    parser.add_argument('-f', '--force',
                        action='store_true',
                        help='overwrite existing output directory')

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
    return parse(logfile=args.infile[0],
                 outdir=args.output if args.output else None,
                 force=args.force)

def parse(logfile: pathlib.Path, outdir: pathlib.Path = None, force: bool = False):
    class State:
        class Domain(enum.Enum):
            SYSTEM      = enum.auto()
            URT         = enum.auto()
            URT_PUBSUB  = enum.auto()
            URT_RPC     = enum.auto()

        def __init__(self, domain: Domain = None, benchmark: µRT.Benchmark = None, realtime: µRT.RTClass = None, stress: bool = None):
            self.domain = domain
            self.benchmark = benchmark
            self.realtime = realtime
            self.stress = stress

        def fromCommand(cmd: str):
            def detectStress(s: str):
                if re.search('\s(--stress|-s)', s):
                    return True
                else:
                    return False

            def retrieveRT(s: str):
                if str(µRT.RTClass.NRT) in s.upper():
                    return µRT.RTClass.NRT
                elif str(µRT.RTClass.SRT) in s.upper():
                    return µRT.RTClass.SRT
                elif str(µRT.RTClass.FRT) in s.upper():
                    return µRT.RTClass.FRT
                elif str(µRT.RTClass.HRT) in s.upper():
                    return µRT.RTClass.HRT
                else:
                    return None

            if cmd.startswith(µRT.Benchmark.SYSTEM_KERNEL.toShellCommand()):
                return State(domain=State.Domain.SYSTEM, benchmark=µRT.Benchmark.SYSTEM_KERNEL)
            elif cmd.startswith(µRT.Benchmark.SYSTEM_MODULE.toShellCommand()):
                return State(domain=State.Domain.SYSTEM, benchmark=µRT.Benchmark.SYSTEM_MODULE)
            elif cmd.startswith(µRT.Benchmark.URT_MEMORY.toShellCommand()):
                return State(domain=State.Domain.URT, benchmark=µRT.Benchmark.URT_MEMORY)
            elif cmd.startswith(µRT.Benchmark.URT_NODE.toShellCommand()):
                return State(domain=State.Domain.URT, benchmark=µRT.Benchmark.URT_NODE, stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_HOPS.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_HOPS, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_PAYLOAD.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_PAYLOAD, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_MESSAGE.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_MESSAGE, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_PUBLISHER.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_PUBLISHER, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_TOPIC.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_TOPIC, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_SUBSCRIBER.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_SUBSCRIBER, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE, realtime=µRT.RTClass.HRT, stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_QOS_JITTER.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_QOS_JITTER, realtime=µRT.RTClass.HRT, stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_PUBSUB_QOS_RATE.toShellCommand()):
                return State(domain=State.Domain.URT_PUBSUB, benchmark=µRT.Benchmark.URT_PUBSUB_QOS_RATE, realtime=µRT.RTClass.HRT, stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_HOPS.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_HOPS, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_PAYLOAD.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_PAYLOAD, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_REQUEST.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_REQUEST, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_SERVICE.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_SERVICE, realtime=retrieveRT(cmd), stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_QOS_DEADLINE.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_QOS_DEADLINE, realtime=µRT.RTClass.HRT, stress=detectStress(cmd))
            elif cmd.startswith(µRT.Benchmark.URT_RPC_QOS_JITTER.toShellCommand()):
                return State(domain=State.Domain.URT_RPC, benchmark=µRT.Benchmark.URT_RPC_QOS_JITTER, realtime=µRT.RTClass.HRT, stress=detectStress(cmd))
            else:
                return State(stress=detectStress(cmd))

        def set(self, domain: Domain = None, benchmark: µRT.Benchmark = None, realtime: µRT.RTClass = None, stress: bool = None):
            if domain != None:
                self.domain = domain
            if benchmark != None:
                self.benchmark = benchmark
            if realtime != None:
                self.realtime = realtime
            if stress != None:
                self.stress = stress
            return self

        def reset(self, domain: bool = False, benchmark: bool = False, realtime: bool = False, stress: bool = False):
            if domain == True:
                self.domain = None
            if benchmark == True:
                self.benchmark = None
            if realtime == True:
                self.realtime = None
            if stress == True:
                self.stress = None
            if domain == False and benchmark == False and realtime == False and stress == False:
                self.reset(domain=True, benchmark=True, realtime=True, stress=True)
            return self

    def logData():
        # abort if no benchmark is currently set
        if state.benchmark == None:
            return None
        elif state.benchmark == µRT.Benchmark.SYSTEM_KERNEL:
            # kernel benchmark data is not logged yet
            return None
        elif state.benchmark == µRT.Benchmark.SYSTEM_MODULE:
            # module benchmark data is not logged yet
            return None

        # check data
        if len(data.index) == 0:
            raise ValueError('no data')

        # generate filename
        filename = str(state.benchmark)
        if state.realtime != None:
            filename = filename + '_' + str(state.realtime)
        if state.stress != None:
            if state.stress == True:
                filename = filename + '_stress'
            else:
                filename = filename + '_nostress'
        filename = filename + '.tsv'

        # set some formating information
        if state.benchmark == µRT.Benchmark.URT_MEMORY:
            print_header = True
            print_intex = True
        else:
            print_header = True
            print_intex = False

        # log data
        outfile = None
        if outdir != None:
            outfile = outdir.joinpath(filename)
            µRT.log('writing parsed log to file: ' + str(outfile), flush=True)
            # check whether file already exists
            if outfile.exists() and force == False:
                print('\tFailed. Use option \'--force\' to overwrite existing files.', flush=True)
                outfile = None
            else:
                data.to_csv(path_or_buf=outfile, sep='\t', header=print_header, index=print_intex)
        else:
            # print to stdout
            print('>>>\t' + filename)
            print(data.to_csv(path_or_buf=None, sep='\t', header=print_header, index=print_intex), end='', flush=True)

        # log meta data (if any)
        if metadata != None:
            # prepare text
            text = ''
            for label, value in metadata.items():
                if label == 'critical':
                    text = text + label + ' (us)\t' + str(value) + os.linesep
                elif label == 'violations':
                    text = text + label + ' (x of N)\t' + str(value[0]) + '\t' + str(value[1]) + os.linesep
                else:
                    text = text + str(label) + '\t' + str(value) + os.linesep

            if outfile != None:
                # write to file
                file = open(outfile, 'a')
                file.write(os.linesep + text)
                file.close()
            else:
                # print to stdout
                print(text, end='')

        # clear data and return
        data.drop(axis='columns', columns=data.columns, inplace=True)
        data.drop(axis='index', index=data.index, inplace=True)

        return outfile

    # sanity check arguments
    if not logfile.exists():
        raise FileExistsError('logfile does not exist')
    elif not logfile.is_file():
        raise Exception('logfile is no file')

    # create output directory
    if outdir != None:
        # create directory if it does not exist yet
        outdir.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outdir)

    # read log file and retrieve prompt string
    logfile = logfile.open(mode='r')
    for line in logfile:
        if re.match('^.*\$ +\S*', line):
            prompt = re.match('^.*\$ ', line).group(0)
            break;
    logfile.seek(0)

    # read logfile line by line and execute FSM
    state = State()
    data = pandas.DataFrame(dtype=np.int32)
    metadata = {}
    linenumber = 0
    outfiles = []
    for line in logfile:
        line = line.strip()

        # detect and interprete prompt & commands
        if line.startswith(prompt) and line != prompt:
            if len(data.index) > 0:
                outfiles.append(logData())
                metadata = {}
            command = line.removeprefix(prompt)
            state = State.fromCommand(command)

        # detect and interprete domain headings
        elif re.match('^#+ [a-zA-Z0-9 -_]+ #+$', line):
            if len(data.index) > 0:
                outfiles.append(logData())
                metadata = {}
            if re.match('^#+ MEMORY #+$', line):
                state = state.set(domain=State.Domain.URT, benchmark=µRT.Benchmark.URT_MEMORY)
            elif re.match('^#+ NODE #+$', line):
                state = state.set(domain=State.Domain.URT, benchmark=µRT.Benchmark.URT_NODE)
            elif re.match('^#+ PUBLISH\-SUBSCRIBE #+$', line):
                state = state.set(domain=State.Domain.URT_PUBSUB)
                state = state.reset(benchmark=True, realtime=True)
            elif re.match('^#+ RPC #+$', line):
                state = state.set(domain=State.Domain.URT_RPC)
                state = state.reset(benchmark=True, realtime=True)

        # detect and interprete benchmark headings
        elif re.match('^-+ [a-zA-Z0-9 -_]+( \([NSFH]RT\))? -+$', line):
            if len(data.index) > 0:
                outfiles.append(logData())
                metadata = {}
            if re.match('^-+ HOPS \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_HOPS)
                elif state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_HOPS)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ PAYLOAD \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_PAYLOAD)
                elif state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_PAYLOAD)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ MESSAGE \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_MESSAGE)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ PUBLISHER \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_PUBLISHER)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ TOPIC \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_TOPIC)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ SUBSCRIBER \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_SUBSCRIBER)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ REQUEST \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_REQUEST)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ SERVICE \([NSFH]RT\) -+$', line):
                if state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_SERVICE)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ DEADLINE -+$', line):
                metadata = {'critical': None, 'violations': None}
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE)
                elif state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_QOS_DEADLINE)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ JITTER -+$', line):
                metadata = {'critical': None, 'violations': None}
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_QOS_JITTER)
                elif state.domain == State.Domain.URT_RPC:
                    state = state.set(benchmark=µRT.Benchmark.URT_RPC_QOS_JITTER)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')
            elif re.match('^-+ RATE -+$', line):
                metadata = {'critical': None, 'violations': None}
                if state.domain == State.Domain.URT_PUBSUB:
                    state = state.set(benchmark=µRT.Benchmark.URT_PUBSUB_QOS_RATE)
                else:
                    raise Exception('implausible log file (line ' + linenumber + ')')

            # extract the real-time class
            if '('+str(µRT.RTClass.NRT)+')' in line.upper():
                state = state.set(realtime=µRT.RTClass.NRT)
            elif '('+str(µRT.RTClass.SRT)+')' in line.upper():
                state = state.set(realtime=µRT.RTClass.SRT)
            elif '('+str(µRT.RTClass.FRT)+')' in line.upper():
                state = state.set(realtime=µRT.RTClass.FRT)
            elif '('+str(µRT.RTClass.HRT)+')' in line.upper():
                state = state.set(realtime=µRT.RTClass.HRT)
            else:
                state = state.reset(realtime=True)

        # read the data
        else:
            def parseTable(data: pandas.DataFrame = None):
                if re.match('^CPU stress:\s+(enabled|disabled)$', line):
                    if (line.endswith('enabled') and state.stress != True) or (line.endswith('disabled') and state.stress != False):
                        raise Exception('CPU stress mismatch')
                elif re.match('^[A-Z]+(\t[A-Z]+)+$', line):
                    data = pandas.DataFrame(columns=line.split(), dtype=np.int32)
                elif re.match('^\s*\d+(\s+\d+)+$', line):
                    data.loc[len(data.index)] = np.array(re.findall('\d+', line), dtype=np.int32)
                return data

            def parseMetaData(mdata: dict):
                if re.match('^critical [a-z]+: \d+ us$', line):
                    mdata['critical'] = int(re.findall('\d+', line)[0])
                elif re.match('^violations: \d+/\d+$', line):
                    mdata['violations'] = [int(val) for val in re.findall('\d+', line)]
                return mdata

            if state.benchmark == None:
                # do nothing
                pass
            elif state.benchmark == µRT.Benchmark.SYSTEM_KERNEL:
                # not evaluated yet
                pass
            elif state.benchmark == µRT.Benchmark.SYSTEM_MODULE:
                # not evaluated yet
                pass
            elif state.benchmark == µRT.Benchmark.URT_MEMORY:
                if re.match('^[A-Z/]+(\t[A-Z]+)+$', line):
                    data = pandas.DataFrame(columns=line.split('\t')[1:], index={line.split('\t')[0]: None}, dtype=np.int32)
                elif re.match('^[a-zA-Z ]+(\t\d+)+$', line):
                    data.loc[line.split('\t')[0]] = np.array(re.findall('\d+', line), dtype=np.int32)
            elif state.benchmark == µRT.Benchmark.URT_NODE:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_HOPS:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_PAYLOAD:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_MESSAGE:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_PUBLISHER:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_TOPIC:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_SUBSCRIBER:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE:
                data = parseTable(data)
                metadata = parseMetaData(metadata)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_QOS_JITTER:
                data = parseTable(data)
                metadata = parseMetaData(metadata)
            elif state.benchmark == µRT.Benchmark.URT_PUBSUB_QOS_RATE:
                data = parseTable(data)
                metadata = parseMetaData(metadata)
            elif state.benchmark == µRT.Benchmark.URT_RPC_HOPS:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_RPC_PAYLOAD:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_RPC_REQUEST:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_RPC_SERVICE:
                data = parseTable(data)
            elif state.benchmark == µRT.Benchmark.URT_RPC_QOS_DEADLINE:
                data = parseTable(data)
                metadata = parseMetaData(metadata)
            elif state.benchmark == µRT.Benchmark.URT_RPC_QOS_JITTER:
                data = parseTable(data)
                metadata = parseMetaData(metadata)
            else:
                raise ValueError('invalid benchmark')

    # close logfile and log remaining data (in any)
    logfile.close()
    if len(data.index) > 0:
        outfiles.append(logData())
        metadata = {}

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
