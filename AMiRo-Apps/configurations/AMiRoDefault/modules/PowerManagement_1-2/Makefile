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



################################################################################
# Build global options                                                         #
#                                                                              #

# use defaults

#                                                                              #
# Build global options                                                         #
################################################################################

################################################################################
# Sources and paths                                                            #
#                                                                              #

# environment setup
include ../modules.mk
BOARD_SENSORRING_DEFAULT ?= BOARD_PROXIMITYSENSOR
BOARD_SENSORRING ?= $(BOARD_SENSORRING_DEFAULT)

# include apps
include $(APPS_DIR)/AMiRo_Charger/AMiRo_charger.mk
ifeq ($(BOARD_SENSORRING),BOARD_PROXIMITYSENSOR)
  include $(APPS_DIR)/AMiRo_ProximitySensor/AMiRo_ProximitySensor.mk
endif
include $(APPS_DIR)/CANBridge/CANBridge.mk

# middleware setup
include $(MIDDLEWARE_DIR)/middleware.mk

# C sources
APPS_CSRC += $(MIDDLEWARE_CSRC) \
             $(AMiRo_Charger_CSRC) \
             $(CANBridge_CSRC) \
             $(realpath apps.c)
ifeq ($(BOARD_SENSORRING),BOARD_PROXIMITYSENSOR)
  APPS_CSRC += $(AMiRo_ProximitySensor_CSRC)
endif

# C++ sources
APPS_CPPSRC += $(MIDDLEWARE_CPPSRC) \
               $(AMiRo_Charger_CPPSRC) \
               $(CANBridge_CPPSRC) 
ifeq ($(BOARD_SENSORRING),BOARD_PROXIMITYSENSOR)
  APPS_CPPSRC += $(AMiRo_ProximitySensor_CPPSRC)
endif

# include directories for configurations
APPS_INC += $(realpath .) \
            $(MIDDLEWARE_INC) \
            $(AMiRo_Charger_INC) \
            $(CANBridge_INC)
ifeq ($(BOARD_SENSORRING),BOARD_PROXIMITYSENSOR)
  APPS_INC += $(AMiRo_ProximitySensor_INC)
endif

#                                                                              #
# Sources and paths                                                            #
################################################################################

################################################################################
# Start of user section                                                        #
#                                                                              #

# List all user defines here
UDEFS +=

# List all ASM defines here
UADEFS +=

# List all user directories here
UINCDIR +=

# List all directories to look for user libraries here
ULIBDIR +=

# List all user libraries here
ULIBS +=

#                                                                              #
# End of user defines                                                          #
################################################################################

################################################################################
# Start of targets section                                                     #
#                                                                              #

# set the build directory
BUILDDIR_DEFAULT ?= $(realpath .)/build
BUILDDIR ?= $(BUILDDIR_DEFAULT)

# export all (custom) variables
export

# call Makefile from OS
OS_MODULE_PATH := $(OS_DIR)/AMiRo-OS/modules/PowerManagement_1-2/
.PHONY: all clean flash info

all:
	$(MAKE) -C $(OS_MODULE_PATH)

clean:
	$(MAKE) -C $(OS_MODULE_PATH) clean

flash:
	$(MAKE) -C $(OS_MODULE_PATH) flash

info:
	$(MAKE) -C $(OS_MODULE_PATH) info

#                                                                              #
# End of targets section                                                       #
################################################################################
