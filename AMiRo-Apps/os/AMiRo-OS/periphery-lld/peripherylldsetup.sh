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
  printf "#             Welcome to the AMiRo-LLD submodule setup!              #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--init] [-w|--wipe] [-q|--quit] [--log=<file>]\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --init\n"
  printf "              Initialize AMiRo-LLD submodule.\n"
  printf "          -d, --documentation <option>\n"
  printf "              Possible options are: 'g' and 'o' (can be combined).\n"
  printf "              - g: Generate HTML documentation of AMiRo-LLD.\n"
  printf "              - o: Open HTML documentation of AMiRo-LLD.\n"
  printf "          -w, --wipe\n"
  printf "              Wipe AMiRo-LLD submodule.\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### initialize AMiRo-LLD submodule #############################################
# Initialize the AMiRo-LLD submodule.
#
# usage:      initAmiroLld
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
function initAmiroLld {
  printInfo "initializing AMiRo-LLD submodule...\n"
  local userdir=$(pwd)
  local peripheryllddir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local amirollddir=${peripheryllddir}/AMiRo-LLD

  # if the AMiRo-LLD folder is not empty
  if [ ! -z "$(ls -A $amirollddir)" ]; then
    printWarning "$(realpath $amirollddir) is not empty. Delete and reinitialize? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case $userinput in
      Y|y)
        wipeAmiroLld
        ;;
      N|n)
        printWarning "AMiRo-LLD initialization aborted by user\n"
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
  cd $peripheryllddir
  git submodule update --init $amirollddir 2>&1 | tee -a $LOG_FILE
  while [ ${PIPESTATUS[0]} -ne 0 ]; do
    printWarning "initialitaion failed. Retry? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        git submodule update --init $amirollddir 2>&1 | tee -a $LOG_FILE;;
      N|n)
        printWarning "AMiRo-LLD initialization aborted by user\n"
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

### AMiRo-LLD dcoumentation setup ##############################################
#
# usage:      documentation [<option>]
# arguments:  <option>
#                 Can be either 'g' or 'o' to generate or open HTML documentation respectively.
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: AMiRo-LLD not nitialized yet.
#             2
#                 Warning: Setup aborted by user.
#             3
#                 Warning: Issues occurred.
#             -1
#                 Error: Unexpected user input.
#             -2
#                 Error: Missing dependencies.
#
function documentation {
  local userdir=$(pwd)
  local peripheryllddir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local amirollddir=${peripheryllddir}/AMiRo-LLD

  # if the AMiRo-LLD folder is empty
  if [ -z "$(ls -A $amirollddir)" ]; then
    printWarning "$amirollddir is empty. Please initialize first.\n"
    return 1
  else
    local option="";
    # if no argument was specified, ask what to do
    if [ $# -eq 0 ]; then
      printInfo "AMiRo-LLD documentation setup\n"
      printf "Please select one of the following actions:\n"
      printf "   [G]  - generate HTML documentation\n"
      printf "   [O]  - open HTML documentation\n"
      printf "  [ESC] - abort this setup\n"
      local userinput
      readUserSelection "GgOo" 'y' userinput
      option=${userinput,,}
      if [ $option = $'\e' ]; then
        printInfo "AMiRo-LLD documentation setup aborted by user\n"
        return 2
      fi
    else
      option="$1"
    fi

    local issues=0
    case "$option" in
      # generate HTML documentation
      G|g)
        # check dependencies
        checkCommands doxygen
        if [ $? -ne 0 ]; then
          printError "Missing dependencies detected.\n"
          return -2
        fi
        # check if required files exist
        if [ -f ${amirollddir}/docs/doxygen/Doxyfile ]; then
          printInfo "generating AMiRo-LLD documentation...\n"
          cd ${amirollddir}/docs/doxygen/
          doxygen Doxyfile 2>&1 | tee -a $LOG_FILE &> /dev/null
          cd $userdir
          printInfo "access AMiRo-LLD documentation via ${amirollddir}docs/doxygen/output/html/index.html\n"
        else
          issues=$((issues + 1))
          printError "could not generate AMiRo-LLD documentation\n"
        fi
        ;;

      # open HTML documentation
      O|o)
        # check if required files exist
        if [ -f ${amirollddir}/docs/doxygen/output/html/index.html ]; then
          printInfo "open AMiRo-LLD documentation\n"
          xdg-open ${amirollddir}/docs/doxygen/output/html/index.html &> /dev/null &
        else
          issues=$((issues + 1))
          printError "could not open AMiRo-LLD documentation\n"
        fi
        ;;

      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac

    if [ $issues -gt 0 ]; then
      return 3
    else
      return 0
    fi
  fi
}

