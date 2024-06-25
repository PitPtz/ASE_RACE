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
APPSCONFIG_MODULES_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# environment setup
APPSCONFIG_DIR := $(realpath $(APPSCONFIG_MODULES_DIR)/..)
include $(APPSCONFIG_MODULES_DIR)/../../apps.mk

# include paths
APPS_INC += $(APPSCONFIG_MODULES_DIR)

# C source files
APPS_CSRC += $(APPSCONFIG_MODULES_DIR)/AMiRoDefault_common.c

# C++ source files
APPS_CPPSRC +=
