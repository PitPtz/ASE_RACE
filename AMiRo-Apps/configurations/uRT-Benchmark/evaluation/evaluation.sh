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

# this script
scriptdir=$(dirname ${BASH_SOURCE[0]})

# print help
if [[ $# == 0 ]]; then
  printf "usage:    $(basename ${BASH_SOURCE[0]}) DIR [OUTFILE]\n"
  printf "\n"
  printf "options:  DIR\n"
  printf "              Input path, containing µRT benchmark logs.\n"
  printf "              Wildcards need to be escaped with a backslash.\n"
  printf "          OUTFILE\n"
  printf "              Optional filename where script output will be printed to.\n"
  printf "              An according fil will be created in each input directory, overwriting existing files.\n"
  exit
fi

# parse and check arguments
inputdirs=$(find $1 -maxdepth 0 -type d)
if [[ $? != 0 ]]; then
  exit -1
fi

# execute evaluation
for dir in ${inputdirs}; do
  if [[ -z $2 ]]; then
    nice "${scriptdir}/Evaluation.py" "$dir" --iteration-range 1: --output --plot parsed evaluated --mode async --force
  else
    nice "${scriptdir}/Evaluation.py" "$dir" --iteration-range 1: --output --plot parsed evaluated --mode async --force > "${dir}/$2"
  fi
done
