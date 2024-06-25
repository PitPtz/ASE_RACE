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
HelloWorldCONF_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available/supported modules
HelloWorldCONF_MODULES_DIR = $(HelloWorldCONF_DIR)/modules/
HelloWorldCONF_MODULES = $(patsubst $(HelloWorldCONF_MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(HelloWorldCONF_MODULES_DIR)*/*))))

HelloWorldCONF_CLEANTRGS = $(HelloWorldCONF_MODULES:%=clean_%)
HelloWorldCONF_FLASHTRGS = $(HelloWorldCONF_MODULES:%=flash_%)
HelloWorldCONF_INFOTRGS = $(HelloWorldCONF_MODULES:%=info_%)

# export all custom variables
unexport HelloWorldCONF_DIR HelloWorldCONF_MODULES_DIR HelloWorldCONF_MODULES HelloWorldCONF_CLEANTRGS HelloWorldCONF_FLASHTRGS HelloWorldCONF_INFOTRGS
export

.PHONY: all clean $(HelloWorldCONF_MODULES) $(HelloWorldCONF_FLASHTRGS) $(HelloWorldCONF_CLEANTRGS) $(HelloWorldCONF_INFOTRGS)

all:
	$(MAKE) -j$(words $(HelloWorldCONF_MODULES)) $(HelloWorldCONF_MODULES)

clean: $(HelloWorldCONF_CLEANTRGS)

$(HelloWorldCONF_MODULES):
	$(MAKE) -C $(HelloWorldCONF_MODULES_DIR)/$@

$(HelloWorldCONF_FLASHTRGS):
	$(MAKE) -C $(HelloWorldCONF_MODULES_DIR)/$(@:flash_%=%) flash

$(HelloWorldCONF_CLEANTRGS):
	$(MAKE) -C $(HelloWorldCONF_MODULES_DIR)/$(@:clean_%=%) clean

$(HelloWorldCONF_INFOTRGS):
	$(MAKE) -C $(HelloWorldCONF_MODULES_DIR)/$(@:info_%=%) info
