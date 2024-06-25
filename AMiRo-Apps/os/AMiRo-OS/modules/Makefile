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



# absolute path to this directory
MODULES_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# list of available modules
MODULES = $(patsubst $(MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(MODULES_DIR)*/*))))

CLEANTRGS = $(MODULES:%=clean_%)
FLASHTRGS = $(MODULES:%=flash_%)
INFOTRGS = $(MODULES:%=info_%)

# export all custom variables
unexport MODULES_DIR MODULES CLEANTRGS FLASHTRGS INFOTRGS
export

.PHONY: all $(MODULES) $(FLASHTRGS) clean $(CLEANTRGS) $(INFOTRGS)

all:
	$(MAKE) -j$(words $(MODULES)) $(MODULES)

$(MODULES):
	$(MAKE) -C $(MODULES_DIR)$@

$(FLASHTRGS):
	$(MAKE) -C $(MODULES_DIR)$(@:flash_%=%) flash

clean:
	$(MAKE) -j$(words $(MODULES)) $(CLEANTRGS)

$(CLEANTRGS):
	$(MAKE) -C $(MODULES_DIR)$(@:clean_%=%) clean

$(INFOTRGS):
	$(MAKE) -C $(MODULES_DIR)$(@:info_%=%) info
