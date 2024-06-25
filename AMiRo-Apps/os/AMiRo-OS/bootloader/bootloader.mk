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



# path to this makefile
AMIROOS_BOOTLOADER_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# set AMiRo-BLT path variable
AMIROBLT := $(AMIROOS_BOOTLOADER_DIR)AMiRo-BLT

# include paths
AMIROOS_BOOTLOADER_INC = $(AMIROBLT)/Target/Source/AMiRo

# load module ID constants
include $(AMIROBLT)/Target/Modules/moduleids.mk

# set default flash port
ifneq ($(word 1,$(filter /dev/ttyAMiRo%,$(sort $(wildcard /dev/*)))),)
  # should be /dev/ttyAMiRo0 in most cases
  FLASH_PORT_DEFAULT := $(word 1,$(filter /dev/ttyAMiRo%,$(sort $(wildcard /dev/*))))
else ifneq ($(word 1,$(filter /dev/ttyUSB%,$(sort $(wildcard /dev/*)))),)
  # may be /dev/ttyUSB* for legacy support
  FLASH_PORT_DEFAULT := $(word 1,$(filter /dev/ttyUSB%,$(sort $(wildcard /dev/*))))
else
  # provide value for error messages
  FLASH_PORT_DEFAULT := n/a
endif
# set the FLASH_PORT if no custom value has been specified
FLASH_PORT ?= $(FLASH_PORT_DEFAULT)

# set the default baudrate
FLASH_BAUDRATE_DEFAULT := 115200
# set the baud rate if no custom value has been specified
FLASH_BAUDRATE ?= $(FLASH_BAUDRATE_DEFAULT)

# set the flashing tool, the command to call it, arguments, and file extensions
ifneq ($(wildcard $(AMIROBLT)/Host/Source/SerialBoot/build/SerialBoot),)
  # if AMiRo-BLT project is available, use SerialBoot
  FLASHTOOL := SerialBoot
  FLASHTOOL_CMD := $(AMIROBLT)/Host/Source/SerialBoot/build/SerialBoot
  FLASHTOOL_EXT := srec
  FLASHTOOL_ARGS = -d$(FLASH_PORT) -b$(FLASH_BAUDRATE) $(foreach module,$(FLASH_MODULES),-T$($(module)_ID_HEX) $(BUILDDIR)/$(module).$(FLASHTOOL_EXT))
else
  # the default tool to use is stm32flash
  ifneq ($(words $(FLASH_MODULES)),1)
    FLASH_MODULES := $(firstword $(FLASH_MODULES))
  endif
  FLASHTOOL := stm32flash
  FLASHTOOL_CMD := stm32flash
  FLASHTOOL_EXT := hex
  FLASHTOOL_ARGS = $(FLASH_PORT) -i "rts,-dtr,-rts":"rts,dtr,-rts" -R -b $(FLASH_BAUDRATE) -w $(BUILDDIR)/$(FLASH_MODULES).$(FLASHTOOL_EXT)
endif

