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
import datetime
import argparse
import pathlib
import commonclasses as µRT
import serial
import numpy as np
import os
import typing
import re
import time
import subprocess

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

def __parseArguments__(args: list[str] = None, ignore_unknown: bool = False):
    def str2bool(s):
        if str(s).upper() == 'FALSE':
            return False
        elif str(s).upper() == 'TRUE':
            return True
        else:
            try:
                return bool(int(s))
            except:
                raise ValueError('invalid argument')

    # setup parser
    parser = argparse.ArgumentParser(description='Execute a µRT benchmark on a device.',
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('port',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='PORT',
                        help='path to the device to interact with')
    parser.add_argument('-i', '--initialize',
                        action='store_true',
                        help='initialize (reset) device prior to the benchmark execution')
    parser.add_argument('-x', '--execute',
                        action='extend',
                        nargs='+',
                        type=str.lower,
                        choices=['all', 'kernel', 'module',
                                 'urt', 'memory', 'node',
                                 'pubsub', 'pubsub-hops', 'pubsub-payload', 'pubsub-message', 'pubsub-publsiher', 'pubsub-topic', 'pubsub-subscriber', 'pubsub-qos', 'pubsub-qos-deadline', 'pubsub-qos-jitter', 'pubsub-qos-rate',
                                 'rpc', 'rpc-hops', 'rpc-payload', 'rpc-request', 'rpc-service', 'rpc-qos', 'rpc-qos-deadline', 'rpc-qos-jitter'],
                        metavar='OPTION',
                        help='''benchmarks to execute
The following options are available:
- general
    all                  all benchmarks
    kernel               kernel (ChibiOS/RT) performance
    module               module performance metrics (AMiRo-OS)
- µRT: general
    uRT                  entire µRT
    memory               memory utilization of components
    node                 node performance
- µRT: pubslish-subscribe subsystem
    pubsub               entire publish-subscribe subsystem
    pubsub-hops          multi-hop latency
    pubsub-payload       payload impact
    pubsub-message       message performance
    pubsub-publisher     publsiher performance
    pubsub-topic         topic performance
    pubsub-subscriber    subscriber performance
    pubsub-qos           complete QoS verification
    pubsub-qos-deadline  deadline QoS verification
    pubsub-qos-jitter    jitter QoS verification
    pubsub-qos-rate      rate QoS verification
- µRT: RPC subsystem
    rpc                  entire ROC subsystem
    rpc-hops             multi-hop latency
    rpc-payload          payload impact
    rpc-request          request performance
    rpc-service          service performance
    rpc-qos              complete QoS verification
    rpc-qos-deadline     deadline QoS verification
    rpc-qos-jitter       jitter QoS verification''')
    parser.add_argument('-s', '--stress',
                        action='extend',
                        nargs='+',
                        type=str2bool,
                        choices=[True, False],
                        required=True,
                        help='whether to stress the CPU during benchmarks')
    parser.add_argument('-r', '--real-time',
                        action='extend',
                        nargs='+',
                        type=µRT.RTClass.fromstring,
                        choices=[µRT.RTClass.NRT, µRT.RTClass.SRT, µRT.RTClass.FRT, µRT.RTClass.HRT],
                        metavar='xRT',
                        help='''real-time scenarios to benchmark
The following options are available:
    NRT                  non-real-time
    SRT                  soft real-time
    FRT                  firm real-time
    HRT                  hard real-time
If not specified, all rea-time scenarios are executed.''')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='PATH',
                        help='''path to store the benchmark output log to
This path can be a file or a directory.''')
    parser.add_argument('-f', '--force',
                        action='store_true',
                        help='overwrite existing output log file')

    # run parser and do some postprocessing
    if ignore_unknown == True:
        parsed = parser.parse_known_args(args)[0]
    else:
        parsed = parser.parse_args(args)
    if not parsed.execute or len(parsed.execute) == 0:
        parsed.execute = ['all']
    execute = np.empty(shape=0, dtype=µRT.Benchmark)
    for e in parsed.execute:
        execute = np.append(execute, µRT.Benchmark.fromstring(e))
    parsed.execute = execute
    if not parsed.real_time or len(parsed.real_time) == 0:
        parsed.real_time = [µRT.RTClass.NRT, µRT.RTClass.SRT, µRT.RTClass.FRT, µRT.RTClass.HRT]

    return parsed

def __log__(text: str, out: typing.TextIO = None):
        if out:
            return out.write(text)
        else:
            return print(text, end='')

#-------------------------------------------------------------------------------
# interface functions and classes
#-------------------------------------------------------------------------------

def runWithArgs(args: list[str] = None, ignore_unknown_args: bool = False):
    args = __parseArguments__(args=args, ignore_unknown=ignore_unknown_args)
    return benchmark(port=args.port[0],
                     init=args.initialize,
                     benchmarks=args.execute,
                     stress=args.stress,
                     realtime=args.real_time,
                     outpath=args.output[0] if args.output else None,
                     force=args.force)

