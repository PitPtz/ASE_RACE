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



# path to this directory
AMIROOS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# set AMiRo-OS path variable
AMIROOS := $(AMIROOS_DIR:/=)

# load bootloader files
include $(AMIROOS)/bootloader/bootloader.mk

# load kernel files
include $(AMIROOS)/kernel/kernel.mk

# load peryphery LLD files
include $(AMIROOS)/periphery-lld/periphery-lld.mk

# load AMiRo-OS files
include $(AMIROOS)/modules/modules.mk
include $(AMIROOS)/core/core.mk
include $(AMIROOS)/test/test.mk

# inclusion directories
AMIROOS_INC = $(CONFDIR) \
              $(AMIROOS) \
              $(AMIROOS_BOOTLOADER_INC) \
              $(AMIROOS_KERNEL_INC) \
              $(AMIROOS_PERIPHERYLLD_INC) \
              $(AMIROOS_MODULES_INC) \
              $(AMIROOS_CORE_INC) \
              $(AMIROOS_TEST_INC) \
              $(MODULE_INC) \
              $(APPS_INC)

# C sources that can be compiled in ARM or THUMB mode depending on the global setting
AMIROOS_CSRC = $(AMIROOS_BOOTLOADER_CSRC) \
               $(AMIROOS_KERNEL_CSRC) \
               $(AMIROOS_PERIPHERYLLD_CSRC) \
               $(AMIROOS_MODULES_CSRC) \
               $(AMIROOS_CORE_CSRC) \
               $(AMIROOS_TEST_CSRC) \
               $(MODULE_CSRC) \
               $(APPS_CSRC)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global setting
AMIROOS_CPPSRC = $(AMIROOS_BOOTLOADER_CPPSRC) \
                 $(AMIROOS_KERNEL_CPPSRC) \
                 $(AMIROOS_PERIPHERYLLD_CPPSRC) \
                 $(AMIROOS_MODULES_CPPSRC) \
                 $(AMIROOS_CORE_CPPSRC) \
                 $(AMIROOS_TEST_CPPSRC) \
                 $(MODULE_CPPSRC) \
                 $(APPS_CPPSRC)

# ASM source files
AMIROOS_ASMSRC = $(AMIROOS_BOOTLOADER_ASMSRC) \
                 $(AMIROOS_KERNEL_ASMSRC) \
                 $(AMIROOS_PERIPHERYLLD_ASMSRC) \
                 $(AMIROOS_MODULES_ASMSRC) \
                 $(AMIROOS_CORE_ASMSRC) \
                 $(AMIROOS_TEST_ASMSRC) \
                 $(MODULE_ASMSRC) \
                 $(APPS_ASMSRC)

# ASM with preprocessor source files here
AMIROOS_ASMXSRC = $(AMIROOS_BOOTLOADER_ASMXSRC) \
                  $(AMIROOS_KERNEL_ASMXSRC) \
                  $(AMIROOS_PERIPHERYLLD_ASMXSRC) \
                  $(AMIROOS_MODULES_ASMXSRC) \
                  $(AMIROOS_CORE_ASMXSRC) \
                  $(AMIROOS_TEST_ASMXSRC) \
                  $(MODULE_ASMXSRC) \
                  $(APPS_ASMXSRC)

