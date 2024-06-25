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



# read OpenOCD version information (in any)
OPENOCD_INFO := $(shell openocd --version 2>&1)

# check whether OpenOCD could be executed at all
ifeq ($(OPENOCD_INFO),)
  $(warning Failed to execute OpenOCD)
else
  # extract version information
  OPENOCD_VERSION := $(strip $(shell echo "$(OPENOCD_INFO)" | grep -oE " [0-9]+\.[0-9]+\.[0-9]+"))
  OPENOCD_VERSION_MAJOR := $(strip $(subst ., ,$(shell echo "$(OPENOCD_VERSION)" | grep -oE "^[0-9]+\.")))
  OPENOCD_VERSION_MINOR := $(strip $(subst ., ,$(shell echo "$(OPENOCD_VERSION)" | grep -oE "\.[0-9]+\.")))
  OPENOCD_VERSION_PATCH := $(strip $(subst ., ,$(shell echo "$(OPENOCD_VERSION)" | grep -oE "\.[0-9]+$$")))
endif
