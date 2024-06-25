################################################################################
# AMiRo-BLT is an bootloader and toolchain designed for the Autonomous Mini    #
# Robot (AMiRo) platform.                                                      #
# Copyright (C) 2016..2021  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.        #
#                                                                              #
# This research/work was supported by the Cluster of Excellence Cognitive      #
# Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is   #
# funded by the German Research Foundation (DFG).                              #
################################################################################

#!/bin/bash

# load library
source "$(dirname ${BASH_SOURCE[0]})/../../bash/setuplib.sh"

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
  printf "#                  Welcome to the QtCreator setup!                   #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "# Copyright (c) 2016..2021  Thomas Schöpping                         #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-c|--create=<module>] [-d|--delete=<module>] [-q|--quit] [--log=<file>]\n"
  printf "\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -c, --create <module>\n"
  printf "              Create project files for a sigle module or for all modules (<module>='*').\n"
  printf "          -d, --delete <module>\n"
  printf "              Delete projet files of a single module or of all modules (<module>='*').\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### read directory where to create/delete projects #############################
# Read the directory where to create/delete project files from user.
#
# usage:      getProjectDir <pathvar> [<initpath>]
# arguments:  <pathvar>
#                 Variable to store the selected path to.
#             <initpath>
#                 Optional initial path to suggest the user.
#                 If not set, the AMiRo-BLT Target/ directory is suggested.
# return:     n/a
#
function getProjectDir {
  local amirobltdir="$2"
  local input=""

  # if no initial path was given, default to AMiRo-BLT Target/ directory
  if [ -z "$amirobltdir" ]; then
    amirobltdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../Target/)
  fi

  printLog "reading path for project files from user...\n"
  read -p "Path where to create/delete project folders & files: " -i $amirobltdir -e input
  input=$(sed "s;^~\($\|/\);${HOME}/;" <<< ${input})
  printLog "user selected path $(realpath $input)\n"
  eval $1="$(realpath $input)"
}

### retrieves the ARM-NONE-EABI-GCC include directory ##########################
# Retrieves the include directory of the currently set arm-none-eabi-gcc.
#
# usage:      retrieveGccIncludeDir <path>
# arguments:  <path>
#                 Variable to store the path to.
# return:    0
#                 No error or warning occurred.
#            -1
#                 Error: Command 'arm-none-eabi-gcc' not found.
#            -2
#                 Error: include directory could not be resolved.
#
function retrieveGccIncludeDir {
  local binpath=$(which arm-none-eabi-gcc)
  local gccincpath=""

  # retrieve binary path or link
  if [ -z "$binpath" ]; then
    printError "command 'arm-none-eabi-gcc' not found\n"
    return -1
  else 

    # traverse any links
    while [ -L "$binpath" ]; do
      binpath=$(realpath $(dirname $binpath)/$(readlink $binpath))
    done
    printInfo "gcc-arm-none-eabi detected: $binpath\n"

    # return include path
    gccincpath=$(realpath $(dirname ${binpath})/../arm-none-eabi/include/)
    if [ ! -d "$gccincpath" ]; then
      printWarning "$gccincpath does not exist\n"
      return -2
    else
      eval $1="$gccincpath"
      return 0
    fi
  fi
}

