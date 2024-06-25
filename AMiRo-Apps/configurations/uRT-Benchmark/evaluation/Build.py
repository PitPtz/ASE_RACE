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
import subprocess
import re
import pandas

import commonclasses as µRT

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
    parser.add_argument('modules',
                        action='store',
                        nargs='+',
                        type=str,
                        metavar='MODULE',
                        help='names of the modules to evaluate')
    parser.add_argument('-o', '--output',
                        action='store',
                        nargs=1,
                        type=pathlib.Path,
                        metavar='PATH',
                        help='''path to store the benchmark output table to
This path must be a directory.''')
    parser.add_argument('-f', '--force',
                        action='store_true',
                        help='overwrite existing output file')

    # run parser and do some postprocessing
    if ignore_unknown == True:
        parsed = parser.parse_known_args(args)[0]
    else:
        parsed = parser.parse_args(args)

    return parsed

def __runMake__(modulename: str, osonly: bool, udefs: str = None):
    testmodule = pathlib.Path(__file__).parent.joinpath('testmodule')

    # execute 'make' and build the module
    cmd = 'make -B -j'
    cmd += ' -C ' + str(testmodule)
    cmd += ' TESTMODULE=' + modulename
    cmd += ' TESTMODULE_OSONLY=' + ('yes' if osonly else 'no')
    if udefs != None:
        cmd += ' UDEFS="' + udefs + '"'
    cmd += ' all'
    make_all = subprocess.run(cmd, shell=True, capture_output=True, text=True)

    # execute 'make' again to clean up
    cmd = 'make'
    cmd += ' -C ' + str(testmodule)
    cmd += ' TESTMODULE=' + modulename
    cmd += ' TESTMODULE_OSONLY=' + ('yes' if osonly else 'no')
    if udefs != None:
        cmd += ' UDEFS="' + udefs + '"'
    cmd += ' clean'
    subprocess.run(cmd, shell=True, capture_output=True, text=True)

    # check exit status
    if make_all.returncode != 0:
        µRT.log(make_all.stderr)
        return None

    # search the output for the size data
    image_size = {'text': None, 'data': None, 'bss': None}
    state = 'search'
    for line in make_all.stdout.splitlines():
        if state == 'search':
            if re.match('^ *text\t *data\t *bss\t *dec\t *hex\t *filename$', line.strip()):
                state = 'found'
        elif state == 'found':
            line = line.split('\t')
            image_size['text'] = int(line[0].strip())
            image_size['data'] = int(line[1].strip())
            image_size['bss'] = int(line[2].strip())
            state = 'done'
            break
    
    # check state and return
    if state != 'done':
        return None
    else:
        return image_size
    
def __generateUdefs__(syncgroups:bool, debug:bool, pubsub:bool, pubsub_profiling:bool, pubsub_qos:bool, rpc:bool, rpc_profiling:bool, rpc_qos:bool):
    udefs = '-DURT_CFG_SYNCGROUPS_ENABLED=' + ('true' if syncgroups else 'false')
    udefs += ' -DURT_CFG_DEBUG_ENABLED=' + ('true' if debug else 'false')
    udefs += ' -DURT_CFG_PUBSUB_ENABLED=' + ('true' if pubsub else 'false')
    udefs += ' -DURT_CFG_PUBSUB_PROFILING=' + ('true' if pubsub_profiling else 'false')
    udefs += ' -DURT_CFG_PUBSUB_QOS_DEADLINECHECKS=' + ('true' if pubsub_qos else 'false')
    udefs += ' -DURT_CFG_PUBSUB_QOS_JITTERCHECKS=' + ('true' if pubsub_qos else 'false')
    udefs += ' -DURT_CFG_PUBSUB_QOS_RATECHECKS=' + ('true' if pubsub_qos else 'false')
    udefs += ' -DURT_CFG_RPC_ENABLED=' + ('true' if rpc else 'false')
    udefs += ' -DURT_CFG_RPC_PROFILING=' + ('true' if rpc_profiling else 'false')
    udefs += ' -DURT_CFG_RPC_QOS_DEADLINECHECKS=' + ('true' if rpc_qos else 'false')
    udefs += ' -DURT_CFG_RPC_QOS_JITTERCHECKS=' + ('true' if rpc_qos else 'false')

    return udefs

#-------------------------------------------------------------------------------
# interface functions and classes
#-------------------------------------------------------------------------------

def runWithArgs(args: list[str] = None, ignore_unknown_args: bool = False):
    args = __parseArguments__(args=args, ignore_unknown=ignore_unknown_args)
    return build(modulenames=args.modules,
                 outdir=args.output[0] if args.output else None,
                 force=args.force)

