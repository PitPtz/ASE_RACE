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
AMiRo_ProximitySensor_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# middleware selection
ifeq ($(CONFIG_MIDDLEWARE),)
  CONFIG_MIDDLEWARE := µRT
else ifneq ($(CONFIG_MIDDLEWARE),µRT)
  $(error incompatible middleware selected: $(CONFIG_MIDDLEWARE))
endif

# include path
AMiRo_ProximitySensor_INC = $(AMiRo_ProximitySensor_DIR)

# C source files
AMiRo_ProximitySensor_CSRC = $(AMiRo_ProximitySensor_DIR)/amiro_proximitysensor.c 

# C++ source files
AMiRo_ProximitySensor_CPPSRC =
