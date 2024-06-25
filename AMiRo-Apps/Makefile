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



# absolue path to this directory
AMIROAPPS_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available configurations
CONFIGURATIONS_DIR = $(AMIROAPPS_DIR)/configurations/
CONFIGURATIONS = $(patsubst $(CONFIGURATIONS_DIR)%/,%,$(sort $(dir $(wildcard $(CONFIGURATIONS_DIR)*/*))))

CONFIGURATIONS_CLEANTRGS = $(CONFIGURATIONS:%=clean_%)

# export all custom variables
unexport AMIROAPPS_DIR CONFIGURATIONS_DIR CONFIGURATIONS CONFIGURATIONS_CLEANTRGS
export

.PHONY: all clean $(CONFIGURATIONS) $(CONFIGURATIONS_CLEANTRGS)

all:
	$(MAKE) -j$(words $(CONFIGURATIONS)) $(CONFIGURATIONS)

clean: $(CONFIGURATIONS_CLEANTRGS)

$(CONFIGURATIONS):
	$(MAKE) -C $(CONFIGURATIONS_DIR)/$@

$(CONFIGURATIONS_CLEANTRGS):
	$(MAKE) -C $(CONFIGURATIONS_DIR)/$(@:clean_%=%) clean
