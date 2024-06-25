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
  printf "#              Welcome to the ChibiOS submodule setup!               #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--init] [-p|--patch] [-d|--documentation=<option>] [-w|--wipe] [-q|--quit] [--log=<file>]\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --init\n"
  printf "              Initialize ChibiOS submodule.\n"
  printf "          -p, --patch,\n"
  printf "              Apply patches to ChibiOS.\n"
  printf "          -d, --documentation <option>\n"
  printf "              Possible options are: 'g' and 'o' (can be combined).\n"
  printf "              - g: Generate HTML documentation of ChibiOS.\n"
  printf "              - o: Open HTML documentation of ChibiOS.\n"
  printf "          -w, --wipe\n"
  printf "              Wipe ChibiOS submodule.\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### initialize ChibiOS submodule ###############################################
# Initializes the ChibiOS Git submodule.
#
# usage:      initChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Aborted by user.
#             -1
#                 Error: Unexpected user input.
#             -1
#                 Error: Missing dependency.
#
function initChibiOS {
  printInfo "initializing ChibiOS submodule...\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS

  # if the kernel folder is not empty
  if [ ! -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is not empty. Delete and reinitialize? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        wipeChibiOS
        ;;
      N|n)
        printWarning "ChibiOS initialization aborted by user\n"
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
  cd $kerneldir
  git submodule update --init $chibiosdir 2>&1 | tee -a $LOG_FILE
  while [ ${PIPESTATUS[0]} -ne 0 ]; do
    printWarning "initialitaion failed. Retry? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        git submodule update --init $chibiosdir 2>&1 | tee -a $LOG_FILE;;
      N|n)
        printWarning "ChibiOS initialization aborted by user\n"
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

