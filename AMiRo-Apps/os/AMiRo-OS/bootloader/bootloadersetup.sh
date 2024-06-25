################################################################################
# AMiRo-OS is an operating system designed for the Autonomous Mini Robot       #
# (AMiRo) platform.                                                            #
# Copyright (C) 2016..2022  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU (Lesser) General Public License as published   #
# by the Free Software Foundation, either version 3 of the License, or         #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU (Lesser) General Public License for more details.                        #
#                                                                              #
# You should have received a copy of the GNU (Lesser) General Public License   #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.        #
#                                                                              #
# This research/work was supported by the Cluster of Excellence Cognitive      #
# Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is   #
# funded by the German Research Foundation (DFG).                              #
################################################################################

#!/bin/bash

# load library
source "$(dirname ${BASH_SOURCE[0]})/../tools/bash/setuplib.sh"

### print welcome text #########################################################
# Prints a welcome message to standard out.
#
# usage:      printWelcomeText
# arguments:  n/a
# return:     n/a
#
function printWelcomeText {
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "#             Welcome to the AMiRo-BLT submodule setup!              #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "# Copyright (c) 2016..2022  Thomas Schöpping                         #\n"
  printf "#                                                                    #\n"
  printf "# This is free software; see the source for copying conditions.      #\n"
  printf "# There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR  #\n"
  printf "# A PARTICULAR PURPOSE. The development of this software was         #\n"
  printf "# supported by the Excellence Cluster EXC 227 Cognitive Interaction  #\n"
  printf "# Technology. The Excellence Cluster EXC 227 is a grant of the       #\n"
  printf "# Deutsche Forschungsgemeinschaft (DFG) in the context of the German #\n"
  printf "# Excellence Initiative.                                             #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
}

### print help #################################################################
# Prints a help text to standard out.
#
# usage:      printHelp
# arguments:  n/a
# return:     n/a
#
function printHelp {
  printInfo "printing help text\n"
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--init] [-s|--setup] [-w|--wipe] [-q|--quit] [--log=<file>]\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --init\n"
  printf "              Initialize the AMiRo-BLT submodule.\n"
  printf "          -s, --setup\n"
  printf "              Enter AMiRo-BLT setup.\n"
  printf "          -w, --wipe\n"
  printf "              Wipe the entire AMiRo-BLT submodule.\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file> \n"
  printf "              Specify a log file.\n"
}

### initialize AMiRo-BLT submodule #############################################
# Initialize the AMiRo-BLT submodule.
#
# usage:      initAmiroBlt
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Arborted by user.
#             -1
#                 Error: Unexpected user input.
#             -2
#                 Error: Missing dependencies.
#
function initAmiroBlt {
  printInfo "initializing AMiRo-BLT submodule...\n"
  local userdir=$(pwd)
  local bootloaderdir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local amirobltdir=${bootloaderdir}/AMiRo-BLT

  # if the AMiRo-BLT folder is not empty
  if [ ! -z "$(ls -A $amirobltdir)" ]; then
    printWarning "$(realpath $amirobltdir) is not empty. Delete and reinitialize? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case $userinput in
      Y|y)
        wipeAmiroBlt
        ;;
      N|n)
        printWarning "AMiRo-BLT initialization aborted by user\n"
        return 1
        ;;
      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac
  fi

  # check dependencies
  checkCommands git
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # initialize submodule to default branch
  cd $bootloaderdir
  git submodule update --init $amirobltdir 2>&1 | tee -a $LOG_FILE
  while [ ${PIPESTATUS[0]} -ne 0 ]; do
    printWarning "initialitaion failed. Retry? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        git submodule update --init $amirobltdir 2>&1 | tee -a $LOG_FILE;;
      N|n)
        printWarning "AMiRo-BLT initialization aborted by user\n"
        cd $userdir
        return 1
        ;;
      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac
  done
  cd $userdir

  return 0
}

### enter AMiRo-BLT setup ######################################################
# Enter AMiRo-BLT submodule setup.
#
# usage:      amiroBltSetup
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             -1
#                 Error: AMiRo-BLT submodule not initialized yet.
#
function amiroBltSetup {
  local amirobltdir=$(dirname $(realpath ${BASH_SOURCE[0]}))/AMiRo-BLT

  # check if AMiRo-BLT submodule is initialized
  if [ -z "$(ls -A $amirobltdir)" ]; then
    printError "$amirobltdir is empty. Please initialize first.\n"
    return -1
  else
    printInfo "entering AMiRo-BLT setup\n"
    printf "\n"
    if [ -z "$LOG_FILE" ]; then
      ${amirobltdir}/setup.sh --noinfo
    else
      ${amirobltdir}/setup.sh --LOG="$LOG_FILE" --noinfo
    fi
    return 0
  fi
}

