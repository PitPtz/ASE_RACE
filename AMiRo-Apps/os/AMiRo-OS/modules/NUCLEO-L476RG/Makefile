################################################################################
# AMiRo-OS is an operating system designed for the Autonomous Mini Robot       #
# (AMiRo) platform.                                                            #
# Copyright (C) 2016..2022  Thomas Schöpping et al.                            #
#                                                                              #
# This program is free software: you can redistribute it and/or modify         #
# it under the terms of the GNU (Lesser) General Public License as published   #
# the Free Software Foundation, either version 3 of the License, or            #
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
# NOTE: Can be overridden externally.                                          #
#                                                                              #

# Compiler options here.
USE_OPT ?= -O2 -fstack-usage -Wl,--print-memory-usage

# C specific options here (added to USE_OPT).
USE_COPT ?= -std=c99 -fshort-enums

# C++ specific options here (added to USE_OPT).
USE_CPPOPT ?= -fno-rtti -std=c++17

# Enable this if you want the linker to remove unused code and data.
USE_LINK_GC ?= yes

# Linker extra options here.
USE_LDOPT ?= -lm

# Enable this if you want link time optimizations (LTO).
USE_LTO ?= yes

# Enable this if you want to see the full log while compiling.
USE_VERBOSE_COMPILE ?= no

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
USE_SMART_BUILD ?= no

#                                                                              #
# Build global options                                                         #
################################################################################

################################################################################
# Architecture or project specific options                                     #
#                                                                              #

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
USE_PROCESS_STACKSIZE ?= 0x400

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
USE_EXCEPTIONS_STACKSIZE ?= 0x400

# Enables the use of FPU.
# Possible selections are:
#   no     - no FPU is used (probably equals 'soft')
#   soft   - does not use the FPU, thus all floating point operations are emulated
#   softfp - uses the FPU, but uses the integer registers only
#   hard   - uses the FPU and passes data via the FPU registers
USE_FPU ?= softfp

# FPU-related options.
USE_FPU_OPT ?= -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16

#                                                                              #
# Architecture or project specific options                                     #
################################################################################

################################################################################
# Project, target, sources and paths                                           #
#                                                                              #

# Absolute path to the project.
PROJECT_PATH := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

# Project name.
PROJECT := $(notdir $(PROJECT_PATH))

# Target settings.
MCU := cortex-m4

# Module specific paths and directories.
include ../defaults.mk

# Linker script.
LDSCRIPT := $(MODULE_DIR)/STM32L476xG.ld

# General AMiRo-OS files.
include ../../amiro-os.mk

# Module specific ChibiOS files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32l4xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32L4xx/platform.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk

# Module specific setup configuration.
# set one of the following values
# - BOARD_MPU6050_CONNECTED
#   Build with support for the MPU6050 sensor.
# - BOARD_VL6180X_CONNECTED
#   Build with support for the VL6180X sensor.
# - BOARD_VL53L1X_CONNECTED
#   Build with support for the VL53L1X sensor.
ifneq ($(origin BOARD_MPU6050_CONNECTED), undefined)
  override UDEFS += -DBOARD_MPU6050_CONNECTED
endif
ifneq ($(origin BOARD_VL6180X_CONNECTED), undefined)
  override UDEFS += -DBOARD_VL6180X_CONNECTED
endif
ifneq ($(origin BOARD_VL53L1X_CONNECTED), undefined)
  override UDEFS += -DBOARD_VL53L1X_CONNECTED
endif

# Module specific periphery LLDs.
include $(AMIROLLD)/drivers/LED/v1/alld_LED.mk
include $(AMIROLLD)/drivers/button/v1/alld_button.mk
ifneq ($(origin BOARD_VL53L1X_CONNECTED), undefined)
  include $(AMIROLLD)/drivers/VL53L1X/v1/alld_VL53L1X.mk
endif
ifneq ($(origin BOARD_MPU6050_CONNECTED), undefined)
  include $(AMIROLLD)/drivers/MPU6050/v1/alld_MPU6050.mk
endif
ifneq ($(origin BOARD_VL6180X_CONNECTED), undefined)
  include $(AMIROLLD)/drivers/VL6180X/v1/alld_VL6180X.mk
endif


# Module specific inclusion directories.
MODULE_INC = $(MODULE_DIR)

# Module specific C source files.
MODULE_CSRC = $(MODULE_DIR)/board.c \
              $(MODULE_DIR)/module.c

# Module specific tests.
include $(MODULE_DIR)/test/button/module_test_button.mk
include $(MODULE_DIR)/test/LED/module_test_LED.mk
ifneq ($(origin BOARD_VL53L1X_CONNECTED), undefined)
  include $(MODULE_DIR)/test/VL53L1X/module_test_VL53L1X.mk
endif
ifneq ($(origin BOARD_MPU6050_CONNECTED), undefined)
  include $(MODULE_DIR)/test/MPU6050/module_test_MPU6050.mk
endif
ifneq ($(origin BOARD_VL6180X_CONNECTED), undefined)
  include $(MODULE_DIR)/test/VL6180X/module_test_VL6180X.mk
endif


# C warning options.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# C++ warning options.
CPPWARN = -Wall -Wextra -Wundef

#                                                                              #
# Project, target, sources and paths                                           #
################################################################################

################################################################################
# Start of user section                                                        #
#                                                                              #

# List all user C define here, like -D_DEBUG=1
UDEFS +=

# Define ASM defines here
UADEFS +=

# List all user directories here
UINCDIR +=

# List the user directory to look for the libraries here
ULIBDIR +=

# List all user libraries here
ULIBS +=

#                                                                              #
# End of user section                                                          #
################################################################################

################################################################################
# Common rules                                                                 #
#                                                                              #

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#                                                                              #
# Common rules                                                                 #
################################################################################

################################################################################
# Custom rules                                                                 #
#                                                                              #

# retrieve OpenOCD version information
ifneq ($(filter flash,$(MAKECMDGOALS)),)
  include ../openocd.mk
  ifeq ($(OPENOCD_VERSION_MAJOR),0)
    OPENOCD_INTERFACE_CFG := $(if $(shell [ $(OPENOCD_VERSION_MINOR) -ge 11 ] && echo true),stlink.cfg,stlink-v2-1.cfg)
  else
    $(error Unsupported version of OpenOCD detected: $(OPENOCD_VERSION))
  endif
endif

.PHONY: all clean flash info

flash: $(BUILDDIR)/$(PROJECT).elf
	openocd -f interface/$(OPENOCD_INTERFACE_CFG) -f target/stm32l4x.cfg -c "program $(BUILDDIR)/$(PROJECT).elf verify reset exit"

define INFO_TEXT

NAME:       NUCLEO-L476RG
PLATFORM:   NUCLEO
MODULE:     STM32L476RG

OPTIONS:
    BUILDDIR=<PATH>
        Specifies the directory, where generated code will be written to.
        Default path: $(BUILDDIR_DEFAULT)
    BOARD_MPU6050_CONNECTED
        Specify this option if a MPU6050 development board is connected to the
        module.
    BOARD_VL6180X_CONNECTED
        Specify this option if a VL6180X development board is connected to the
        module.
    BOARD_VL53L1X_CONNECTED
        Specify this option if a VL53L1X development board is connected to the
        module.

endef

info:
	$(info $(INFO_TEXT))
	@exit

#                                                                              #
# Custom rules                                                                 #
################################################################################
