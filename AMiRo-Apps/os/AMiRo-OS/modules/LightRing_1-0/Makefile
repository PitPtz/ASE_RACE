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
USE_FPU ?= no

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
MCU := cortex-m3

# Module specific paths and directories.
include ../defaults.mk

# Linker script.
LDSCRIPT := $(MODULE_DIR)/STM32F103xE.ld

# General AMiRo-OS files.
include ../../amiro-os.mk

# Module specific ChibiOS files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f1xx.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F1xx/platform.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk

# Module specific setup configuration.
#n/a

# Module specific periphery LLDs.
include $(AMIROLLD)/drivers/AT24C01B/v1/alld_AT24C01B.mk
include $(AMIROLLD)/drivers/TLC5947/v1/alld_TLC5947.mk
include $(AMIROLLD)/drivers/TPS20xxB/v1/alld_TPS20xxB.mk

# Module specific inclusion directories.
MODULE_INC = $(MODULE_DIR)

# Module specific C source files.
MODULE_CSRC = $(MODULE_DIR)/board.c \
              $(MODULE_DIR)/module.c

# Module specific tests.
include $(MODULE_DIR)/test/AT24C01B/module_test_AT24C01B.mk
include $(MODULE_DIR)/test/TLC5947/module_test_TLC5947.mk
include $(MODULE_DIR)/test/TPS20xxB/module_test_TPS20xxB.mk

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

NAME:       LightRing v1.0
PLATFORM:   AMiRo
MODULE:     LightRing
VERSION:    1.0

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

endef

info:
	$(info $(INFO_TEXT))
	@exit

#                                                                              #
# Custom rules                                                                 #
################################################################################
