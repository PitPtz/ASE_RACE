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
uRTBenchmarkCONF_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available/supported modules
uRTBenchmarkCONF_MODULES_DIR = $(uRTBenchmarkCONF_DIR)/modules/
uRTBenchmarkCONF_MODULES = $(patsubst $(uRTBenchmarkCONF_MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(uRTBenchmarkCONF_MODULES_DIR)*/*))))

uRTBenchmarkCONF_CLEANTRGS = $(uRTBenchmarkCONF_MODULES:%=clean_%)
uRTBenchmarkCONF_FLASHTRGS = $(uRTBenchmarkCONF_MODULES:%=flash_%)
uRTBenchmarkCONF_INFOTRGS = $(uRTBenchmarkCONF_MODULES:%=info_%)

# export all custom variables
unexport uRTBenchmarkCONF_DIR uRTBenchmarkCONF_MODULES_DIR uRTBenchmarkCONF_MODULES uRTBenchmarkCONF_CLEANTRGS uRTBenchmarkCONF_FLASHTRGS uRTBenchmarkCONF_INFOTRGS
export

.PHONY: all clean $(uRTBenchmarkCONF_MODULES) $(uRTBenchmarkCONF_FLASHTRGS) $(uRTBenchmarkCONF_CLEANTRGS) $(uRTBenchmarkCONF_INFOTRGS)

all:
	$(MAKE) -j$(words $(uRTBenchmarkCONF_MODULES)) $(uRTBenchmarkCONF_MODULES)

clean: $(uRTBenchmarkCONF_CLEANTRGS)

$(uRTBenchmarkCONF_MODULES):
	$(MAKE) -C $(uRTBenchmarkCONF_MODULES_DIR)/$@

$(uRTBenchmarkCONF_FLASHTRGS):
	$(MAKE) -C $(uRTBenchmarkCONF_MODULES_DIR)/$(@:flash_%=%) flash

$(uRTBenchmarkCONF_CLEANTRGS):
	$(MAKE) -C $(uRTBenchmarkCONF_MODULES_DIR)/$(@:clean_%=%) clean

$(uRTBenchmarkCONF_INFOTRGS):
	$(MAKE) -C $(uRTBenchmarkCONF_MODULES_DIR)/$(@:info_%=%) info
