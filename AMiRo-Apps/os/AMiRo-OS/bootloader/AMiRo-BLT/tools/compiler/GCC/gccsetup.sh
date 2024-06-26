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
  printf "#                     Welcome to the GCC setup!                      #\n"
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
  printf "usage:    $(basename ${BASH_SOURCE[0]}) [-h|--help] [-i|--install] [-c|--change] [-q|--quit] [--log=<file>]\n"
  printf "\n"
  printf "options:  -h, --help\n"
  printf "              Print this help text.\n"
  printf "          -i, --install\n"
  printf "              Install a first compiler or another version.\n"
  printf "          -u, --uninstall\n"
  printf "              Unistall a version.\n"
  printf "          -c, --change\n"
  printf "              Change the default version.\n"
  printf "          --quit\n"
  printf "              Quit the script.\n"
  printf "          --log=<file>\n"
  printf "              Specify a log file.\n"
}

### detect installed versions ##################################################
# Detect all installed version of arm-none-eabi-gcc, if any.
#
# usage:      detectInstalledVersions <binarray> <current> [<current_idx>]
# arguments:  <binarray>
#                 Array variable to store all detected binary paths to. 
#             <current>
#                 Variable to store the currently active binary to.
#             <current_idx>
#                 Index of the curretly selected version in the output array (<binarray>).
# return:     n/a
#
function detectInstalledVersions {
  local armgcc_command=$(command -v arm-none-eabi-gcc)
  local armgcc_commanddir=${HOME}/gcc-none-eabi
  local armgcc_currentbin=""
  local armgcc_installdir=${HOME}/gcc-none-eabi
  local armgcc_bins=()
  local armgcc_bincnt=0

  # check for already installed versions
  if [ -n "$armgcc_command" ]; then
    # follow the link to the actual binary
    armgcc_commanddir=$(dirname $armgcc_command)
    armgcc_currentbin=$armgcc_command
    while [ -L $armgcc_currentbin ]; do
      # differentiate between relative and absolute paths
      if [[ $(readlink $armgcc_currentbin) = /* ]]; then
        armgcc_currentbin=$(readlink $armgcc_currentbin)
      else
        armgcc_currentbin=$(realpath $(dirname $armgcc_currentbin)/$(readlink $armgcc_currentbin))
      fi
    done
    # the installation location is assumed to be two directories up
    armgcc_installdir=$(realpath $(dirname ${armgcc_currentbin})/../..)
    # list all detected instalations
    for dir in $(ls -d ${armgcc_installdir}/*/); do
      if [ -f ${dir}/bin/arm-none-eabi-gcc ]; then
        armgcc_bins[$armgcc_bincnt]=${dir}bin/arm-none-eabi-gcc
        armgcc_bincnt=$((armgcc_bincnt + 1))
      fi
    done

    # set the output variables
    eval "$1=(${armgcc_bins[*]})"
    eval $2="$armgcc_currentbin"
    if [ -n "$3" ]; then
      for (( bin=0; bin<${#armgcc_bins[@]}; ++bin )); do
        if [ ${armgcc_bins[bin]} = "$armgcc_currentbin" ]; then
          eval $3=$bin
        fi
      done
    fi
  else
    eval "$1=()"
    eval $2=""
    if [ -n "$3" ]; then
      eval $3=""
    fi
  fi
}

### install new version ########################################################
# Fetches an installation package from the internet, installs it and expands
# the $PATH environment variable (via .bashrc) if required.
#
# usage:      installNewVersion [-i|--install=<path>] [-l|--link=<path>]
# argumenst:  -i, --install <path>
#                 Path where to install the new version to.
#             -l, --link <path>
#                 Path where to create according links.
# return:     0
#                 No error or warnign occurred.
#             1
#                 Warning: Installation aborted by user.
#             -1
#                 Error: specified URL can not be reached.
#             -2
#                 Error: Missing dependecny.
#
function installNewVersion {
  local installbasedir=${HOME}/gcc-arm-embedded
  local linkdir="/usr/bin"

  # check dependencies
  checkCommands wget
  if [ $? -ne 0 ]; then
    printError "Missing dependencies detected.\n"
    return -2
  fi

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -i=*|--install=*)
          installbasedir=$(realpath "${1#*=}"); shift 1;;
        -i|--install)
          installbasedir="$2"; shift 2;;
        -l=*|--link=*)
          linkdir=$(realpath "${1#*=}"); shift 1;;
        -l|--link)
          linkdir="$2"; shift 2;;
        *) # sanity check (exit with error)
          printError "invalid option: $1\n"; shift 1;;
      esac
    else
      otherargs+=("$1")
      shift 1
    fi
  done

  # read download URL from user
  printf "In order to install a compiler, you have to specify a download link for the according installation file.\n"
  printf "For this project, the GNU Arm Embedded Toolchain is recommended:\n"
  printf "  https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm\n"
  printf "Search the download area for the desired version and select the package according to your system.\n"
  printf "\n"
  printLog "read installation url from user\n"
  local armgcc_downloadurl=""
  while [ -z "$armgcc_downloadurl" ]; do
    read -p "Download link for the installation file: " -e armgcc_downloadurl
    if [ -z "$armgcc_downloadurl" ]; then
      printWarning "installation aborted by user\n"
      return 1
    fi
    # check whether url is valid
    wget --spider -r "$armgcc_downloadurl" &>/dev/null
    if [ $? -ne 0 ]; then
      printError "'$armgcc_downloadurl' can not be reached\n"
      return -1
    fi
  done
  printLog "user selected $armgcc_downloadurl\n"

  # if the file already exists, ask the user if it should be downloaded again
  local armgcc_tarball=$(basename $(wget --spider -r "$armgcc_downloadurl" 2>&1 | \
                                    grep "^--" | \
                                    tail -n 1 | \
                                    awk '{print $NF}'))
  if [ -f "$armgcc_tarball" ]; then
    printWarning "$armgcc_tarball already exists. Delete and redownload? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        rm "$armgcc_tarball"
        wget "$armgcc_downloadurl" -O "$armgcc_tarball" | tee -a $LOG_FILE
        ;;
      N|n)
        ;;
      *) # sanity check (exit with error)
        printError "unexpected argument: $userinput\n";;
    esac
  else
    wget "$armgcc_downloadurl" -O "$armgcc_tarball" | tee -a $LOG_FILE
  fi

  # extract tarball
  printInfo "extracting ${armgcc_tarball}...\n"
  tar -axf "${armgcc_tarball}" | tee -a $LOG_FILE
  local compilerdir=`tar -atf "${armgcc_tarball}" | sed -e 's@/.*@@' | uniq`

  # install gcc arm embedded
  printLog "read installation directory from user\n"
  local installdir=""
  read -p "Installation directory: " -i ${installbasedir}/${compilerdir} -e installdir
  installdir=$(sed "s;^~\($\|/\);${HOME}/;" <<< ${installdir})
  printLog "user selected $installdir\n"
  linkdir=$(dirname ${installdir})
  printLog "read link directory\n"
  read -p "Link directory: " -i $linkdir -e linkdir
  linkdir=$(sed "s;^~\($\|/\);${HOME}/;" <<< ${linkdir})
  printLog "user selected $linkdir\n"
  # if the installation path already exists, ask user to overwrite
  if [ -d "$installdir" ]; then
    printWarning "$installdir already exists. Overwrite? [y/n]\n"
    local userinput=""
    readUserSelection "YyNn" 'n' userinput
    case "$userinput" in
      Y|y)
        ;;
      N|n)
        printWarning "installation aborted by user\n"
        return 1
        ;;
      *) # sanity check (exit with error)
        printError "invalid option: $userinput\n";;
    esac
  # make sure the whole ínstallation path exists
  else
    while [ ! -d $(dirname "$installdir") ]; do
      local dir=$(dirname "$installdir") 
      while [ ! -d $(dirname "$dir") ]; do
        dir=$(dirname "$dir")
      done
      echo "mkdir $dir"
      mkdir "$dir"
    done
  fi
  # move the extracted compiler folder
  mv -f "$compilerdir" "$installdir"
  # make sure whole link path exists
  while [ ! -d "$linkdir" ]; do
    local dir="$linkdir"
    while [ ! -d $(dirname "$linkdir") ]; do
      dir=$(dirname "$dir")
    done
    mkdir "$dir"
  done
  # create / overwrite links
  local linkpath=$(realpath --relative-base=$linkdir ${installdir}/bin/)
  ls ${installdir}/bin/ | xargs -i ln -sf ${linkpath}/{} ${linkdir}/{}
  printInfo "default version set to $(${linkdir}/arm-none-eabi-gcc -dumpversion)\n"

  # append the link directory to the PATH environment variable if required
  if [[ ! "$linkdir" = *"$PATH"* ]]; then
    local bashrc_file=${HOME}/.bashrc
    local bashrc_identifier="##### AMiRo ENVIRONMENT CONFIGURATION #####"
    local bashrc_note="# DO NOT EDIT THESE LINES MANUALLY!"
    local bashrc_entry="export PATH=\$PATH:$linkdir"

    # find and edit old entry, or append a new one to the file
    local bashrc_idlines=$(grep -x -n "$bashrc_identifier" "$bashrc_file" | cut -f1 -d:) # string of line numbers
    bashrc_idlines=(${bashrc_idlines//"\n"/" "}) # array of line numbers
    case ${#bashrc_idlines[@]} in

      # append a new entry to the BASHRC_FILE
      0)
        # make sure the last line is empty
        if [[ ! $(tail -1 $bashrc_file) =~ ^[\ \t]*$ ]]; then
          printf "\n" >> $bashrc_file
        fi
        # append text to file
        sed -i --follow-symlinks '$a'"$bashrc_identifier\n$bashrc_note\n$bashrc_entry\n$bashrc_identifier\n" $bashrc_file
        # print note
        printInfo "Your $bashrc_file has been updated. You need to source it to apply the changes in your environment.\n"
        read -p "  Understood!"
        ;;

      # extend the old entry
      2)
        # don't do anything if the line is already present
        local bashrc_entrylines=$(grep -x -n "$bashrc_entry" $bashrc_file | cut -f1 -d:) # string of line numbers
        bashrc_entrylines=(${bashrc_entrylines//"\n"/" "}) # array of line numbers
        if [[ ${#bashrc_entrylines[@]} -eq 0 ]]; then
          # insert the entry before the closing identifier
          sed -i --follow-symlinks "${bashrc_idlines[1]}"'i'"$bashrc_entry" $bashrc_file
          # print note
          printInfo "Your $bashrc_file has been updated. You need to source it to apply the changes in your environment.\n"
          read -p "  Understood!"
        elif [[ ${#bashrc_entrylines[@]} -eq 1 && ( ${bashrc_entrylines[0]} -lt ${bashrc_idlines[0]} || ${bashrc_entrylines[0]} -gt ${bashrc_idlines[1]} ) ]]; then
          # print an error that there is an entry at the wrong place
          printError "corrupted entry in your $bashrc_file detected\n"
          printf "The following entry was found at the wrong place:\n"
          printf "\n"
          printf "$bashrc_entry\n"
          printf "\n"
          printf "To fix this, delete the line and rerun this setup.\n"
          read -p "  Understood!"
        elif [[ ${#bashrc_entrylines[@]} -gt 1 ]]; then
          # print an error that there are multiple entries
          printError "corrupted entry in your $bashrc_file detected\n"
          printf "There are multiple identical entries in your $bashrc_file file.\n"
          printf "To fix it, make sure that it contains the following line exactly once:\n"
          printf "\n"
          printf "$bashrc_entry\n"
          printf "\n"
          read -p "  Understood!"
        fi
        ;;

      # error state (corrupted entry detected)
      *)
        printError "unable to append link directory to \$PATH variable\n"
        printf "There seems to be a broken entry in your $bashrc_file file.\n"
        printf "To fix it, make sure that the following line appears exactly twice and encloses your AMiRo related settings:\n"
        printf "\n"
        printf "$bashrc_identifier\n"
        printf "\n"
        read -p "  Understood!"
        ;;
    esac
  fi

  # clean up the current directory
  rm "$armgcc_tarball"

  return 0
}

### uninstall a version ########################################################
# Select an installed version and uninstall it from the system.
#
# usage:      uninstallVersion <versions> <current_idx> <linkdir>
# arguments:  <version>
#                 Array of available versions (full path to binary).
#             <current_idx>
#                 Index of the currently selected version in the array.
#             <linkdir>
#                 Path where to delete old links.
# return:     0
#                 No error or warning occurred.
#             1
#                 Warning: Installation aborted by user.
#             -1
#                 Error: An exception occurred.
#
function uninstallVersion {
  local versions=("${!1}")
  local current_idx="$2"
  local linkdir="$3"

  # check whether at least two installations were detected
  if [ ${#versions[@]} -eq 0 ]; then
    printError "no installation detected\n"
    return -1
  else
    # print all available versions
    printInfo "choose the installation to uninstall to or type 'A' to abort:\n"
    for (( cnt=0; cnt<${#versions[@]}; ++cnt )); do
      if [ $cnt -eq $current_idx ]; then
        printf "*%3u: %s\n" $(($cnt + 1)) ${versions[$cnt]}
      else
        printf " %3u: %s\n" $(($cnt + 1)) ${versions[$cnt]}
      fi
    done

    # read user selection
    printLog "read user slection\n"
    local userinput=""
    while [ -z $userinput ] ; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#versions[@]} ] && [[ ! "$userinput" =~ ^[Aa]$ ]]; then
        printWarning "Please enter an integer between 1 and ${#versions[@]} or 'A' to abort.\n"
        userinput=""
      fi
      if [ ${#versions[@]} -gt 1 ] && [ $((userinput - 1)) -eq $current_idx ]; then
        printWarning "Unable to uninstall currently selected version (as long as there are others).\n"
        userinput=""
      fi
    done

    if [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
      return 1
    else
      local idx=$((userinput - 1))
      printf "\n"
      # prompt selected and aks user for confirmation
      printInfo "${versions[$idx]} will be removed. Continue? [y/n]\n"
      readUserSelection "YyNn" 'n' userinput
      case "$userinput" in
        Y|y)
          ;;
        N|n)
          printWarning "uninstallation process aborted by user\n"
          return 1
          ;;
        *) # sanity check (exit with error)
          printError "invalid option: $userinput\n"
          return -1
          ;;
      esac
      # find and delete any links pointing to the version to be deleted
      for link in `find $linkdir -maxdepth 1 -type l`; do
        local l=$link
        # follow the link to the actual binary
        while [ -L $l ]; do
          # differentiate between relative and absolute paths
          if [[ $(readlink $l) = /* ]]; then
            l=$(readlink $l)
          else
            l=$(realpath $(dirname $l)/$(readlink $l))
          fi
        done
        # delete the link if it points to the version to be uninstalled
        if [ $(dirname $l) == $(dirname ${versions[$idx]}) ]; then
          rm $link
        fi
      done
      # delete the version directory (assumed to be one directory up)
      rm -rf $(realpath $(dirname ${versions[$idx]})/..)
      printInfo "${versions[$idx]} has been removed.\n"
    fi
  fi

  return 0
}

### change default version #####################################################
# Change the default arm-none-eabi-gcc version.
#
# usage:      changeDefaultVersion <versions> <linkdir>
# argumenst:  <versions>
#                 Array of available versions (full path to binary).
#             <linkdir>
#                 Path where to delete old and create new links.
# return:     0
#                 No error or warnign occurred.
#             -1
#                 Error: no installation detected.
#
function changeDefaultVersion {
  local versions=("${!1}")
  local linkdir="$2"

  # check whether an installation was detected
  if [ ${#versions[@]} -eq 0 ]; then
    printError "no installation detected\n"
    return -1
  else
    # print all available versions
    printInfo "choose the installation to switch to or type 'A' to abort:\n"
    for (( cnt=0; cnt<${#versions[@]}; ++cnt )); do
      printf "  %2u: %s\n" $(($cnt + 1)) ${versions[$cnt]}
    done

    # read user selection
    printLog "read user slection\n"
    local userinput=""
    while [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#versions[@]} ] && [[ ! "$userinput" =~ ^[Aa]$ ]]; do
      read -p "your selection: " -e userinput
      printLog "user selection: $userinput\n"
      if [[ ! "$userinput" =~ ^[0-9]+$ ]] || [ ! "$userinput" -gt 0 ] || [ ! "$userinput" -le ${#versions[@]} ] && [[ ! "$userinput" =~ ^[Aa]$ ]]; then
        printWarning "Please enter an integer between 1 and ${#versions[@]} or 'A' to abort.\n"
      fi
    done

    if [[ "$userinput" =~ ^[Aa]$ ]]; then
      printWarning "aborted by user\n"
    else
      local idx=$((userinput - 1))
      # find and delete old links
      rm `find $linkdir -maxdepth 1 -type l | grep -Ev "*[0-9]\.[0-9]\.[0-9]"`
      # create new links with relative or absolute paths
      local bindir=$(dirname ${versions[$idx]})
      local linkpath=$(realpath --relative-base=$linkdir $bindir)
      ls $bindir | xargs -i ln -sf $linkpath/{} $linkdir/{}
      printInfo "default version set to $(arm-none-eabi-gcc -dumpversion)\n"
    fi
  fi

  return 0
}

### main function of this script ###############################################
# The IDE setup lets the user select an IDE of choice.
# As of now, only QtCreator is supported.
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

  # detect installed versions and inform user
  local installedversions=()
  local currentversion=""
  local currentversionidx="n/a"
  detectInstalledVersions installedversions currentversion currentversionidx
  case "${#installedversions[@]}" in
    0)
      printInfo "no installation has been detected\n";;
    1)
      printInfo "1 installation has been detected:\n";;
    *)
      printInfo "${#installedversions[@]} installations have been detected:\n";;
  esac
  for (( idx=0; idx<${#installedversions[@]}; ++idx )); do
    if [ ${installedversions[$idx]} = "$currentversion" ]; then
      printInfo "  * ${installedversions[$idx]}\n"
    else
      printInfo "    ${installedversions[$idx]}\n"
    fi
  done
  printf "\n"

  # parse arguments
  local otherargs=()
  while [ $# -gt 0 ]; do
    if ( parseIsOption $1 ); then
      case "$1" in
        -h|--help) # already handled; ignore
          shift 1;;
        -i|--install)
          if [ -z "$currentversion" ]; then
            installNewVersion
          else
            installNewVersion --install=$(realpath $(dirname $currentversion)/../..) --link=$(realpath $(dirname $currentversion)/../..)
          fi
          detectInstalledVersions installedversions currentversion currentversionidx
          printf "\n"; shift 1;;
        -u|--uninstall)
          if [ ! -z "$currentversion" ]; then
            uninstallVersion installedversions[@] $currentversionidx $(realpath $(dirname $currentversion)/../..)
            detectInstalledVersions installedversions currentversion currentversionidx
          else
            printError "no installation detected\n"
          fi
          printf "\n"; shift 1;;
        -c|--change)
          if [ ! -z "$currentversion" ]; then
            changeDefaultVersion installedversions[@] $(realpath $(dirname $currentversion)/../..)
          else
            printError "no installation detected\n"
          fi
          printf "\n"; shift 1;;
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
    # update list of installed compilers
    detectInstalledVersions installedversions currentversion currentversionidx

    # main menu info prompt and selection
    printInfo "GCC setup main menu\n"
    printf "Please select one of the following actions:\n"
    if [ ${#installedversions[@]} -eq 0 ]; then
      printf "   [I]  - install a first compiler\n"
    else
      printf "   [I]  - install another version\n"
    fi
    printf "   [U]  - uninstall a version\n"
    printf "   [C]  - change default version\n"
    printf "  [ESC] - quit this setup\n"
    local userinput=""
    readUserSelection "IiUuCc" 'y' userinput
    printf "\n"

    # evaluate user selection
    case "$userinput" in
      I|i)
        if [ -z "$currentversion" ]; then
          installNewVersion
        else
          installNewVersion --install=$(realpath $(dirname $currentversion)/../..) --link=$(realpath $(dirname $currentversion)/../..)
        fi
        detectInstalledVersions installedversions currentversion currentversionidx
        printf "\n";;
      U|u)
        if [ ! -z "$currentversion" ]; then
          uninstallVersion installedversions[@] $currentversionidx $(realpath $(dirname $currentversion)/../..)
          detectInstalledVersions installedversions currentversion currentversionidx
        else
          printError "no installation detected\n"
        fi
        printf "\n";;
      C|c)
        if [ ! -z "$currentversion" ]; then
          changeDefaultVersion installedversions[@] $(realpath $(dirname $currentversion)/../..)
        else
          printError "no installation detected\n"
        fi
        printf "\n";;
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

