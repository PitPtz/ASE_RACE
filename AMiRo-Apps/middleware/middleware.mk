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



# absolute path to this directory
MIDDLEWARE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# middleware setup
ifeq ($(CONFIG_MIDDLEWARE),µRT)
  $(info >>> MIDDLEWARE: µRT)

  include $(MIDDLEWARE_DIR)/uRT/urt.mk

  # include paths
  MIDDLEWARE_INC = $(URT_INC) \
                   $(MIDDLEWARE_DIR)

  # C source files
  MIDDLEWARE_CSRC = $(URT_CSRC) \
                    $(MIDDLEWARE_DIR)/apps_urtosal.c

  # C++ source files
  MIDDLEWARE_CPPSRC = $(URT_CPPSRC)

  # URT-OSAL implementation header file
  override UDEFS += -DURT_CFG_OSAL_HEADER=apps_urtosal.h

else ifeq ($(CONFIG_MIDDLEWARE),)
  $(info >>> MIDDLEWARE: none)
else
  $(error >>> MIDDLEWARE: unknown/unsupported middleware selected: $(CONFIG_MIDDLEWARE))
endif