### reset AMiRo-LLD submodule and wipe directory ###############################
# Resets the AMiRo-LLD Git submodule and wipes the directory.
#
# usage:      wipeAmiroLld
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: AMiRo-LLD Git submodule already empty
#             2
#                 Warning: Aborted by user.
#             -1
#                 Error: Unexpected input occurred.
#             -2
#                 Error: Missing dependencies.
#
function wipeAmiroLld {
  printInfo "reset and wipe Git submodule $amirollddir\n"
  local userdir=$(pwd)
  local peripheryllddir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local amirollddir=${peripheryllddir}/AMiRo-LLD

  # check dependencies
  checkCommands git
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # if the AMiRo-LLD folder is empty
  if [ -z "$(ls -A $amirollddir)" ]; then
    printWarning "$amirollddir is already empty\n"
    return 1
  else
    # get some information from Git
    local userdir=$(pwd)
    cd $peripheryllddir
    local git_basehash=($(git ls-tree -d HEAD $amirollddir)); git_basehash=${git_basehash[2]}
    cd $amirollddir
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
          printWarning "wiping AMiRo-LLD Git submodule aborted by user\n"
          return 2
          ;;
        *) # sanity check (return error)
          printError "unexpected input: $userinput\n"; return -1;;
      esac
    fi

    # checkout base commit and delete all local branches
    cd $peripheryllddir
    git submodule update --force --checkout $amirollddir | tee -a $LOG_FILE
    cd $amirollddir
    local git_branches=($(git for-each-ref --format="%(refname)"))
    for branch in $git_branches; do
      if [[ $branch = *"heads/"* ]]; then
        git branch -D ${branch##*/} | tee -a $LOG_FILE
      fi
    done
    cd $userdir

    # deinitialize AMiRo-LLD submodule and delete any remaining files
    cd $peripheryllddir
    git submodule deinit -f $amirollddir 2>&1 | tee -a $LOG_FILE
    rm -rf $amirollddir/*
    cd $userdir

    return 0
  fi
}

### main function of this script ###############################################
# Initializes or wipes the AMiRo-LLD Git submodule.
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
          initAmiroLld; printf "\n"; shift 1;;
        -d=*|--documentation=*)
          documentation "${1#*=}"; printf "\n"; shift 1;;
        -d|--documentation)
          if ( ! parseIsOption $2 ); then
            documentation "$2"; printf "\n"; shift 2
          else
            documentation; printf "\n"; shift 1
          fi;;
        -w|--wipe)
          wipeAmiroLld; printf "\n"; shift 1;;
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
    printInfo "Periphery-LLD setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "   [I]  - initialize AMiRo-LLD submodule\n"
    printf "   [D]  - generate or open HTML documentation\n"
    printf "   [W]  - wipe AMiRo-LLD submodule\n"
    printf "  [ESC] - quit this setup\n"
    local userinput=""
    readUserSelection "IiDdWw" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        initAmiroLld; printf "\n";;
      W|w)
        wipeAmiroLld; printf "\n";;
      D|d)
        documentation; printf "\n";;
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

