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
LDSCRIPT := $(MODULE_DIR)/STM32F405xG.ld

# General AMiRo-OS files.
include ../../amiro-os.mk

# Module specific ChibiOS files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk

# Module specific setup configuration.
# set one of the following values
# - BOARD_BREAKOUT_NONE
#   Build without breakout module support
# - BOARD_BREAKOUT_ESP
#   Build with support for the ESP breakout module
BOARD_BREAKOUT_MODULE_DEFAULT ?= BOARD_BREAKOUT_NONE
BOARD_BREAKOUT_MODULE ?= $(BOARD_BREAKOUT_MODULE_DEFAULT)
ifneq ($(BOARD_BREAKOUT_MODULE),)
  override UDEFS += -DBOARD_BREAKOUT_MODULE=$(BOARD_BREAKOUT_MODULE)
endif

ifeq ($(BOARD_BREAKOUT_MODULE),)
  # error
  $(error BOARD_BREAKOUT_MODULE not set)
else ifeq ($(BOARD_BREAKOUT_MODULE), BOARD_BREAKOUT_NONE)
  # build no additional drivers
else ifeq ($(BOARD_BREAKOUT_MODULE), BOARD_BREAKOUT_ESP)
  # build no additional drivers
else
  # error
  $(error BOARD_BREAKOUT_MODULE set to invalid value)
endif

# Module specific setup configuration.
# set one of the following values
# - BOARD_NOSENSORRING
#   Build without suuport for any sensor ring.
# - BOARD_PROXIMITYSENSOR
#   Build supporting the traditional ProximitySensor.
# - BOARD_DISTANCESENSOR_VL53L0X
#   Build supporting the DistanceSensor populated with VL53L0X sensors.
# - BOARD_DISTANCESENSOR_VL53L1X
#   Build supporting the DistanceSensor populated with VL53L1X sensors.
BOARD_SENSORRING_DEFAULT ?= BOARD_PROXIMITYSENSOR
BOARD_SENSORRING ?= $(BOARD_SENSORRING_DEFAULT)
ifneq ($(BOARD_SENSORRING),)
  override UDEFS += -DBOARD_SENSORRING=$(BOARD_SENSORRING)
endif

# Module specific periphery LLDs.
include $(AMIROLLD)/drivers/AT24C01B/v1/alld_AT24C01B.mk
include $(AMIROLLD)/drivers/bq241xx/v1/alld_bq241xx.mk
include $(AMIROLLD)/drivers/bq27500/v1/alld_bq27500.mk
include $(AMIROLLD)/drivers/INA219/v1/alld_INA219.mk
include $(AMIROLLD)/drivers/LED/v1/alld_LED.mk
include $(AMIROLLD)/drivers/PCA9544A/v1/alld_PCA9544A.mk
include $(AMIROLLD)/drivers/PKxxxExxx/v1/alld_PKxxxExxx.mk
include $(AMIROLLD)/drivers/switch/v1/alld_switch.mk
include $(AMIROLLD)/drivers/TPS6211x/v1/alld_TPS6211x.mk
ifeq ($(BOARD_SENSORRING),)
  # error
  $(error BOARD_SENSORRING not set)
else ifeq ($(BOARD_SENSORRING), BOARD_NOSENSORRING)
  # build no additional drivers
else ifeq ($(BOARD_SENSORRING), BOARD_PROXIMITYSENSOR)
  # build all drivers for the ProximitySensor
  include $(AMIROLLD)/drivers/MPR121/v1/alld_MPR121.mk
  include $(AMIROLLD)/drivers/VCNL4020/v1/alld_VCNL4020.mk
else ifeq ($(BOARD_SENSORRING), BOARD_DISTANCESENSOR_VL53L0X)
  # build all drivers for the DistanceSensor (VL53L0X)
  include $(AMIROLLD)/drivers/AT42QT1050/v1/alld_AT42QT1050.mk
  include $(AMIROLLD)/drivers/PCAL6524/v1/alld_PCAL6524.mk
else ifeq ($(BOARD_SENSORRING), BOARD_DISTANCESENSOR_VL53L1X)
  # build all drivers for the DistanceSensor (VL53L1X)
  include $(AMIROLLD)/drivers/AT42QT1050/v1/alld_AT42QT1050.mk
  include $(AMIROLLD)/drivers/PCAL6524/v1/alld_PCAL6524.mk
else
  # error
  $(error BOARD_SENSORRING set to invalid value)
endif

# Module specific inclusion directories.
MODULE_INC = $(MODULE_DIR)

# Module specific C source files.
MODULE_CSRC = $(MODULE_DIR)/board.c \
              $(MODULE_DIR)/module.c

