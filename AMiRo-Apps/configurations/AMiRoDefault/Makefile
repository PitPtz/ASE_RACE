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
AMiRoDefaultCONF_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available/supported modules
AMiRoDefaultCONF_MODULES_DIR = $(AMiRoDefaultCONF_DIR)/modules/
AMiRoDefaultCONF_MODULES = $(patsubst $(AMiRoDefaultCONF_MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(AMiRoDefaultCONF_MODULES_DIR)*/*))))

AMiRoDefaultCONF_CLEANTRGS = $(AMiRoDefaultCONF_MODULES:%=clean_%)
AMiRoDefaultCONF_FLASHTRGS = $(AMiRoDefaultCONF_MODULES:%=flash_%)
AMiRoDefaultCONF_INFOTRGS = $(AMiRoDefaultCONF_MODULES:%=info_%)

# export all custom variables
unexport AMiRoDefaultCONF_DIR AMiRoDefaultCONF_MODULES_DIR AMiRoDefaultCONF_MODULES AMiRoDefaultCONF_CLEANTRGS AMiRoDefaultCONF_FLASHTRGS AMiRoDefaultCONF_INFOTRGS
export

.PHONY: all clean $(AMiRoDefaultCONF_MODULES) $(AMiRoDefaultCONF_FLASHTRGS) $(AMiRoDefaultCONF_CLEANTRGS) $(AMiRoDefaultCONF_INFOTRGS)

.NOTPARALLEL: $(AMiRoDefaultCONF_FLASHTRGS)

all:
	$(MAKE) -j$(words $(AMiRoDefaultCONF_MODULES)) $(AMiRoDefaultCONF_MODULES)

clean: $(AMiRoDefaultCONF_CLEANTRGS)

$(AMiRoDefaultCONF_MODULES):
	$(MAKE) -C $(AMiRoDefaultCONF_MODULES_DIR)/$@

$(AMiRoDefaultCONF_FLASHTRGS):
	$(MAKE) -C $(AMiRoDefaultCONF_MODULES_DIR)/$(@:flash_%=%) flash

$(AMiRoDefaultCONF_CLEANTRGS):
	$(MAKE) -C $(AMiRoDefaultCONF_MODULES_DIR)/$(@:clean_%=%) clean

$(AMiRoDefaultCONF_INFOTRGS):
	$(MAKE) -C $(AMiRoDefaultCONF_MODULES_DIR)/$(@:info_%=%) info
