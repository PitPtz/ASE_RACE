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

# include apps
include $(APPS_DIR)/AMiRo_DifferentialMotionEstimator/AMiRo_DifferentialMotionEstimator.mk
include $(APPS_DIR)/AMiRo_DifferentialMotorControl/AMiRo_DifferentialMotorControl.mk
include $(APPS_DIR)/AMiRo_Floor/AMiRo_Floor.mk
include $(APPS_DIR)/CANBridge/CANBridge.mk
include $(APPS_DIR)/AMiRo_BNO055/AMiRo_BNO055.mk
include $(APPS_DIR)/LightFloorData/LightFloorData.mk
# include $(APPS_DIR)/LineFollowing/LineFollowing.mk
include $(APPS_DIR)/ObstacleAvoidance/ObstacleAvoidance.mk
include $(APPS_DIR)/Odometry/Odometry.mk
include $(APPS_DIR)/Intro_LineFollowing/Intro_LineFollowing.mk
include $(APPS_DIR)/Intro_WallFollowing/Intro_WallFollowing.mk

# middleware setup
include $(MIDDLEWARE_DIR)/middleware.mk

# C sources
APPS_CSRC += $(MIDDLEWARE_CSRC) \
             $(AMiRo_DifferentialMotionEstimator_CSRC) \
             $(AMiRo_DifferentialMotorControl_CSRC) \
             $(AMiRo_Floor_CSRC) \
             $(CANBridge_CSRC) \
             $(Intro_WallFollowing_CSRC) \
             $(AMiRo_BNO055_CSRC) \
             $(LightFloorData_CSRC) \
             $(ObstacleAvoidance_CSRC) \
             $(Intro_LineFollowing_CSRC) \
             $(Odometry_CSRC) \
             $(realpath apps.c) 
 
# C++ sources
APPS_CPPSRC += $(MIDDLEWARE_CPPSRC) \
               $(AMiRo_DifferentialMotionEstimator_CPPSRC) \
               $(AMiRo_DifferentialMotorControl_CPPSRC) \
               $(AMiRo_Floor_CPPSRC) \
               $(AMiRo_BNO055_CPPSRC) \
               $(LightFloorData_CPPSRC) \
               $(ObstacleAvoidance_CPPSRC) \
               $(CANBridge_CPPSRC) \
               $(Odometry_CPPSRC) \
               $(Intro_LineFollowing_CPPSRC) \
               $(Intro_WallFollowing_CPPSRC)

# include directories for configurations
APPS_INC += $(realpath .) \
            $(MIDDLEWARE_INC) \
            $(AMiRo_DifferentialMotionEstimator_INC) \
            $(AMiRo_DifferentialMotorControl_INC) \
            $(AMiRo_Floor_INC) \
            $(AMiRo_BNO055_INC) \
            $(LightFloorData_INC) \
            $(ObstacleAvoidance_INC) \
            $(Intro_LineFollowing_INC) \
            $(Intro_WallFollowing_INC) \
            $(CANBridge_INC) \
            $(Odometry_INC) 
            #$(LineFollowing_INC) 

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
OS_MODULE_PATH := $(OS_DIR)/AMiRo-OS/modules/DiWheelDrive_1-2/
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