### patch ChibiOS ##############################################################
# Applies patches to ChibiOS submodule.
#
# usage:      patchChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: ChibiOS not initialized yet.
#             2
#                 Warning: Setup aborted by user.
#             -1
#                 Error: Unexpected user input.
#
function patchChibiOS {
  printInfo "applying patches to ChibiOS\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS
  local git_branch_patched="AMiRo-OS"

  # check dependencies
  checkCommands git
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is empty. Please initialize first.\n"
    return 1
  fi
   
  # get some information from Git
  cd $chibiosdir
  local git_branch_current=$(git rev-parse --abbrev-ref HEAD)
  local git_branches=$(git for-each-ref --format="%(refname)")
  local git_dirtyfiles=($(git ls-files -dmo --exclude-standard --exclude=/doc))
  cd $userdir

  local issues=0
  # if the current branch is already $git_branch_patched
  if [ "$git_branch_current" = "$git_branch_patched" ]; then
    issues=$((issues + 1))
    printWarning "current branch is already $git_branch_patched\n"
  fi
  # if the current branch is bot $git_branch_patched, but another branch $git_branch_patched already exists
  if [ "$git_branch_current" != "$git_branch_patched" ] && [[ "$git_branches" = *"$git_branch_patched"* ]]; then
    issues=$((issues + 1))
    printWarning "another branch $git_branch_patched already exists\n"
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
        printWarning "ChibiOS patching aborted by user\n"
        return 2
        ;;
      *) # sanity check (return error)
        printError "unexpected input: $userinput\n"; return -1;;
    esac
  fi

  # create a new branch and apply the patches
  local patches=${kerneldir}/patches/*.patch
  cd $chibiosdir
  git checkout -b "$git_branch_patched" 2>&1 | tee -a $LOG_FILE
  for patch in $patches; do
    printInfo "applying $(basename ${patch})...\n"
    git apply --whitespace=nowarn --ignore-space-change --ignore-whitespace < $patch 2>&1 | tee -a $LOG_FILE
#    # These lines are disabled for safety reasons:
#    #   Filed commits are detected as valid changes by the super-project.
#    #   This would lead to errorneous updates of the super-project, so to point to one of those (local) commits.
#    #   Since these commits are not pushed upstream, initialization of the super-project will therefore fail, because
#    #   the referenced hashes (after patching) do not exist in a clean copy of this sub-project.
#    git add $(git ls-files -dmo --exclude-standard --exclude=/doc) $(git diff --name-only) 2>&1 | tee -a $LOG_FILE
#    git commit --message="$patch applied" 2>&1 | tee -a $LOG_FILE
  done
  cd $userdir

  return 0
}

### ChibiOS dcoumentation setup ################################################
#
# usage:      documentation [<option>]
# arguments:  <option>
#                 Can be either 'g' or 'o' to generate or open HTML documentation respectively.
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: ChibiOS not initialized yet.
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
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printWarning "$chibiosdir is empty. Please initialize first.\n"
    return 1
  else
    local option="";
    # if no argument was specified, ask what to do
    if [ $# -eq 0 ]; then
      printInfo "ChibiOS documentation setup\n"
      printf "Please select one of the following actions:\n"
      printf "   [G]  - generate HTML documentation\n"
      printf "   [O]  - open HTML documentation\n"
      printf "  [ESC] - abort this setup\n"
      local userinput
      readUserSelection "GgOo" 'y' userinput
      option=${userinput,,}
      if [ $option = $'\e' ]; then
        printInfo "ChibiOS documentation setup aborted by user\n"
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
        checkCommands sh doxygen
        if [ $? -ne 0 ]; then
          printError "Missing dependencies detected.\n"
          return -2
        fi
        # check if required files exist
        if [ -f ${chibiosdir}/doc/build/full_rm/makehtml.sh ]; then
          printInfo "generating ChibiOS documentation...\n"
          cd ${chibiosdir}/doc/build/full_rm
          mkdir ${chibiosdir}/doc/manuals/html
          sh ${chibiosdir}/doc/build/full_rm/makehtml.sh 2>&1 | tee -a $LOG_FILE &> /dev/null
          cd $userdir
          printInfo "access ChibiOS documentation via ${chibiosdir}doc/manuals/html/full_rm/index.html\n"
        else
          issues=$((issues + 1))
          printError "could not generate ChibiOS documentation\n"
        fi
        ;;

      # open HTML documentation
      O|o)
        # check if required files exist
        if [ -f ${chibiosdir}/doc/manuals/html/full_rm/index.html ]; then
          printInfo "open ChibiOS documentation\n"
          xdg-open ${chibiosdir}/doc/manuals/html/full_rm/index.html &> /dev/null &
        else
          issues=$((issues + 1))
          printError "could not open ChibiOS documentation\n"
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

### reset ChibiOS submodule and wipe directory #################################
# Resets the ChibiOS Git submodule and wipes the directory.
#
# usage:      wipeChibiOS
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Submodule directory is already empty.
#             2
#                 Warning: Wiping aborted by user.
#             -1
#                 Error: Unexpected user input.
#
function wipeChibiOS {
  printInfo "reset and wipe Git submodule $kerneldir\n"
  local userdir=$(pwd)
  local kerneldir=$(dirname $(realpath ${BASH_SOURCE[0]}))
  local chibiosdir=${kerneldir}/ChibiOS
  local git_branch_patched="AMiRo-OS"

  # check dependencies
  checkCommands git
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # if the ChibiOS folder is empty
  if [ -z "$(ls -A $chibiosdir)" ]; then
    printInfo "$chibiosdir is alread empty\n"
    return 1
  else 
    # get some information from Git
    cd $kerneldir
    local git_basehash=($(git ls-tree -d HEAD $kerneldir)); git_basehash=${git_basehash[2]}
    cd $chibiosdir
    local git_branch_current=$(git rev-parse --abbrev-ref HEAD)
    local git_difftobase="$(git diff ${git_basehash}..HEAD)"
    local git_commits=$(git log --format=oneline ${git_basehash}..HEAD)
    local git_dirtyfiles=($(git ls-files -dmo --exclude-standard --exclude=/doc))
    cd $userdir
    local issues=0
    # if the HEAD is neither detached, nor is the current branch $git_branch_patched
    if [ "$git_branch_current" != "HEAD" ] && [ "$git_branch_current" != "$git_branch_patched" ]; then
      issues=$((issues + 1))
      printWarning "modifications to ChibiOS Git submodule detected\n"
    fi
    # if HEAD is ahead of submodule base commit but with more than just applied patches
    if [ -n "$git_difftobase" ] && [ -n "$(echo $git_commits | grep -Ev '\.patch applied$')" ]; then
      issues=$((issues + 1))
      printWarning "HEAD is ahead of submodule base by unexpected commits\n"
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
          printWarning "wiping ChibiOS Git submodule aborted by user\n"
          return 2
          ;;
        *) # sanity check (return error)
          printError "unexpected input: $userinput\n"; return -1;;
      esac
    fi

    # checkout base commit and delete all local branches
    cd $kerneldir
    git submodule update --force --checkout $kerneldir | tee -a $LOG_FILE
    cd $chibiosdir
    local git_branches=($(git for-each-ref --format="%(refname)"))
    for branch in $git_branches; do
      if [[ $branch = *"heads/"* ]]; then
        git branch -D ${branch##*/} | tee -a $LOG_FILE
      fi
    done
    cd $userdir

    # deinitialize ChibiOS submodule and delete any remaining files
    cd $kerneldir
    git submodule deinit -f $chibiosdir 2>&1 | tee -a $LOG_FILE
    rm -rf $chibiosdir/*
    cd $userdir

    return 0
  fi
}

### main function of this script ###############################################
# The kernel setup provides comfortable initialization, patching, documentation
# generation and cleanup for ChibiOS.
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
          initChibiOS; printf "\n"; shift 1;;
        -p|--patch)
          patchChibiOS; printf "\n"; shift 1;;
        -d=*|--documentation=*)
          documentation "${1#*=}"; printf "\n"; shift 1;;
        -d|--documentation)
          if ( ! parseIsOption $2 ); then
            documentation "$2"; printf "\n"; shift 2
          else
            documentation; printf "\n"; shift 1
          fi;;
        -w|--wipe)
          wipeChibiOS; printf "\n"; shift 1;;
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
    printInfo "ChibiOS kernel setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "   [I]  - initialize ChibiOS submodule\n"
    printf "   [P]  - apply patches to ChibiOS\n"
    printf "   [D]  - generate or open HTML documentation\n"
    printf "   [W]  - wipe ChibiOS submodule\n"
    printf "  [ESC] - quit this setup\n"
    local userinput=""
    readUserSelection "IiPpDdWw" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        initChibiOS; printf "\n";;
      P|p)
        patchChibiOS; printf "\n";;
      D|d)
        documentation; printf "\n";;
      W|w)
        wipeChibiOS; printf "\n";;
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