def benchmark(port: pathlib.Path, init: bool, benchmarks: list[µRT.Benchmark], realtime: list[µRT.RTClass], stress: list[bool], outpath: pathlib.Path = None, force: bool = False):
    # sanity check arguments
    if not port.exists():
        raise IOError('device path does not exist')
    elif not port.is_char_device():
        raise IOError('device path is no char device')

    # open output file
    if outpath:
        outpath = outpath.absolute()
        # if the output path is a directory
        if outpath.name == outpath.stem:
            # append a file name
            outpath = outpath.joinpath(datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S') + '.log')
        # check whether the file already exists but must not be overwritten
        if outpath.exists() and force == False:
            raise Exception('output file exists')
        # create parent directories as required and open file for writing
        outpath.parent.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outpath.parent)
        outfile = outpath.open(mode='w')
    else:
        outfile = None

    # log some Git related information
    cmd = 'git rev-parse HEAD'
    ret = subprocess.run((cmd[0:len(cmd.split(maxsplit=1)[0])] + ' -C ' + str(pathlib.Path(__file__).parent.absolute()) + cmd[len(cmd.split(maxsplit=1)[0]):]).split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if ret.returncode == 0:
        __log__(cmd + '\t' + ret.stdout.decode("utf-8").strip() + '\n', outfile)
        cmd = 'git status --short'
        ret = subprocess.run((cmd[0:len(cmd.split(maxsplit=1)[0])] + ' -C ' + str(pathlib.Path(__file__).parent.absolute()) + cmd[len(cmd.split(maxsplit=1)[0]):]).split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if ret.returncode == 0:
            if len(ret.stdout.decode("utf-8").strip()) == 0:
                __log__(cmd + '\tclean\n', outfile)
            else:
                __log__(cmd + '\tmodified\n', outfile)
        __log__('\n', outfile)

    # open device, discard any pending input
    device = serial.Serial()
    device.port = str(port)
    device.baudrate = 115200
    device.timeout = 0.1
    device.rts = False
    device.open()
    while device.in_waiting:
        device.readline()

    # reset device if requested
    if init == True:
        µRT.log('resetting device', flush=True)
        if re.search('.*AMiRo.*', device.port):
            # reset by toggling RTS on and off again
            device.rts = True
            time.sleep(1)
            device.rts = False
            time.sleep(1)
        else:
            # reset by shell command
            device.write(str('module:shutdown --restart\n').encode())
            # discard final output before restart
            while True:
                line = device.readline().decode(errors='ignore')
                if re.match('^#+$', line):
                    __log__(line, outfile)
                    break

    # log output and retrieve console prompt
    prompt = None
    while True:
        line = device.readline().decode(errors='ignore')
        if len(line) > 0:
            __log__(line, outfile)
            # last line is assumed to be the prompt
            prompt = line
        else:
            break
    # if no prompt could be retrieved yet (e.g. the device was not reset)
    if prompt == None:
        # send a newline character
        device.write(str('\n').encode())
        # read just sent newline character
        line = device.readline().decode(errors='ignore')
        # read next line (assumed to be the prompt)
        line = device.readline().decode(errors='ignore')
        if len(line) > 0:
            __log__(line, outfile)
            prompt = line
        else:
            raise Exception('failed to retrieve prompt')
    if outfile: outfile.flush()

    # build up list of shell commands
    commands = np.empty(shape=0, dtype=str)
    for benchmark in benchmarks:
        def appendStress(text: str, stress: bool):
            if stress == True:
                return text + ' --stress'
            else:
                return text

        if benchmark == µRT.Benchmark.SYSTEM_KERNEL or benchmark == µRT.Benchmark.SYSTEM_MODULE or benchmark == µRT.Benchmark.URT_MEMORY:
            commands = np.append(commands, benchmark.toShellCommand())
        elif benchmark == µRT.Benchmark.URT_NODE or \
             benchmark == µRT.Benchmark.URT_PUBSUB_QOS_DEADLINE or benchmark == µRT.Benchmark.URT_PUBSUB_QOS_JITTER or benchmark == µRT.Benchmark.URT_PUBSUB_QOS_RATE or \
             benchmark == µRT.Benchmark.URT_RPC_QOS_DEADLINE or benchmark == µRT.Benchmark.URT_RPC_QOS_JITTER:
            for s in stress:
                commands = np.append(commands, appendStress(benchmark.toShellCommand(), s))
        elif benchmark == µRT.Benchmark.URT_PUBSUB_HOPS or benchmark == µRT.Benchmark.URT_PUBSUB_PAYLOAD or benchmark == µRT.Benchmark.URT_PUBSUB_MESSAGE or benchmark == µRT.Benchmark.URT_PUBSUB_PUBLISHER or benchmark == µRT.Benchmark.URT_PUBSUB_TOPIC or benchmark == µRT.Benchmark.URT_PUBSUB_SUBSCRIBER or \
             benchmark == µRT.Benchmark.URT_RPC_HOPS or benchmark == µRT.Benchmark.URT_RPC_PAYLOAD or benchmark == µRT.Benchmark.URT_RPC_REQUEST or benchmark == µRT.Benchmark.URT_RPC_SERVICE:
            for rt in realtime:
                for s in stress:
                    commands = np.append(commands, appendStress(benchmark.toShellCommand() + ' ' + str(rt), s))
        else:
            raise ValueError('invalid benchmark')

    # execute commands
    if outfile != None:
        µRT.log('writing raw log to file: ' + str(outpath), flush=True)
    for cmd in commands:
        if outfile:
            µRT.log('executing command: ' + cmd, flush=True)
        device.write(str(cmd + '\n').encode())
        while True:
            line = device.readline().decode(errors='ignore')
            if len(line) > 0:
                __log__(line, outfile)
            if line == prompt:
                break
        if outfile:
            outfile.flush()

    # outro
    device.close()
    if outfile:
        outfile.close()
        μRT.waitForFilesystem(outfile)
        return pathlib.Path(outfile.name)
    else:
        print('', flush=True) # flush linebreak
        return None

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
