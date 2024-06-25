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
source "$(dirname ${BASH_SOURCE[0]})/tools/bash/setuplib.sh"

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
  printf "#                   Welcome to the AMiRo-OS setup!                   #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--init] [-k|--kernel] [-b|--bootloader] [-p|--periphery-LLD] [-c|--compiler] [--IDE] [-q|--quit] [--log=<file>]\n"
  printf "\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --init\n"
  printf "              Run project initalization.\n"
  printf "          -k, --kernel\n"
  printf "              Enter kernel setup.\n"
  printf "          -b, --bootloader\n"
  printf "              Enter bootloader setup.\n"
  printf "          -p, --periphery-LLD\n"
  printf "              Enter periphery-LLD setup.\n"
  printf "          -c, --compiler\n"
  printf "              Enter compiler setup.\n"
  printf "          -e, --IDE\n"
  printf "              Enter IDE setup.\n"
  printf "          -d, --documentation <option>\n"
  printf "              Possible options are: 'g' and 'o' (can be combined).\n"
  printf "              - g: Generate HTML documentation of AMiRo-OS.\n"
  printf "              - o: Open HTML documentation of AMiRo-OS.\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### initialize whole project ###################################################
# Initializes the whole project, including fetching all submodules and so on.
#
# usage:      projectInitialization
# arguments:  n/a
# return:     n/a
#
function projectInitialization {
  printInfo "initializing project\n"
  printf "\n"
  if [ -z "$LOG_FILE" ]; then
    printInfo "initializing kernel submodule...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --init --quit --noinfo
    printf "\n"
    printInfo "initializing ChibiOS...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --patch --documentation=g --quit --noinfo
    printf "\n"

    printInfo "initializing bootloader submodule...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/bootloadersetup.sh --init --quit --noinfo
    printf "\n"
    printInfo "initializing AMiRo-BLT...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/AMiRo-BLT/setup.sh --stm32flash --SerialBoot --quit --noinfo
    printf "\n"

    printInfo "initializing periphery-LLD (AMiRo-LLD)...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/periphery-lld/peripherylldsetup.sh --init --documentation=g --quit --noinfo
    printf "\n"
  else
    printInfo "initializing kernel submodule...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --init --quit --LOG="$LOG_FILE" --noinfo
    printf "\n"
    printInfo "initializing ChibiOS...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --patch --documentation=g --quit --LOG="$LOG_FILE" --noinfo
    printf "\n"

    printInfo "initializing bootloader submodule...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/bootloadersetup.sh --init --quit --LOG="$LOG_FILE" --noinfo
    printf "\n"
    printInfo "initializing AMiRo-BLT...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/AMiRo-BLT/setup.sh --stm32flash --SerialBoot --quit --LOG="$LOG_FILE" --noinfo
    printf "\n"

    printInfo "initializing periphery-LLD (AMiRo-LLD)...\n"
    $(dirname $(realpath ${BASH_SOURCE[0]}))/periphery-lld/peripherylldsetup.sh --init -documentation=g --quit --LOG="$LOG_FILE" --noinfo
    printf "\n"
  fi

  # the function will print an INFO message
  documentation g
  printf "\n"

  printInfo "initialization complete\n"
}

### enter kernel setup #########################################################
# Enter the kernel setup.
#
# usage:      kernelSetup
# arguments:  n/a
# return:     n/a
#
function kernelSetup {
  printInfo "entering kernel setup\n"
  printf "\n"
  if [ -z $LOG_FILE ]; then
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --noinfo
  else
    $(dirname $(realpath ${BASH_SOURCE[0]}))/kernel/kernelsetup.sh --LOG="$LOG_FILE" --noinfo
  fi
}

