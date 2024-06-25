################################################################################
# AMiRo-Apps is a collection of applications and configurations for the        #
# Autonomous Mini Robot (AMiRo) platform.                                      #
# Copyright (C) 2018..2022  Thomas Schöpping et al.                            #
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
  printf "# Copyright (c) 2018..2022  Thomas Schöpping                         #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-c|--create=<configuration>] [-d|--delete=<configuration>] [-q|--quit] [--log=<file>]\n"
  printf "\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -c, --create <configuration>\n"
  printf "              Create projects for a single configuration or for all configurations (<configuration>='*').\n"
  printf "          -d, --delete <configuration>\n"
  printf "              Delete projet files of a single configuration or of all configurations (<configuration>='*').\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### read directory where to create/delete projects #############################
# Read the directory where to create/delete project files from user.
#
# usage:      getProjectsDir <pathvar> [<initpath>]
# arguments:  <pathvar>
#                 Variable to store the selected path to.
#             <initpath>
#                 Optional initial path to suggest the user.
#                 If not set, the AMiRo-Apps root directory is suggested.
# return:     n/a
#
function getProjectsDir {
  local amiroappsdir="$2"
  local input=""

  # if no initial path was given, default to AMiRo-Apps root directory
  if [ -z "$amiroappsdir" ]; then
    amiroosdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../)
  fi

  printLog "reading path for project files from user...\n"
  read -p "Path where to create/delete project files: " -i $amiroappsdir -e input
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

