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

# Enable this if you want the linker to remove unused code and data.
USE_LINK_GC := no

# Enable this if you want link time optimizations (LTO).
USE_LTO := no

# use defaults fo reverything else

#                                                                              #
# Build global options                                                         #
################################################################################

################################################################################
# Sources and paths                                                            #
#                                                                              #

# environment setup
include ../../../apps.mk

# include apps
#n/a

# middleware setup
ifndef TESTMODULE_OSONLY
  $(error TESTMODULE_OSONLY no defined)
else
  ifeq ($(TESTMODULE_OSONLY),no)
    CONFIG_MIDDLEWARE := µRT
    include $(MIDDLEWARE_DIR)/middleware.mk
  else
    ifneq ($(TESTMODULE_OSONLY),yes)
      $(error TESTMODULE_OSONLY must be either 'yes' or 'no')
    endif
  endif
endif

# C sources
APPS_CSRC += $(MIDDLEWARE_CSRC)

# C++ sources
APPS_CPPSRC += $(MIDDLEWARE_CPPSRC)

# include directories for configurations
APPS_INC += $(realpath .) \
            $(MIDDLEWARE_INC)

#                                                                              #
# Sources and paths                                                            #
################################################################################

################################################################################
# Start of user section                                                        #
#                                                                              #

# List all user defines here
ifeq ($(TESTMODULE_OSONLY),yes)
  override UDEFS += -DTESTMODULE_OSONLY
endif

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
ifndef TESTMODULE
  $(error TESTMODULE not defined)
endif
OS_MODULE_PATH := $(OS_DIR)/AMiRo-OS/modules/$(TESTMODULE)
ifeq ($(wildcard $(OS_MODULE_PATH)),)
  $(error $(TESTMODULE) not supported by the OS)
endif
.PHONY: all clean flash info

all:
	$(MAKE) -C $(OS_MODULE_PATH) UDEFS="$(UDEFS)"

clean:
	$(MAKE) -C $(OS_MODULE_PATH) UDEFS="$(UDEFS)" clean

info:
	$(MAKE) -C $(OS_MODULE_PATH) UDEFS="$(UDEFS)" info

#                                                                              #
# End of targets section                                                       #
################################################################################
