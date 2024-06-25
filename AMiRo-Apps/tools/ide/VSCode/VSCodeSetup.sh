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
  printf "#                  Welcome to the VSCode setup!                      #\n"
  printf "#                                                                    #\n"
  printf "######################################################################\n"
  printf "#                                                                    #\n"
  printf "# Copyright (c..2022  Svenja Kenneweg                                #\n"
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
#             <inipath>
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
  local configs_detected=()
  local configsdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../configurations)

  # detect all available modules (via directories)
  for dir in $(ls -d ${configsdir}/*/); do
    configs_detected[${#configs_detected[@]}]=$(basename $dir)
  done

  # set the output variable
  eval "$1=(${configs_detected[*]})"
}

### create project files for a single or for all configurations ################
# Create project files for all modules of a single or of all configurations.
#
# usage:      createProject <configurations> [-c|--configuration=<configuration>] [-p|--path=<path>] [--gcc=<path>] [-o|--out=<var>] [--gccout=<var>]
# arguments:  <configurations>
#                 Array containing all configurations available.
#             -c, --configuration <configuration>
#                 Name (folder name) of the configuration for which project files shall be generated.
#                 Set <configuration> to '*' in order to generate projects for all available configurations.
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
#
function createProject {
  local config=""
  local configs=("${!1}")
  local configs2generate=()
  local modules2generate=()
  local configsdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../configurations)
  local gccincludedir=""
  local gccoutvar=""
  local includes=()
  local makedir=""
  local makedirs=()
  local modules=()
  local otherargs=()
  local outvar=""
  local parse_state=""
  local projectfiles=()
  local projects=()
  local projectsdir=""
  local projectsconfigdir=""
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
        -pc=*|--path=*)
          projectsconfigdir=$(realpath "${1#*=}"); shift 1;;
        -pc|--path)
          projectsconfigdir=$(realpath "$2"); shift 2;;
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

  # generate project names for user selected modules of all selected configurations
  for config in ${configs2generate[@]}; do
    if [ -d ${configsdir}/${config}/modules ]; then
      modules=($(find ${configsdir}/${config}/modules/ -mindepth 1 -maxdepth 1 -type d))
      if [ ${#modules[@]} -gt 0 ]; then
        # list all available modules
        printInfo "select a module, type '*' for all or type 'A' to abort:\n"
        for (( idx=0; idx<${#modules[@]}; ++idx )); do
          printf "%4u: %s\n" $(($idx + 1)) "$(basename ${modules[$idx]})"
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
        for module in ${modules2generate[@]}; do
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
    getProjectsDir projectsdir $(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../vscode)
    printf "\n"
  fi
  # check whether the directory exists and create it if required
  if [ ! -d "$projectsdir" ]; then
    printWarning "Creating nonexistent directory '$projectsdir'.\n"
    mkdir -p "$projectsdir"
  fi
  
  # read vsCode workbench config directory if required
  if [ -z "$projectsconfigdir" ]; then
    projectsconfigdir=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/../../../.vscode)
    printf "\n"
  fi
  # check whether the directory exists and create it if required
  if [ ! -d "$projectsconfigdir" ]; then
    printWarning "Creating nonexistent directory '$projectsconfigdir'.\n"
    mkdir -p "$projectsconfigdir"
  fi

  # check for existing project files
  for project in ${projects[@]}; do
    projectfiles+=($(find ${projectsdir} -maxdepth 1 -type f -regextype posix-extended -regex "^.*/${project}\.(code-workspace)$" | sort))
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

  # generate for all configurations
  cd "$projectsdir"
  for config in ${configs2generate[@]}; do
    # retrieve all modules
    modules=()
    for module in ${modules2generate[@]}; do
      modules+=($(basename $module))
    done
    # silently skip this configuration if it contains no modules
    if [ ${#modules[@]} == 0 ]; then
      continue
    fi

    printInfo "generating VSCode projects for configuration '${config}'...\n"e
    for module in ${modules[@]}; do
      # run make, but only run the GCC preprocessor and produce no binaries
      sourcefiles=()
      sourcefile=""
      parse_state="WAIT_FOR_INCLUDE_COMPILE_MAKE"
      makedir=""
      makedirs=()
      # capture all output from make and GCC and append the return value of make as last line
      printInfo "processing module ${module} (this may take a while)...\n"
      rawout=$(make --directory ${configsdir}/${config}/modules/${module} --always-make USE_OPT="-v -E -H" USE_VERBOSE_COMPILE="no" OUTFILES="" 2>&1 && echo $?)
      # check whether the make call was successfull
      if [[ $(echo "${rawout}" | tail -n 1) != "0" ]]; then
        printError "executing 'make' in configuration directory failed\n"
        cd "$userdir"
        return -3
      else
        # cleanup
        make --directory ${configsdir}/${config}/modules/${module} clean &>/dev/null
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
              includes[${#includes[@]}]="$(dirname ${sourcefile})" #CHANGED
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
              # if the makedirs stack is empty or the directory does not mathc the last entry
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
              includes[${#includes[@]}]="$(dirname ${sourcefile})" #CHANGED
              parse_state="WAIT_FOR_INCLUDE_COMPILE_MAKE"
            fi;;
        esac
      done
      printf "\n"
      # sort and remove duplicates
      IFS=$'\n'; sourcefiles=($(sort --unique <<< "${sourcefiles[*]}")); unset IFS

      # sort and remove duplicates
      IFS=$'\n'; includes=($(sort --unique <<< "${includes[*]}")); unset IFS

      local amirodir="$configsdir/../*"
      local hiddenFiles=()
      local visibleFiles=()
      for file in $amirodir; do
        filethere=0
        for sources in ${sourcefiles[*]}; do
          # check if file is somewhere in the sources
          if [[ "$sources" == *"$(realpath $file)"* ]]; then
            filethere=1
          fi
        done
        if [ $filethere -eq 1 ]; then
          filethere=0
          visibleFiles[${#visibleFiles[@]}]=$(realpath $file)
        else
          hiddenFiles[${#hiddenFiles[@]}]=$(realpath $file)
        fi
      done

      gothroughfiles=1
      local tempVisible=()
      while [ $gothroughfiles -eq 1 ]; do
        tempVisible=()
        for file in ${visibleFiles[*]}; do
          amirodir="$file/*"
          printf "."
          for filetwo in $amirodir; do
            filethere=0
            for sources in ${sourcefiles[*]}; do
              # check if file is somewhere in the sources
              if [[ "$sources" == *"$(realpath $filetwo)"* ]]; then
                filethere=1
              fi
            done
            if [ $filethere -eq 1 ]; then
              filethere=0
              if [[ -d $(realpath $filetwo) ]]; then
                tempVisible[${#tempVisible[@]}]=$(realpath $filetwo)
              fi
            else
              hiddenFiles[${#hiddenFiles[@]}]=$(realpath $filetwo)
            fi
          done
        done
        if [ ${#tempVisible[@]} -eq 0 ]; then
          gothroughfiles=0
        else
          visibleFiles=("${tempVisible[@]}")
        fi
      done
      printf "\n"

      #generate the code-workspace file, containing all relevant settings for this vscode workbench
      echo "" > ${projectsdir}/${config}_${module}.code-workspace
      echo "{"'"folders"'": [{" >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"path"'": "'"../"'"," >> ${projectsdir}/${config}_${module}.code-workspace
      echo '"name": "'${config}_${module}'"}],' >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"settings"'": {" >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"files.associations"'": {" >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"*.h"'": "'"c"'"," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"*.c"'": "'"c"'"," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"*.cpp"'": "'"cpp"'"," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"*.hpp"'": "'"cpp"'"}," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"files.exclude"'": {" >> ${projectsdir}/${config}_${module}.code-workspace
      printf "write hidden files"
      for hiddenFile in ${hiddenFiles[*]}; do
        if [[ -d $hiddenFile ]]; then
          echo ""'"**/'$(basename $hiddenFile)'"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
        else
          echo ""'"**/'$(basename $hiddenFile)'"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
        fi
      done
      echo ""'"**/.dep"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"**/build"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"**/.gitignore"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
      echo ""'"**/.gitmodules"'": true," >> ${projectsdir}/${config}_${module}.code-workspace
      echo "}}}" >> ${projectsdir}/${config}_${module}.code-workspace

      #generate the c_cpp_properties.json containing all relevant settings for the c extension
      if [ -f ${projectsconfigdir}/c_cpp_properties.json ]; then
        grep -q ${config}_$(basename ${module}) ${projectsconfigdir}/c_cpp_properties.json
        if [ $? -eq 0 ]; then
          # remove everything between the two matching strings
          sed -i 's|"'${module}'/apps.c",.*"'${module}'/apps.c"|"'${module}'/apps.c", "'${module}'/apps.c"|' ${projectsconfigdir}/c_cpp_properties.json
          # write the new includes into the file
          for includeFile in ${includes[*]}; do
            sed -i 's|"'${module}'/apps.c",|& "'$includeFile'",|' ${projectsconfigdir}/c_cpp_properties.json
          done
        else
          #Remove the last two lines
          sed -i '$d' ${projectsconfigdir}/c_cpp_properties.json
          sed -i '$d' ${projectsconfigdir}/c_cpp_properties.json
          #Add the new configuration
          echo "}," >> ${projectsconfigdir}/c_cpp_properties.json
          echo "{" >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"name"'": "'"'${config}_$(basename ${module})'"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"includePath"'": [" >> ${projectsconfigdir}/c_cpp_properties.json
          echo -n '"'${module}'/apps.c"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          for includeFile in ${includes[*]}; do
            echo -n '"'$includeFile'"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          done
          echo '"'${module}'/apps.c"' >> ${projectsconfigdir}/c_cpp_properties.json
          echo "],"'"defines"'": [" >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"AMIRO_APPS=true"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"URT_CFG_OSAL_HEADER=\"apps_urtosal.h\""' >> ${projectsconfigdir}/c_cpp_properties.json
          echo "]," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"cStandard"'": "'"c17"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"cppStandard"'": "'"c++17"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"intelliSenseMode"'": "'"linux-gcc-arm64"'"," >> ${projectsconfigdir}/c_cpp_properties.json
          echo "}]," >> ${projectsconfigdir}/c_cpp_properties.json
          echo '"version"'": 4 }" >> ${projectsconfigdir}/c_cpp_properties.json
        fi
      else
        #Add the new configuration
        echo "" > ${projectsconfigdir}/c_cpp_properties.json
        echo "{"'"configurations"'": [{" >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"name"'": "'"'${config}_$(basename ${module})'"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"includePath"'": [" >> ${projectsconfigdir}/c_cpp_properties.json
        echo -n '"'${module}'/apps.c"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        for includeFile in ${includes[*]}; do
          echo -n '"'$includeFile'"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        done
        echo '"'${module}'/apps.c"' >> ${projectsconfigdir}/c_cpp_properties.json
        echo "],"'"defines"'": [" >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"AMIRO_APPS=true"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"URT_CFG_OSAL_HEADER=\"apps_urtosal.h\""' >> ${projectsconfigdir}/c_cpp_properties.json
        echo "]," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"cStandard"'": "'"c17"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"cppStandard"'": "'"c++17"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"intelliSenseMode"'": "'"linux-gcc-arm64"'"," >> ${projectsconfigdir}/c_cpp_properties.json
        echo "}]," >> ${projectsconfigdir}/c_cpp_properties.json
        echo '"version"'": 4 }" >> ${projectsconfigdir}/c_cpp_properties.json
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
# usage:      deleteProject [-p|--path=<path>] [-m|--module=<project>] [-o|--out=<var>] [-w|-wipe=<flag>]
# arguments:  -p, --path <path>
#                 Path where to delete the project files.
#             -m, --module <project>
#                 Project name for which the project files shall be deleted or '*' to delete for all modules.
#             -o, --out <var>
#                 Variable to store the path to.
#             -w, --wipe <flag>
#                 Flag whether to delete .user files as well (must be either "true" or "false").
# return:     0
#               No error or warning occurred.
#             1
#               Aborted by user.
#             2
#               There are no project files for the specified module at the specified location.
#
function deleteProject {
  local files=()
  local otherargs=()
  local outvar=""
  local project=""
  local projects=()
  local projects2delete=()
  local projectsdir=""
  local userinput=""
  local wipe=""

  # parse arguments
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -p=*|--path=*)
          projectsdir=$(realpath "${1#*=}"); shift 1;;
        -p|--path)
          projectsdir=$(realpath "$2"); shift 2;;
        -m=*|--module=*)
          project="${1#*=}"; shift 1;;
        -m|--module)
          project="${2}"; shift 2;;
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
    if [ ! -d "$projectsdir" ]; then
      printWarning "directory does not exist.\n"
      projectsdir=""
    fi
  done
  printf "\n"

  # retrieve all projects in the specified directory
  projects=($(find "${projectsdir}" -maxdepth 1 -type f -regextype posix-extended -regex "^.+\.(includes|files|config|creator|cflags|cxxflags|creator\.user(\..+)?)$" | grep -oE "/[^/\.]+\." | grep -oE "[^/].+[^\.]" | sort --unique))

  # check whether there are any project files at the specified location
  if [ ${#projects[@]} -eq 0 ]; then
    printWarning "no projects detected at '${projectsdir}'\n"
    return 2
  fi

  # select project
  if [ -z "$project" ]; then
    # list all available projects
    printInfo "select a project, type '*' for all or type 'A' to abort:\n"
    for (( idx=0; idx<${#projects[@]}; ++idx )); do
      printf "%4u: %s\n" $(($idx + 1)) "${projects[$idx]}"
    done
    # read user input
    printLog "read user selection\n"
    userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#projects[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#projects[@]} ] && [[ ! "$userinput" =~ ^(\*|[Aa])$ ]]; then
        printWarning "Please enter an integer between 1 and ${#projects[@]}, '*' to select all or 'A' to abort.\n"
      fi
    done
    if [[ "$userinput" =~ ^[0-9]+$ ]]; then
      # store selection
      projects2delete=(${projects[$(($userinput - 1))]})
    elif [[ "$userinput" =~ ^\*$ ]]; then
      projects2delete=(${projects[@]})
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
    if [[ ! "${projects[@]}" =~ "$project" ]]; then
      printWarning "there are no files for project '$project' at location '$projectsdir'\n"
      return 2
    else
      projects2delete=($project)
    fi
  fi

  # check for .user files
  files=()
  for project in ${projects2delete[@]}; do
    files+=($(find "${projectsdir}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${project}\.creator\.user(\..+)?$"))
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
  for project in ${projects2delete[@]}; do
    if [ "$wipe" == "true" ]; then
      files+=($(find "${projectsdir}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${project}\.(includes|files|config|creator|cflags|cxxflags|creator\.user(\..+)?)$"))
    else
      files+=($(find "${projectsdir}" -maxdepth 1 -type f -regextype posix-extended -regex "^.*${project}\.(includes|files|config|creator|cflags|cxxflags)$"))
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
  printInfo "${#files[@]} files have been deleted\n"

  # store the path to the output variable, if required
  if [ ! -z "$outvar" ]; then
    eval $outvar="$projectsdir"
  fi

  return 0
}

### main function of this script ###############################################
# Creates, deletes and wipes VSCode project files.
#
# usage:      see function printHelp
# arguments:  see function printHelp
# return:     0
#                 No error or warning ocurred.
#
function main {
  local cmd=""
  local cmdidx=""
  local configurations=()
  local filenameidx=""
  local logfile=""
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
    printInfo "VSCode setup main menu\n"
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