### reset AMiRo-BLT submodule and wipe directory ###############################
# Resets the AMiRo-BLT Git submodule and wipes the directory.
#
# usage:      wipeAmiroBlt
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: AMiRo-BLT Git submodule already empty
#             2
#                 Warning: Aborted by user.
#             -1
#                 Error: Unexpected input occurred
#             -2
#                 Error: Missing dependencies.
#
function wipeAmiroBlt {
  printInfo "reset and wipe Git submodule $amirobltdir\n"
  local userdir=$(pwd)
  local bootloaderdir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local amirobltdir=${bootloaderdir}/AMiRo-BLT

  # check dependencies
  checkCommands git
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # if the AMiRo-BLT folder is empty
  if [ -z "$(ls -A $amirobltdir)" ]; then
    printWarning "$amirobltdir is already empty\n"
    return 1
  else
    # get some information from Git
    cd $bootloaderdir
    local git_basehash=($(git ls-tree -d HEAD $amirobltdir)); git_basehash=${git_basehash[2]}
    cd $amirobltdir
    local git_branch_current=$(git rev-parse --abbrev-ref HEAD)
    local git_difftobase="$(git diff ${git_basehash}..HEAD)"
    local git_commits=$(git log --format=oneline ${git_basehash}..HEAD)
    local git_dirtyfiles=($(git ls-files -dmo --exclude-standard --exclude=/doc))
    cd $userdir
    local issues=0
    # if HEAD is ahead of submodule base commit
    if [ -n "$git_difftobase" ]; then
      issues=$((issues + 1))
      printWarning "HEAD is ahead of submodule base\n"
    fi
    # if there are untracked, modified, or deleted files
    if [ ${#git_dirtyfiles[@]} != 0 ]; then
      issues=$((issues + 1))
      printWarning "there are ${#git_dirtyfiles[@]} untracked, modified, or deleted files\n"
    fi
    if [ $issues -gt 0 ]; then
      local userinput=""
      printWarning "$issues issues detected. Do you want to continue? [y/n]\n"
      readUserSelection "YyNn" 'n' userinput
      case "$userinput" in
        Y|y)
          ;;
        N|n)
          printWarning "wiping AMiRo-BLT Git submodule aborted by user\n"
          return 2
          ;;
        *) # sanity check (return error)
          printError "unexpected input: $userinput\n"; return -1;;
      esac
    fi

    # checkout base commit and delete all local branches
    cd $bootloaderdir
    git submodule update --force --checkout $amirobltdir | tee -a $LOG_FILE
    cd $amirobltdir
    local git_branches=($(git for-each-ref --format="%(refname)"))
    for branch in $git_branches; do
      if [[ $branch = *"heads/"* ]]; then
        git branch -D ${branch##*/} | tee -a $LOG_FILE
      fi
    done
    cd $userdir

    # deinitialize AMiRo-BLT submodule and delete any remaining files
    cd $bootloaderdir
    git submodule deinit -f $amirobltdir 2>&1 | tee -a $LOG_FILE
    rm -rf $amirobltdir/*
    cd $userdir

    return 0
  fi
}

### main function of this script ###############################################
# Initializes or wipes the AMiRo-BLT Git submodule, and provides an entry point
# to its setup.
#
# usage:      see function printHelp
# arguments:  see function printHelp
# return:     0
#                 No error or warning occurred.
#
function main {
  # print welcome/info text if not suppressed
  if [[ $@ != *"--noinfo"* ]]; then
    printWelcomeText
  else
    printf "######################################################################\n"
  fi
  printf "\n"

  # if --help or -h was specified, print the help text and exit
  if [[ $@ == *"--help"* || $@ == *"-h"* ]]; then
    printHelp
    printf "\n"
    quitScript
  fi

  # set log file if specified
  if [[ $@ == *"--log"* ]] || [[ $@ == *"--LOG"* ]]; then
    # get the parameter (file name)
    local cmdidx=1
    while [[ ! "${!cmdidx}" = "--log"* ]] && [[ ! "${!cmdidx}" = "--LOG"* ]]; do
      cmdidx=$[cmdidx + 1]
    done
    local cmd="${!cmdidx}"
    local logfile=""
    if [[ "$cmd" = "--log="* ]] || [[ "$cmd" = "--LOG="* ]]; then
      logfile=${cmd#*=}
    else
      local filenameidx=$((cmdidx + 1))
      logfile="${!filenameidx}"
    fi
    # optionally force silent appending
    if [[ "$cmd" = "--LOG"* ]]; then
      setLogFile --option=c --quiet "$logfile" LOG_FILE
    else
      setLogFile "$logfile" LOG_FILE
      printf "\n"
    fi
  fi
  # log script name
  printLog "this is $(realpath ${BASH_SOURCE[0]})\n"

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -i|--init)
           initAmiroBlt; printf "\n"; shift 1;;
        -s|--setup)
           amiroBltSetup; printf "\n"; shift 1;;
        -w|--wipe)
           wipeAmiroBlt; printf "\n"; shift 1;;
        --quit)
          quitScript; shift 1;;
        --log=*|--LOG=*) # already handled; ignore
          shift 1;;
        --log|--LOG) # already handled; ignore
          shift 2;;
        --noinfo) # already handled; ignore
          shift 1;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # interactive menu
  while ( true ); do
    # main menu info prompt and selection
    printInfo "Bootloader setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "   [I]  - initialize AMiRo-BLT submodule\n"
    printf "   [S]  - enter AMiRo-BLT setup\n"
    printf "   [W]  - wipe AMiRo-BLT submodule\n"
    printf "  [ESC] - quit this setup\n"
    local userinput=""
    readUserSelection "IiSsWw" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        initAmiroBlt; printf "\n";;
      S|s)
        amiroBltSetup; printf "\n";;
      W|w)
        wipeAmiroBlt; printf "\n";;
      $'\e')
        quitScript;;
      *) # sanity check (exit with error)
        printError "unexpected argument: $userinput\n"
        exit 1
        ;;
    esac
  done

  # the script should never reach this point
  printError "script control flow corruppted.\n"
  exit 255
}

################################################################################
# SCRIPT ENTRY POINT                                                           #
################################################################################

main "$@"

