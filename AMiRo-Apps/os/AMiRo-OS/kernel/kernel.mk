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



# set kernel path variable
AMIROOS_KERNEL_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# set ChibiOS path variable
CHIBIOS := $(AMIROOS_KERNEL_DIR)ChibiOS

# load licensing files
include $(CHIBIOS)/os/license/license.mk

# load startup files
## must be loaded by each module individually

# load HAL-OSAL files
include $(CHIBIOS)/os/hal/hal.mk
## platform.mk must be loaded by each module individually
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
include $(CHIBIOS)/os/hal/lib/streams/streams.mk

# load RTOS files
include $(CHIBIOS)/os/rt/rt.mk
## compiler port must be loaded by each module individually

## load auto-build files in ./source recursively.
#include $(CHIBIOS)/tools/mk/autobuild.mk

# load test files
include $(CHIBIOS)/os/test/test.mk
include $(CHIBIOS)/test/rt/rt_test.mk
include $(CHIBIOS)/test/oslib/oslib_test.mk

# include paths
AMIROOS_KERNEL_INC = $(ALLINC)  \
                     $(TESTINC) \
                     $(CHIBIOS)/os/hal/lib/streams

# C source files
AMIROOS_KERNEL_CSRC = $(ALLCSRC) \
                      $(CHIBIOS)/os/various/syscalls.c \
                      $(CHIBIOS)/os/various/evtimer.c \
                      $(TESTSRC)

# C++ source files
AMIROOS_KERNEL_CPPSRC = $(ALLCPPSRC)

# ASM source files
AMIROOS_KERNEL_ASMSRC = $(ALLASMSRC)

# ASM with preprocessor source files
AMIROOS_KERNEL_ASMXSRC = $(ALLXASMSRC)

### Information forwarded to ChibiOS ###
# Inclusion directories.
INCDIR = $(AMIROOS_INC)

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(AMIROOS_CSRC)

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(AMIROOS_CPPSRC)

# List ASM source files here.
ASMSRC = $(AMIROOS_ASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(AMIROOS_ASMXSRC)