# Module specific tests.
include $(MODULE_DIR)/test/adc/module_test_adc.mk
include $(MODULE_DIR)/test/AT24C01B/module_test_AT24C01B.mk
include $(MODULE_DIR)/test/bq241xx/module_test_bq241xx.mk
include $(MODULE_DIR)/test/bq27500/module_test_bq27500.mk
include $(MODULE_DIR)/test/bq27500_bq241xx/module_test_bq27500_bq241xx.mk
include $(MODULE_DIR)/test/INA219/module_test_INA219.mk
include $(MODULE_DIR)/test/LED/module_test_LED.mk
include $(MODULE_DIR)/test/PKxxxExxx/module_test_PKxxxExxx.mk
include $(MODULE_DIR)/test/switch/module_test_switch.mk
include $(MODULE_DIR)/test/TPS6211x/module_test_TPS6211x.mk
include $(MODULE_DIR)/test/TPS6211x_INA219/module_test_TPS6211x_INA219.mk
ifeq ($(BOARD_SENSORRING),)
  # error
  $(error BOARD_SENSORRING not set)
else ifeq ($(BOARD_SENSORRING), BOARD_NOSENSORRING)
  # build no additional tests
else ifeq ($(BOARD_SENSORRING), BOARD_PROXIMITYSENSOR)
  # build all tests for the ProximitySensor
  include $(MODULE_DIR)/test/MPR121/module_test_MPR121.mk
  include $(MODULE_DIR)/test/PCA9544A/module_test_PCA9544A.mk
  include $(MODULE_DIR)/test/VCNL4020/module_test_VCNL4020.mk
else ifeq ($(BOARD_SENSORRING), BOARD_DISTANCESENSOR_VL53L0X)
  # build all tests for the DistanceSensor (VL53L0X)
  include $(MODULE_DIR)/test/AT42QT1050/module_test_AT42QT1050.mk
  include $(MODULE_DIR)/test/PCAL6524/module_test_PCAL6524.mk
else ifeq ($(BOARD_SENSORRING), BOARD_DISTANCESENSOR_VL53L1X)
  # build all tests for the DistanceSensor (VL53L1X)
  include $(MODULE_DIR)/test/AT42QT1050/module_test_AT42QT1050.mk
  include $(MODULE_DIR)/test/PCAL6524/module_test_PCAL6524.mk
else
  # error
  $(error BOARD_SENSORRING set to invalid value)
endif

# C warning options.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# C++ warning options.
CPPWARN = -Wall -Wextra -Wundef

# Create an additional .srec image file.
SREC = $(CP) -O srec --srec-len=248

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

FLASH_MODULES = $(PROJECT)
FLASH_FILES = $(BUILDDIR)/$(PROJECT).$(FLASHTOOL_EXT)

.PHONY: all clean flash info

flash: $(FLASH_FILES)
	$(info )
ifeq ($(FLASHTOOL),SerialBoot)
	$(info Flashing ($(FLASHTOOL)):)
	$(FLASHTOOL_CMD) $(FLASHTOOL_ARGS)
else
	$(info ERROR: unable to flash the module (SerialBoot unavailable))
endif

define INFO_TEXT

NAME:       PowerManagement v1.2
PLATFORM:   AMiRo
MODULE:     PowerManagement
VERSION:    1.2

OPTIONS:
    BUILDDIR=<PATH>
        Specifies the directory, where generated code will be written to.
        Default path: $(BUILDDIR_DEFAULT)
    FLASH_PORT=<PORT>
        Specifies the serial port to be used for flashing the module.
        Default port: $(FLASH_PORT_DEFAULT)
    FLASH_BAUDRATE=<BAUDRATE>
        Specifies the baudrate to be used when flashing module.
        Defaults baudrate: $(FLASH_BAUDRATE_DEFAULT)
    BOARD_SENSORRING=<SENSORRING>
        Specify whether and which sensor ring  is attached to the module
        (default: $(BOARD_SENSORRING_DEFAULT)).
        Available settings are:
        - BOARD_NOSENSORRING
            No sensor ring attached.
        - BOARD_PROXIMITYSENSOR
            ProximitySensor (VCNL4020 + MPR121)
        - BOARD_DISTANCESENSOR_VL53L0X
            DistanceSensor prototype (VL53L0X + AT42QT1050)
        - BOARD_DISTANCESENSOR_VL53L1X
            DistanceSensor prototype (VL53L1X + AT42QT1050)
    BOARD_BREAKOUT_MODULE=<BREAKOUTBOARD>
        Specify whether and which breakout board is attached
        (default: $(BOARD_BREAKOUT_MODULE_DEFAULT)).
        Available settings are:
        - BOARD_BREAKOUT_NONE
            No breakout board attached.
        - BOARD_BREAKOUT_ESP
            ESP32

endef

info:
	$(info $(INFO_TEXT))
	@exit

#                                                                              #
# Custom rules                                                                 #
################################################################################
