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
L515TriggerCONF_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available/supported modules
L515TriggerCONF_MODULES_DIR = $(L515TriggerCONF_DIR)/modules/
L515TriggerCONF_MODULES = $(patsubst $(L515TriggerCONF_MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(L515TriggerCONF_MODULES_DIR)*/*))))

L515TriggerCONF_CLEANTRGS = $(L515TriggerCONF_MODULES:%=clean_%)
L515TriggerCONF_FLASHTRGS = $(L515TriggerCONF_MODULES:%=flash_%)
L515TriggerCONF_INFOTRGS = $(L515TriggerCONF_MODULES:%=info_%)

# export all custom variables
unexport L515TriggerCONF_DIR L515TriggerCONF_MODULES_DIR L515TriggerCONF_MODULES L515TriggerCONF_CLEANTRGS L515TriggerCONF_FLASHTRGS L515TriggerCONF_INFOTRGS
export

.PHONY: all clean $(L515TriggerCONF_MODULES) $(L515TriggerCONF_FLASHTRGS) $(L515TriggerCONF_CLEANTRGS) $(L515TriggerCONF_INFOTRGS)

all:
	$(MAKE) -j$(words $(L515TriggerCONF_MODULES)) $(L515TriggerCONF_MODULES)

clean: $(L515TriggerCONF_CLEANTRGS)

$(L515TriggerCONF_MODULES):
	$(MAKE) -C $(L515TriggerCONF_MODULES_DIR)/$@

$(L515TriggerCONF_FLASHTRGS):
	$(MAKE) -C $(L515TriggerCONF_MODULES_DIR)/$(@:flash_%=%) flash

$(L515TriggerCONF_CLEANTRGS):
	$(MAKE) -C $(L515TriggerCONF_MODULES_DIR)/$(@:clean_%=%) clean

$(L515TriggerCONF_INFOTRGS):
	$(MAKE) -C $(L515TriggerCONF_MODULES_DIR)/$(@:info_%=%) info
