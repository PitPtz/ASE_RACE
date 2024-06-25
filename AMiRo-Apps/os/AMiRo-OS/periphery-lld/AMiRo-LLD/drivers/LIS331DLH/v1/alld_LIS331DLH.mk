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
AMIROLLD_LIS331DLH_V1_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# include paths
AMIROLLD_INC += $(AMIROLLD_LIS331DLH_V1_DIR)

# C source files
AMIROLLD_CSRC += $(AMIROLLD_LIS331DLH_V1_DIR)alld_LIS331DLH.c