### detect available modules ###################################################
# Detect all avalable modules supported by AMiRo-OS.
#
# usage:      detectModules <modulearray>
# arguments:  <modulearray>
#                 Array variable to store all detected modules to.
# return:     n/a
#
function detectModules {
  local modulesdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../Target/Modules)
  local modules_detected=()

  # detect all available modules (via directories)
  for dir in $(ls -d ${modulesdir}/*/); do
    modules_detected[${#modules_detected[@]}]=$(basename $dir)
  done

  # set the output variable
  eval "$1=(${modules_detected[*]})"
}

### create project files for a single or all modules ###########################
# Create project files for a single or all modules.
#
# usage:      createProject <modules> [-m|--module=<module>] [-p|--path=<path>] [--gcc=<path>] [-o|--out=<var>] [--gccout=<var>]
# arguments:  <modules>
#                 Array containing all modules available.
#             -m, --module <module>
#                 Name (folder name) of the module for which project files shall be generated, or '*' to generate for all modules.
#             -p, --path <path>
#                 Path where to create the project files.
#             --gcc=<path>
#                 Path to the GCC include directory.
#             -o, --out <var>
#                 Variable to store the path to.
#             --gccout=<var>
#                 Variable to store the path to the GCC include directory to.
#                 If this optional arguments is absent, ths function will ask for user input.
# return:     0
#                 No error or warning occurred.
#             1
#                 Aborted by user.
#             -1
#                 No modules available.
#             -2
#                 The specified <module> could not be found.
#             -3
#                 Parsing the project for the specified module failed.
#             -4
#                 Missing dependencies.
#             -5
#                 Creating of a directory/file failed (probably due to unsufficient permissions).
#
function createProject {
  local gccincludedir=""
  local gccoutvar=""
  local includes=()
  local module=""
  local modules=("${!1}")
  local modules2generate=()
  local modulesdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../Target/Modules)
  local otherargs=()
  local outvar=""
  local parse_state=""
  local projectdir=""
  local projectfiles=()
  local quotes="\'\"\´\`\„\“"
  local rawout=""
  local sourcefile=""
  local sourcefiles=()
  local userdir=$(pwd)
  local userinput=""

  # check dependencies
  checkCommands make
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -4
  fi

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -m=*|--module=*)
          module="${1#*=}"; shift 1;;
        -m|--module)
          module="$2"; shift 2;;
        -p=*|--path=*)
          projectdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectdir=$(realpath "$2"); shift 2;;
        --gcc=*)
          gccincludedir=$(realpath "${1#*=}"); shift 1;;
        --gcc)
          gccincludedir=$(realpath "$2"); shift 2;;
        -o=*|--out=*)
          outvar=${1#*=}; shift 1;;
        -o|--out)
          outvar=$2; shift 2;;
        --gccout=*)
          gccoutvar=$(realpath "${1#*=}"); shift 1;;
        --gccout)
          gccoutvar=$(realpath "$2"); shift 2;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # sanity check for the modules variable
  if [ -z "${modules[*]}" ]; then
    printError "no modules available\n"
    return -1
  fi

  # select module
  if [ -z $module ]; then
    # list all available modules
    printInfo "select a module, type '*' for all or type 'A' to abort:\n"
    for (( idx=0; idx<${#modules[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${modules[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; then
        printWarning "Please enter an integer between 1 and ${#modules[@]}, '*' to select all or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[0-9]+$ ]]; then
      # store selection
      modules2generate=(${modules[$(($userinput - 1))]})
    elif [[ "$userinput" =~ ^\*$ ]]; then
      modules2generate=(${modules[@]})
    elif [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    fi
    printf "\n"
  else
    # check whether the specified module is available
    if [[ ! "${modules[@]}" =~ "$module" ]]; then
      printError "module '$module' not available\n"
      return -2
    else
      modules2generate=($module)
    fi
  fi

  # read absolute project directory if required
  if [ -z "$projectdir" ]; then
    getProjectDir projectdir $(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../Target/qtcreator)
    printf "\n"
  fi

  # check for existing project files
  modules2overwrite=()
  for module in ${modules2generate[@]}; do
    if [ -d ${projectdir}/${module} ] && [ -n '$(find ${projectdir}/${module} -maxdepth 1 -type f -regextype posix-extended -regex "^.*/${module}\.(config|creator|files|includes)$")' ]; then
      modules2overwrite[${#modules2overwrite[@]}]=${module}
    fi
  done
  if [ ${#modules2overwrite[@]} != 0 ]; then
    printWarning "The following modules will be overwritten:\n"
    for module in ${modules2overwrite[@]}; do
      printWarning "\t$(basename $module)\n"
    done
    userinput=""
    printInfo "Continue and overwrite? [y/n]\n"
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        ;;
      N|n)
        printWarning "project generation aborted by user\n"
        return 1
        ;;
      *)
        printError "unexpected input: ${userinput}\n"; return -999;;
    esac
    printf "\n"
  fi

  # retrieve absolute GCC include path
  if [ -z "$gccincludedir" ]; then
    retrieveGccIncludeDir gccincludedir
  fi

  # generate all modules
  for module in ${modules2generate[@]}; do
    # check whether the directoty exists and create it if required
    if [ ! -d "${projectdir}/${module}" ]; then
      printWarning "Creating nonexistent directory '${projectdir}/${module}/'.\n"
      mkdir -p "${projectdir}/${module}"
      if [ $? != 0 ]; then
        printError "failed to create directory\n"
        cd "$userdir"
        return -5
      fi
    fi

    # print message
    printInfo "generating QtCreator project files for the $module module...\n"

    # delete clang cache
    rm -rf ${projectdir}/${module}/.qtc_clangd/

    # run make, but only run the GCC preprocessor and produce no binaries
    sourcefiles=()
    sourcefile=""
    parse_state="WAIT_FOR_INCLUDE_OR_COMPILE"
    makedir=""
    makedirs=()
    # capture all output from make and GCC and append the return value of make as last line
    printInfo "processing project (this may take a while)...\n"
    rawout=$(make --directory ${modulesdir}/${module}/Boot --always-make USER_CFLAGS="-v -H" OBJ_PATH=qt_obj BIN_PATH=qt_bin 2>&1 && echo $?)
    rm -rf ${modulesdir}/${module}/Boot/qt_obj ${modulesdir}/${module}/Boot/qt_bin
    # check whether the make call was successfull
    if [[ $(echo "${rawout}" | tail -n 1) != "0" ]]; then
      printError "executing 'make' in module directory failed\n"
      cd "$userdir"
      return -3
    fi
    # extract file names from raw output
    IFS=$'\n'; rawout=($rawout); unset IFS
    for line in "${rawout[@]}"; do
      case $parse_state in
        WAIT_FOR_INCLUDE_OR_COMPILE)
          # lines stating included files look like:
          # ... <../relative/path/to/file>
          if [[ "$line" =~ ^\.+[[:blank:]].+\..+$ ]]; then
            sourcefile=${line##* }
            if [[ ! "$sourcefile" =~ ^/ ]]; then
              sourcefile=$(realpath ${modulesdir}/${module}/Boot/${sourcefile})
            fi
            sourcefiles[${#sourcefiles[@]}]="$sourcefile"
          # whenever the next source file is processed, a message appears like:
          # Compiling <filename>
          elif [[ "$line" =~ ^\+\+\+[[:blank:]]Compiling[[:blank:]]\[(.+\.c)\]$ ]]; then
            printf "."
            sourcefile=${BASH_REMATCH[1]}
            parse_state="WAIT_FOR_COMPILERCALL"
          fi;;
        WAIT_FOR_COMPILERCALL)
          # wait for the actual call of the compiler to retrieve the full path to the source file
          if [[ "$line" == *${sourcefile}* ]]; then
            line="${line%%${sourcefile}*}${sourcefile}"
            sourcefile=$(realpath ${modulesdir}/${module}/Boot/${line##* })
            sourcefiles[${#sourcefiles[@]}]="$sourcefile"
            parse_state="WAIT_FOR_INCLUDE_OR_COMPILE"
          fi;;
      esac
    done
    printf "\n"
    # sort and remove duplicates
    IFS=$'\n'; sourcefiles=($(sort --unique <<< "${sourcefiles[*]}")); unset IFS

    # extract include paths
    includes=()
    for source in ${sourcefiles[*]}; do
      includes[${#includes[@]}]="$(dirname ${source})"
    done
    # sort and remove duplicates
    IFS=$'\n'; includes=($(sort --unique <<< "${includes[*]}")); unset IFS

    # generate the .incldues file, containing all include paths
    echo "" > ${projectdir}/${module}/${module}.includes
    for inc in ${includes[*]}; do
      echo "$inc" >> ${projectdir}/${module}/${module}.includes
    done
    # generate the .files file, containing all source files
    echo "" > ${projectdir}/${module}/${module}.files
    for source in ${sourcefiles[*]}; do
      # skip GCC files
      if [[ ! "$source" =~ .*/gcc.* ]]; then
        echo "$source" >> ${projectdir}/${module}/${module}.files
      fi
    done
    # generate a default project configuration file if it doesn't exist yet
    if [ ! -f ${projectdir}/${module}/${module}.config ]; then
      echo "// Add predefined macros for your project here. For example:" > ${projectdir}/${module}/${module}.config
      echo "// #define YOUR_CONFIGURATION belongs here" >> ${projectdir}/${module}/${module}.config
    fi
    # generate a default .creator file if it doesn't exist yet
    if [ ! -f ${projectdir}/${module}/${module}.creator ]; then
      echo "[general]" > ${projectdir}/${module}/${module}.creator
    fi
  done

  # go back to user directory
  cd "$userdir"

  # fill the output variables
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectdir"
  fi
  if [ ! -z "$gccoutvar" ]; then
    eval $gccoutvar="$gccincludedir"
  fi

  return 0
}

### delete project files for a single or all modules ###########################
# Deletes all project files and optionally .user files, too.
#
# usage:      deleteProject [-p|--path=<path>] [-m|--module=<module>] [-o|--out=<var>] [-w|--wipe=<flag>]
# arguments:  -p, --path <path>
#                 Path where to delete the project files.
#             -m, --module <module>
#                 Module name for which the project files shall be deleted or '*' to delete for all modules.
#             -o, --out <var>
#                 Variable to store the path to.
#             -w, --wipe <flag>
#                 Flag whether to delete .user files as well (must be either "true" or "false").
# return:     0
#               No error or warning occurred.
#             1
#               Aborted by user.
#             2
#               There are no project files (for the specified module) at the specified location.
#
function deleteProject {
  local files=()
  local module=""
  local modules=()
  local modules2delete=()
  local otherargs=()
  local outvar=""
  local projectdir=""
  local userinput=""
  local wipe=""

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -p=*|--path=*)
          projectdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectdir=$(realpath "$2"); shift 2;;
        -m=*|--module=*)
          module="${1#*=}"; shift 1;;
        -m|--module)
          module="${2}"; shift 2;;
        -o=*|--out=*)
          outvar=${1#*=}; shift 1;;
        -o|--out)
          outvar=$2; shift 2;;
        -w=*|--wipe=*)
          wipe="${1#*=}"; shift 1;;
        -w|--wipe)
          wipe="$2"; shift 2;;
        *)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # sanity checks on parameters
  if [ ! -z $wipe ] && [[ ! "$wipe" =~ ^(true|false)$ ]]; then
    printWarning "invalid value set to 'wipe' argument\n"
    printInfo "I will act as if there was no '--wipe' argument and ask for input later.\n"
    wipe=""
  fi

  # read absolute project directory if required
  while [ -z "$projectdir" ]; do
    getProjectDir projectdir $(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../Target/qtcreator)
    if [ ! -d "$projectdir" ]; then
      printWarning "directory does not exist.\n"
      projectdir=""
    fi
  done
  printf "\n"

  # retrieve all modules in the specified directory
  modules=($(find "${projectdir}" -maxdepth 2 -type f -regextype posix-extended -regex "^.+\.(cflags|config|creator|creator\.user(\..+)?|cxxflags|files|includes)$" | grep -oE "/[^/\.]+\." | grep -oE "[^/].+[^\.]" | sort --unique))

  # check whether there are any project files at the specified location
  if [ ${#modules[@]} -eq 0 ]; then
    printWarning "no projects detected at '${projectdir}'\n"
    return 2
  fi

  # select module
  if [ -z "$module" ]; then
    # list all available modules
    printInfo "select a module, type '*' for all or type 'A' to abort:\n"
    for (( idx=0; idx<${#modules[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${modules[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#modules[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; then
        printWarning "Please enter an integer between 1 and ${#modules[@]}, '*' to select all or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[0-9]+$ ]]; then
      # store selection
      modules2delete=(${modules[$(($userinput - 1))]})
    elif [[ "$userinput" =~ ^\*$ ]]; then
      modules2delete=(${modules[@]})
    elif [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    else
      printError "unexpected user input: ${userinput}\n"
      return -999
    fi
    printf "\n"
  else
    # check whether the specified module is available
    if [[ ! "${modules[@]}" =~ "$module" ]]; then
      printWarning "there are no project files for module '$module' at location '$projectdir'\n"
      return 2
    else
      modules2delete=($module)
    fi
  fi

  # check for .user files
  files=()
  for module in ${modules2delete[@]}; do
    files+=($(find "${projectdir}" -maxdepth 2 -type f -regextype posix-extended -regex "^.*${module}\.creator\.user(\..+)?$"))
  done

  # read wipe information if required
  if [ ${#files[@]} -gt 0 ] && [ -z "$wipe" ]; then
    userinput=""
    printInfo "Wipe user data as well? [y/n]\n"
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        wipe="true";;
      N|n)
        wipe="false";;
      *)
        printError "unexpected input: ${userinput}\n"; return -999;;
    esac
    printf "\n"
  fi

  # retrieve all files
  files=()
  for module in ${modules2delete[@]}; do
    if [ "$wipe" == "true" ]; then
      files+=($(find "${projectdir}" -maxdepth 2 -type f -regextype posix-extended -regex "^.*${module}\.(includes|files|config|creator|cflags|cxxflags|creator\.user(\..+)?)$"))
    else
      files+=($(find "${projectdir}" -maxdepth 2 -type f -regextype posix-extended -regex "^.*${module}\.(includes|files|config|creator|cflags|cxxflags)$"))
    fi
  done

  # list all files to be deleted and ask for confirmation
  if [ ${#files[@]} -gt 0 ]; then
    printWarning "The following files will be deleted:\n"
    for file in ${files[@]}; do
      printWarning "\t$(basename ${file})\n"
    done
    userinput=""
    printInfo "Do you want to continue? [y/n]\n"
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        ;;
      N|n)
        printWarning "aborted by user\n"
        return 1;;
      *)
        printError "unexpected input: ${userinput}\n"; return -999;;
    esac
  else
    printInfo "no files to delete\n"
  fi

  # finally delete the files
  for file in ${files[@]}; do
    rm ${file} 2>&1 | tee -a $LOG_FILE
  done
  # further delete any clang caches
  for module in ${modules2delete[@]}; do
    rm -rf "${projectdir}/${module}/.qtc_clangd/"
  done
  # remove all empty directories
  for dir in $(find "${projectdir}" -type d -empty); do
    rm -rf "$dir"
  done

  # store the path to the output variable, if required
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectdir"
  fi

  return 0
}

### main function of this script ###############################################
# Creates, deletes and wipes QtCreator project files for any module supported by AMiRo-OS.
#
# usage:      see function printHelp
# arguments:  see function printHelp
# return:     0
#                 No error or warning ocurred.
#
function main {
  local cmd=""
  local cmdidx=""
  local filenameidx=""
  local logfile=""
  local modules=()
  local otherargs=()
  local userinput=""

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
    cmdidx=1
    while [[ ! "${!cmdidx}" = "--log"* ]] && [[ ! "${!cmdidx}" = "--LOG"* ]]; do
      cmdidx=$[cmdidx + 1]
    done
    cmd="${!cmdidx}"
    logfile=""
    if [[ "$cmd" = "--log="* ]] || [[ "$cmd" = "--LOG="* ]]; then
      logfile=${cmd#*=}
    else
      filenameidx=$((cmdidx + 1))
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

  # detect available modules and inform user
  detectModules modules
  case "${#modules[@]}" in
    0)
      printInfo "no module has been detected\n";;
    1)
      printInfo "1 module has been detected:\n";;
    *)
      printInfo "${#modules[@]} modules have been detected:\n";;
  esac
  for (( idx=0; idx<${#modules[@]}; ++idx )); do
    printInfo "  - ${modules[$idx]}\n"
  done
  printf "\n"

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -c=*|--create=*)
          createProject modules[@] --module="${1#*=}"; printf "\n"; shift 1;;
        -c|--create)
          createProject modules[@] --module="${2}"; printf "\n"; shift 2;;
        -d=*|--delete=*)
          deleteProject --module="${1#*=}"; printf "\n"; shift 1;;
        -d|--delete)
          deleteProject --module="${2}"; printf "\n"; shift 2;;
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
    printInfo "QtCreator setup main menu\n"
    printf "Please select one of the following actions:\n"
    printf "   [C]  - create project files\n"
    printf "   [D]  - delete project files\n"
    printf "  [ESC] - quit this setup\n"
    userinput=""
    readUserSelection "CcDd" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      C|c)
        createProject modules[@]; printf "\n";;
      D|d)
        deleteProject; printf "\n";;
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