### detect available configurations ############################################
# Detect all avalable configurations.
#
# usage:      detectConfigurations <configurationsarray>
# arguments:  <configurationsarray>
#                 Array variable to store all detected configurations to.
# return:     n/a
#
function detectConfigurations {
  local configsdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../configurations)
  local configs_detected=()

  # detect all available modules (via directories)
  for dir in $(ls -d ${configsdir}/*/); do
    configs_detected[${#configs_detected[@]}]=$(basename $dir)
  done

  # set the output variable
  eval "$1=(${configs_detected[*]})"
}

### create project files for a single or all configurations ################
# Create project files for all modules of a single or all configurations.
#
# usage:      createProject <configurations> [-c|--configuration=<configuration>] [-p|--path=<path>] [--gcc=<path>] [-o|--out=<var>] [--gccout=<var>]
# arguments:  <configurations>
#                 Array containing all configurations available.
#             -c, --configuration <configuration>
#                 Name (folder name) of the configuration for which project files shall be generated, or '*' to generate for all configurations.
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
#             2
#                 The selected configuration does not contain any modules.
#             -1
#                 No configurations available.
#             -2
#                 The specified <configuration> could not be found.
#             -3
#                 Parsing the project for the specified configuration failed.
#             -4
#                 Missing dependencies.
#             -5
#                 Creating of a directory/file failed (probably due to unsufficient permissions).
#
function createProject {
  local gccincludedir=""
  local gccoutvar=""
  local includes=()
  local makedir=""
  local makedirs=()
  local config=""
  local configs=("${!1}")
  local configs2generate=()
  local configsdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../configurations)
  local modules=()
  local modulesdir=""
  local otherargs=()
  local outvar=""
  local parse_state=""
  local projectsdir=""
  local projectfiles=()
  local projects=()
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
        -c=*|--configuration=*)
          config="${1#*=}"; shift 1;;
        -c|--configuration)
          config="$2"; shift 2;;
        -p=*|--path=*)
          projectsdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectsdir=$(realpath "$2"); shift 2;;
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

  # sanity check for the configs variable
  if [ -z "${configs[*]}" ]; then
    printError "no configurations available\n"
    return -1
  fi

  # select configuration
  if [ -z $config ]; then
    # list all available configurations
    printInfo "select a configuration, type '*' for all or type 'A' to abort:\n"
    for (( idx=0; idx<${#configs[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${configs[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#configs[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#configs[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; then
        printWarning "Please enter an integer between 1 and ${#configs[@]}, '*' to select all or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[0-9]+$ ]]; then
      # store selection
      configs2generate=(${configs[$(($userinput - 1))]})
    elif [[ "$userinput" =~ ^\*$ ]]; then
      configs2generate=(${configs[@]})
    elif [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    fi
    printf "\n"
  else
    # check whether the specified configuration is available
    if [[ ! "${configs[@]}" =~ "$config" ]]; then
      printError "configuration '$config' not available\n"
      return -2
    else
      configs2generate=($config)
    fi
  fi

  # generate project names for all modules of all selected configurations
  for config in ${configs2generate[@]}; do
    if [ -d ${configsdir}/${config}/modules ]; then
      modules=($(find ${configsdir}/${config}/modules/ -mindepth 1 -maxdepth 1 -type d))
      if [ ${#modules[@]} -gt 0 ]; then
        for module in ${modules[@]}; do
          projects+=("${config}_$(basename ${module})")
        done
      else
        printWarning "configuration '${config}' does not contain any modules\n"
      fi
    else
      printWarning "no 'modules/' folder exists in configuration '${config}'\n"
    fi
  done
  if [ ${#projects[@]} -eq 0 ]; then
    printWarning "no modules detected, aborting\n"
    return 2
  fi

  # read absolute project directory if required
  if [ -z "$projectsdir" ]; then
    getProjectsDir projectsdir $(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../qtcreator)
    printf "\n"
  fi
  # check whether the directoty exists and create it if required
  if [ ! -d "$projectsdir" ]; then
    printWarning "Creating nonexistent directory '$projectsdir'.\n"
    mkdir -p "$projectsdir"
  fi

  # check for existing project files
  for project in ${projects[@]}; do
    projectfiles+=($(find ${projectsdir} -maxdepth 1 -type f -regextype posix-extended -regex "^.*/${project}\.(includes|files|config|creator)$" | sort))
  done
  if [ ${#projectfiles[@]} != 0 ]; then
    printWarning "The following files will be overwritten:\n"
    for pfile in ${projectfiles[@]}; do
      printWarning "\t$(basename $pfile)\n"
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

  # generate all configurations
  for config in ${configs2generate[@]}; do
    # retrieve all modules
    modulesdir="${configsdir}/${config}/modules"
    modules=()
    for dir in $(find ${modulesdir} -mindepth 1 -maxdepth 1 -type d); do
      modules+=($(basename $dir))
    done
    # silently skip this configuration if it contains no modules
    if [ ${#modules[@]} == 0 ]; then
      continue
    fi

    # print message
    printInfo "generating QtCreator projects for configuration '${config}'...\n"

    for module in ${modules[@]}; do
      # check whether the directory exists and create it if required
      if [ ! -d "${projectsdir}/${config}/${module}" ]; then
        printWarning "Creating nonexistent directory '${projectsdir}/${config}/${module}/'.\n"
        mkdir -p "${projectsdir}/${config}/${module}"
        if [ $? != 0 ]; then
          printError "failed to create directoty\n"
          cd "$userdir"
          return -5
        fi
      fi

      # delete clang cache
      rm -rf ${projectsdir}/${config}/${module}/.qtc_clangd/

      # run make, but only run the GCC preprocessor and produce no binaries
      sourcefiles=()
      sourcefile=""
      parse_state="WAIT_FOR_INCLUDE_COMPILE_MAKE"
      makedir=""
      makedirs=()
      # capture all output from make and GCC and append the return value of make as last line
      printInfo "processing module ${module} (this may take a while)...\n"
      rawout=$(make --directory ${modulesdir}/${module} --always-make USE_OPT="-v -E -H" USE_VERBOSE_COMPILE="no" OUTFILES="" BUILDDIR=${modulesdir}/${module}/qt_build 2>&1 && echo $?)
      rm -rf ${modulesdir}/${module}/qt_build
      # check whether the make call was successfull
      if [[ $(echo "${rawout}" | tail -n 1) != "0" ]]; then
        printError "executing 'make' in configuration directory failed\n"
        cd "$userdir"
        return -3
      fi
      # extract file names from raw output
      IFS=$'\n'; rawout=($rawout); unset IFS
      for line in "${rawout[@]}"; do
        case $parse_state in
          WAIT_FOR_INCLUDE_COMPILE_MAKE)
            # lines stating include files look like:
            # ... <../relative/path/to/file>
            if [[ "$line" =~ ^\.+[[:blank:]].+\..+$ ]]; then
              sourcefile=${line##* }
              if [[ ! "$sourcefile" =~ ^/ ]]; then
                sourcefile=$(realpath ${makedirs[-1]}/${sourcefile})
              fi
              sourcefiles[${#sourcefiles[@]}]="$sourcefile"
            # whenever the next source file is processed, a message appears like:
            # Compiling <filename>
            elif [[ "$line" =~ ^Compiling[[:blank:]](.+\..+)$ ]]; then
              printf "."
              sourcefile=${BASH_REMATCH[1]}
              parse_state="WAIT_FOR_COMPILERCALL"
            # if make is called again in another directory or a nested make call leaves the directory, a message appears like:
            # make[1]: Entering directory 'directory'
            # make[999]: Verzeichnis „directory“ wird verlassen
            elif [[ "$line" =~ ^make(\[[0-9]+\])?:([[:alnum:]]|[[:blank:]])*[$quotes].*[$quotes]([[:alnum:]]|[[:blank:]])*$ ]]; then
              # extract directory path
              makedir=$(echo ${line} | grep -oE "[${quotes}].*[${quotes}]" | grep -oE "[^${quotes}].*[^${quotes}]")
              # if the makedirs stack is empty or the directory does not match the last entry
              if [ ${#makedirs[@]} == 0 ] || [ "${makedir}" != "${makedirs[-1]}" ]; then
                # push the directory to the stack
                makedirs+=(${makedir})
              else
                # pop the directory from the stack
                unset makedirs[-1]
              fi
            fi;;
          WAIT_FOR_COMPILERCALL)
            # wait for the actual call of the compiler to retrieve the full path to the source file
            if [[ "$line" == *${sourcefile}* ]]; then
              line="${line%%${sourcefile}*}${sourcefile}"
              line="${line##* }"
              if [[ "$line" =~ ^/ ]]; then
                # aboslute path
                sourcefile=$line
              else
                # relative path
                sourcefile=$(realpath ${makedirs[-1]}/${line##* })
              fi
              sourcefiles[${#sourcefiles[@]}]="$sourcefile"
              parse_state="WAIT_FOR_INCLUDE_COMPILE_MAKE"
            fi;;
        esac
      done
      printf "\n"
      # sort and remove duplicates
      IFS=$'\n'; sourcefiles=($(sort --unique <<< "${sourcefiles[*]}")); unset IFS

      # extract include paths
      for source in ${sourcefiles[*]}; do
        includes[${#includes[@]}]="$(dirname ${source})"
      done
      # sort and remove duplicates
      IFS=$'\n'; includes=($(sort --unique <<< "${includes[*]}")); unset IFS

      # generate the .incldues file, containing all include paths
      echo "" > ${projectsdir}/${config}/${module}/${module}.includes
      for inc in ${includes[*]}; do
        echo "$inc" >> ${projectsdir}/${config}/${module}/${module}.includes
      done
      # generate the .files file, containing all source files
      echo "" > ${projectsdir}/${config}/${module}/${module}.files
      for source in ${sourcefiles[*]}; do
        # skip GCC files
        if [[ ! "$source" =~ .*/gcc.* ]]; then
          echo "$source" >> ${projectsdir}/${config}/${module}/${module}.files
        fi
      done
      # generate a default project configuration file if it doesn't exits yet
      if [ ! -f ${projectsdir}/${config}/${module}/${module}.config ]; then
        echo "// Add predefined macros for your project here. For example:" > ${projectsdir}/${config}/${module}/${module}.config
        echo "// #define YOUR_CONFIGURATION belongs here" >> ${projectsdir}/${config}/${module}/${module}.config
        echo "#define AMIRO_APPS true" >> ${projectsdir}/${config}/${module}/${module}.config
        echo "#define URT_CFG_OSAL_HEADER \"apps_urtosal.h\"" >> ${projectsdir}/${config}/${module}/${module}.config
      fi
      # generate a default .creator file if it doesn't exist yet
      if [ ! -f ${projectsdir}/${config}/${module}/${module}.creator ]; then
        echo "[general]" > ${projectsdir}/${config}/${module}/${module}.creator
      fi
    done
  done

  # go back to user directory
  cd "$userdir"

  # fill the output variables
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectsdir"
  fi
  if [ ! -z "$gccoutvar" ]; then
    eval $gccoutvar="$gccincludedir"
  fi

  return 0
}

### delete project files for a single or all modules ###########################
# Deletes all project files and optionally .user files, too.
#
# usage:      deleteProject [-p|--path=<path>] [-c|--configuration=<configuration>] [-m|--module=<module>] [-o|--out=<var>] [-w|-wipe=<flag>]
# arguments:  -p, --path <path>
#                 Path where to delete the project files.
#             -c, --configuration <configuration>
#                 Configuration name for which the project files shall be deleted or '*' to delete for all configurations.
#             -m, --modules <module>
#                 Module name for which the projcet files shall be deleted of '*' to delete for all modules.
#             -o, --out <var>
#                 Variable to store the path to.
#             -w, --wipe <flag>
#                 Flag whether to delete .user files as well (must be either "true" or "false").
# return:     0
#               No error or warning occurred.
#             1
#               Aborted by user.
#             2
#               There selected configuraion does not exist at the specified location.
#             3
#               There selected module does not exist for the selected configuration(s) at the specified location.
#
function deleteProject {
  local projectsdir=""
  local configuration=""
  local module=""
  local outvar=""
  local wipe=""
  local otherargs=()
  local configurations=()
  local userinput=""
  local configurations2delete=()
  local modules=()
  local modules2delete=()
 local files=()

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -p=*|--path=*)
          projectsdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectsdir=$(realpath "$2"); shift 2;;
        -c=*|--configuration=*)
          configuration="${1#*=}"; shift 1;;
        -c|--configuration)
          configuration="${2}"; shift 2;;
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
  while [ -z "$projectsdir" ]; do
    getProjectsDir projectsdir $(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../qtcreator)
    if [ -z "$projectsdir" ]; then
      printWarning "aborted by user\n"
      return 1
    elif [ ! -d "$projectsdir" ]; then
      printWarning "directory does not exist.\n"
      projectsdir=""
    fi
  done
  printf "\n"

  # retrieve all configurations in the specified directory
  configurations=($(find "${projectsdir}" -mindepth 1 -maxdepth 1 -type d | grep -oE "/[^/\.]+$" | grep -oE "[^/].+$"))

  # select configuration
  if [ -z "$configuration" ]; then
    # list all available configurations
    printInfo "select a configuration, type '*' for all or type 'A' to abort:\n"
    for (( idx=0; idx<${#configurations[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${configurations[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#configurations[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#configurations[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; then
        printWarning "Please enter an integer between 1 and ${#configurations[@]}, '*' to select all or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[0-9]+$ ]]; then
      # store selection
      configurations2delete=(${configurations[$(($userinput - 1))]})
    elif [[ "$userinput" =~ ^\*$ ]]; then
      configurations2delete=(${configurations[@]})
    elif [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    else
      printError "unexpected user input: ${userinput}\n"
      return -999
    fi
    printf "\n"
  else
    # check whether the specified project is available
    if [[ ! "${configurations[@]}" =~ "$configuration" ]]; then
      printWarning "there is no configuration '$configuration' at location '$projectsdir'\n"
      return 2
    else
      configurations2delete=($configuration)
    fi
  fi

  # retrieve all modules in the specified configurations
  modules=()
  for (( idx=0; idx<${#configurations2delete[@]}; ++idx )); do
    modules+=($(find "${projectsdir}/${configurations2delete[$idx]}" -mindepth 1 -maxdepth 1 -type d | grep -oE "/[^/\.]+$" | grep -oE "[^/].+$"))
  done
  IFS=$'\n'; modules=($(sort --unique <<< "${modules[*]}")); unset IFS

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
      printWarning "there is no module '$module' for configuration(s) '${modules2delete[*]}' at location '$projectsdir'\n"
      return 3
    else
      modules2delete=($module)
    fi
  fi

  # check for .user files
  files=()
  for config in ${configurations2delete[@]}; do
    for mod in ${modules2delete[@]}; do
      if [ -d "${projectsdir}/${config}/${mod}" ]; then
        files+=($(find "${projectsdir}/${config}/${mod}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${mod}\.creator\.user(\..+)?$"))
      fi
    done
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
  for config in ${configurations2delete[@]}; do
    for mod in ${modules2delete[@]}; do
      if [ -d "${projectsdir}/${config}/${mod}" ]; then
        if [ "$wipe" == "true" ]; then
          files+=($(find "${projectsdir}/${config}/${mod}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${project}\.(includes|files|config|creator|cflags|cxxflags|creator\.user(\..+)?)$"))
        else
          files+=($(find "${projectsdir}/${config}/${mod}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${project}\.(includes|files|config|creator|cflags|cxxflags)$"))
        fi
      fi
    done
  done

  # list all files to be deleted and ask for confirmation
  if [ ${#files[@]} -gt 0 ]; then
    printWarning "The following files will be deleted:\n"
    for file in ${files[@]}; do
      printWarning "\t${file#${projectsdir}/}\n"
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
  for config in ${configurations2delete[@]}; do
    for mod in ${modules2delete[@]}; do
      if [ -d "${projectsdir}/${config}/${mod}" ]; then
        rm -rf "${projectsdir}/${config}/${mod}/.qtc_clangd/"
      fi
    done
  done
  # remove all empty directories
  for config in $(find "${projectsdir}" -mindepth 1 -maxdepth 1 -type d); do
    for mod in $(find "${config}" -mindepth 1 -maxdepth 1 -type d -empty); do
      rm -rf "$mod"
    done
  done
  for config in $(find "${projectsdir}" -mindepth 1 -maxdepth 1 -type d -empty); do
    rm -rf "$config"
  done

  # store the path to the output variable, if required
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectsdir"
  fi

  return 0
}

### main function of this script ###############################################
# Creates, deletes and wipes QtCreator project files.
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
  local configurations=()
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

  # detect available configurations and inform user
  detectConfigurations configurations
  case "${#configurations[@]}" in
    0)
      printInfo "no configuration has been detected\n";;
    1)
      printInfo "1 configuration has been detected:\n";;
    *)
      printInfo "${#configurations[@]} configurations have been detected:\n";;
  esac
  for (( idx=0; idx<${#configurations[@]}; ++idx )); do
    printInfo "  - ${configurations[$idx]}\n"
  done
  printf "\n"

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -c=*|--create=*)
          createProject configurations[@] --configuration="${1#*=}"; printf "\n"; shift 1;;
        -c|--create)
          createProject configurations[@] --configuration="${2}"; printf "\n"; shift 2;;
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
        createProject configurations[@]; printf "\n";;
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

