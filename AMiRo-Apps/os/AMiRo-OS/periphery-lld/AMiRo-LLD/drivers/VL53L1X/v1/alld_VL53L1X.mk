################################################################################
# AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous  #
# Mini Robot (AMiRo) platform.                                                 #
# Copyright (C) 2016..2022  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU Lesser General Public License as published by  #
# the Free Software Foundation, either version 3 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU Lesser General Public License for more details.                          #
#                                                                              #
# You should have received a copy of the GNU Lesser General Public License     #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.        #
#                                                                              #
# This research/work was supported by the Cluster of Excellence Cognitive      #
# Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is   #
# funded by the German Research Foundation (DFG).                              #
################################################################################



# path to this directory
AMIROLLD_ST_VL53L1X := $(dir $(lastword $(MAKEFILE_LIST)))

# include paths
AMIROLLD_INC += $(AMIROLLD_ST_VL53L1X) \
                $(AMIROLLD_ST_VL53L1X)/api/core \
                $(AMIROLLD_ST_VL53L1X)/api/platform

# C source files
AMIROLLD_CSRC += $(AMIROLLD_ST_VL53L1X)alld_VL53L1X.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api_calibration.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api_core.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api_debug.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api_preset_modes.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_api_strings.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_core.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_core_support.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_error_strings.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_register_funcs.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_silicon_core.c \
                 $(AMIROLLD_ST_VL53L1X)api/core/vl53l1_wait.c 
                 # $(AMIROLLD_ST_VL53L1X)api/platform/vl53l1_platform_log.c