def build(modulenames: list[str], outdir: pathlib.Path = None, force: bool = False):
    # create outpt directory
    if outdir != None:
        # create output directory as needed
        outdir.mkdir(parents=True, exist_ok=True)
        µRT.waitForFilesystem(outdir)

    # repeast for all modules
    outfiles = []
    for module in modulenames:

        # sanity check arguments
        if outdir != None:
            # check whether a directory was specified
            if outdir.name != outdir.stem:
                raise Exception('output path must be a directory')
            # create directory if required
            outdir.mkdir(parents=True, exist_ok=True)

            outfile = outdir.joinpath(module + '.tsv')
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

        # print log
        µRT.log('building module: ' + module, flush=True)

        # prepare the data frame to store all values to
        build_sizes = pandas.DataFrame(index=pandas.MultiIndex(names=['component','debug','QoS','profiling'],
                                                            levels=[['OS','core','syncgroups','publish-subscribe','RPC','complete'],
                                                                    [False,True,pandas.NA],
                                                                    [False,True,pandas.NA],
                                                                    [False,True,pandas.NA]],
                                                            codes=[[ 0, 1, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5],
                                                                    [-1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1],
                                                                    [-1,-1,-1,-1,-1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1],
                                                                    [-1,-1,-1,-1,-1, 0, 1, 0, 1, 0, 1, 0 ,1, 0, 1, 0, 1, 0, 1, 0 ,1, 0, 1, 0, 1, 0, 1, 0 ,1]]),
                                       columns=pandas.MultiIndex.from_product([['text','data','bss'],['absolute','difference']]))

        # build one configuration at a time
        for configuration in build_sizes.index:
            cfg = dict(zip(build_sizes.index.names,configuration))
            build_size = __runMake__(modulename=module,
                                    osonly=True if cfg['component'] == 'OS' else False,
                                    udefs=__generateUdefs__(syncgroups=True if (cfg['component'] == 'syncgroups' or cfg['component'] == 'complete') else False,
                                                            debug=True if cfg['debug'] == True else False,
                                                            pubsub=True if (cfg['component'] == 'publish-subscribe' or cfg['component'] == 'complete') else False,
                                                            pubsub_profiling=True if (cfg['component'] == 'publish-subscribe' or cfg['component'] == 'complete') and cfg['profiling'] == True else False,
                                                            pubsub_qos=True if (cfg['component'] == 'publish-subscribe' or cfg['component'] == 'complete') and cfg['QoS'] == True else False,
                                                            rpc=True if (cfg['component'] == 'RPC' or cfg['component'] == 'complete') else False,
                                                            rpc_profiling=True if (cfg['component'] == 'RPC' or cfg['component'] == 'complete') and cfg['profiling'] == True else False,
                                                            rpc_qos=True if (cfg['component'] == 'RPC' or cfg['component'] == 'complete') and cfg['QoS'] == True else False))

            # store results in the data frame
            if not build_size is None:
                build_sizes.loc[configuration,('text','absolute')] = build_size['text']
                build_sizes.loc[configuration,('data','absolute')] = build_size['data']
                build_sizes.loc[configuration,('bss','absolute')] = build_size['bss']
            else:
                build_sizes.loc[configuration,('text','absolute')] = pandas.NA
                build_sizes.loc[configuration,('data','absolute')] = pandas.NA
                build_sizes.loc[configuration,('bss','absolute')] = pandas.NA

        # calculare relative differences compared to baseline (OS)
        baseline_index = ('OS',pandas.NA,pandas.NA,pandas.NA)
        for index in build_sizes.index:
            for column in build_sizes.columns.levels[0]:
                # skip unavailable data points
                if pandas.isna(build_sizes.loc[index,(column,'absolute')]):
                    continue
                build_sizes.loc[index,(column,'difference')] = build_sizes.loc[index,(column,'absolute')] - build_sizes.loc[baseline_index,(column,'absolute')]

        if outfile != None:
            # write to file
            µRT.log('writing build size table to file: ' + str(outfile))
            build_sizes.to_csv(path_or_buf=outfile, sep='\t', header=True, index=True)
            µRT.waitForFilesystem(outfile)
            outfiles.append(outfile)
        else:
            # print to stdout
            print(build_sizes.to_csv(None, sep='\t', header=True, index=True), end='', flush=True)

    return outfiles

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------

if os.path.realpath(sys.argv[0]) == os.path.realpath(__file__):
    try:
        runWithArgs(sys.argv[1:])
    except BaseException as e:
        print('ERROR: ' + str(e))