### enter bootloader setup #####################################################
# Enter bootloader setup.
#
# usage:      bootloaderSetup
# arguments:  n/a
# return:     n/a
#
function bootloaderSetup {
  printInfo "entering bootloader setup\n"
  printf "\n"
  if [ -z $LOG_FILE ]; then
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/bootloadersetup.sh --noinfo
  else
    $(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/bootloadersetup.sh --LOG="$LOG_FILE" --noinfo
  fi
}

### enter periphery LLD setup ##################################################
# Enter the Periphery-LLD setup.
#
# usage:      peripheryLldSetup
# arguments:  n/a
# return:     n/a
#
function peripheryLldSetup {
  printInfo "entering periphery-LLD setup\n"
  printf "\n"
  if [ -z $LOG_FILE ]; then
    $(dirname $(realpath ${BASH_SOURCE[0]}))/periphery-lld/peripherylldsetup.sh --noinfo
  else
    $(dirname $(realpath ${BASH_SOURCE[0]}))/periphery-lld/peripherylldsetup.sh --LOG="$LOG_FILE" --noinfo
  fi
}

### enter compiler setup #######################################################
# Enter the compiler setup of tzhe AMiRo-BLT submodule.
#
# usage:      compilerSetup
# arguments:  n/a
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: AMiRo-BLT submodule not nitialized yet-
#
function compilerSetup {
  # check if the AMiRo-BLT submodule is initialized and the script file exists
  local amirobltdir=$(dirname $(realpath ${BASH_SOURCE[0]}))/bootloader/AMiRo-BLT
  local compilerscriptfile=${amirobltdir}/tools/compiler/compilersetup.sh
  if [ -z "$(ls -A $amirobltdir)" ] || [ ! -f $compilerscriptfile ]; then
    printWarning "Please initialize AMiRo-BLT submodule first.\n"
    return 1
  else
    printInfo "entering compiler setup\n"
    if [ -z "$LOG_FILE" ]; then
      $compilerscriptfile --noinfo
    else
      $compilerscriptfile --LOG="$LOG_FILE" --noinfo
    fi
    return 0
  fi
}

### enter IDE setup ############################################################
# Enter IDE setup.
#
# usage:      ideSetup
# arguments:  n/a
# return:     n/a
#
function ideSetup {
  printInfo "entering IDE setup\n"
  printf "\n"
  if [ -z $LOG_FILE ]; then
    $(dirname $(realpath ${BASH_SOURCE[0]}))/tools/ide/idesetup.sh --noinfo
  else
    $(dirname $(realpath ${BASH_SOURCE[0]}))/tools/ide/idesetup.sh --LOG="$LOG_FILE" --noinfo
  fi
}

### AMiRo-OS dcoumentation setup ###############################################
#
# usage:      documentation [<option>]
# arguments:  <option>
#                 Can be either 'g' or 'o' to generate or open HTML documentation respectively.
# return:     0
#                 No error or warning occurred.
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
  local amiroosdir=$(dirname $(realpath ${BASH_SOURCE[0]}))

  local option="";
  # if no argument was specified, ask what to do
  if [ $# -eq 0 ]; then
    printInfo "AMiRo-OS documentation setup\n"
    printf "Please select one of the following actions:\n"
    printf "   [G]  - generate HTML documentation\n"
    printf "   [O]  - open HTML documentation\n"
    printf "  [ESC] - abort this setup\n"
    local userinput
    readUserSelection "GgOo" 'y' userinput
    option=${userinput,,}
    if [ $option = $'\e' ]; then
      printInfo "AMiRo-OS documentation setup aborted by user\n"
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
      if [ -f ${amiroosdir}/docs/doxygen/Doxyfile ]; then
        printInfo "generating AMiRo-OS documentation...\n"
        cd ${amiroosdir}/docs/doxygen/
        doxygen Doxyfile 2>&1 | tee -a $LOG_FILE &> /dev/null
        cd $userdir
        printInfo "access AMiRo-OS documentation via ${amiroosdir}docs/doxygen/output/html/index.html\n"
      else
        issues=$((issues + 1))
        printError "could not generate AMiRo-OS documentation\n"
      fi
      ;;

    # open HTML documentation
    O|o)
      # check if required files exist
      if [ -f ${amiroosdir}/docs/doxygen/output/html/index.html ]; then
        printInfo "open AMiRo-OS documentation\n"
        xdg-open ${amiroosdir}/docs/doxygen/output/html/index.html &> /dev/null &
      else
        issues=$((issues + 1))
        printError "could not open AMiRo-OS documentation\n"
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
}

### main function of this script ###############################################
# Provides functions for project initialization, configuration of IDE and
# compiler setup, as well as entry points to the several Git submodules.
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
          projectInitialization; printf "\n"; shift 1;;
        -k|--kernel)
          kernelSetup; printf "\n"; shift 1;;
        -b|--bootloader)
          bootloaderSetup; printf "\n"; shift 1;;
        -p|--periphery-LLD)
          peripheryLldSetup; printf "\n"; shift 1;;
        -c|--compiler)
          compilerSetup; printf "\n"; shift 1;;
        -e|--IDE)
          ideSetup; printf "\n"; shift 1;;
        -d=*|--documentation=*)
          documentation "${1#*=}"; printf "\n"; shift 1;;
        -d|--documentation)
          if ( ! parseIsOption $2 ); then
            documentation "$2"; printf "\n"; shift 2
          else
            documentation; printf "\n"; shift 1
          fi;;
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
    printInfo "AMiRo-OS main menu\n"
    printf "Please select one of the following actions:\n"
    printf "   [I]  - project initialization\n"
    printf "   [K]  - enter kernel setup\n"
    printf "   [B]  - enter bootloaders setup\n"
    printf "   [P]  - enter periphery-LLD setup\n"
    printf "   [C]  - enter compiler setup\n"
    printf "   [E]  - enter IDE project setup\n"
    printf "   [D]  - generate or open HTML documentation\n"
    printf "  [ESC] - quit this setup\n"
    local userinput=""
    readUserSelection "IiKkBbPpCcEeDd" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        projectInitialization; printf "\n";;
      K|k)
        kernelSetup; printf "\n";;
      B|b)
        bootloaderSetup; printf "\n";;
      P|p)
        peripheryLldSetup; printf "\n";;
      C|c)
        compilerSetup; printf "\n";;
      E|e)
        ideSetup; printf "\n";;
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

