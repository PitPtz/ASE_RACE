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



define HELP_TEXT
################################################################################
#                                                                              #
# Copyright (c) 2016..2022  Thomas Schöpping                                   #
#                                                                              #
# This is free software; see the source for copying conditions. There is NO    #
# warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  #
# The development of this software was supported by the Excellence Cluster     #
# EXC 227 Cognitive Interaction Technology. The Excellence Cluster EXC 227 is  #
# a grant of the Deutsche Forschungsgemeinschaft (DFG) in the context of the   #
# German Excellence Initiative.                                                #
#                                                                              #
################################################################################

  AMiRo-OS Makefile help
  ======================

TARGETS:

  help:
      Prints this text.

  all:
      Builds the binaries for all modules.
      This target is automatically parallelized with the number of modules.

  <module>:
      Builds the binary only for the specified module.

  flash_<module>:
      Builds the binary for the specified module and flashes it to the hardware.

  clean:
      Deletes all temporary and binary files of all modules.
      This target is automatically parallelized with the number of modules.

  clean_<module>:
      Deletes all temporary and binary files of the specified module.

  info_<module>:
      Prints some information about the specified module. Most importantly, many
      modules provide additional parameters to customize a build. Such options
      and available settings are described in detail by the modules' information
      messages.

EXAMPLES:

  >$$ make DiWheelDrive_1-1
      This command will generate the binary file for the DiWheelDrive module
      (version 1.1).

  >$$ make DiWheelDrive_1-1 LightRing_1-0
      This command will generate the binary files for the two modules
      DiWheelDrive (version 1.1) and LightRing (version 1.0).

  >$$ make flash_DiWheelDrive_1-1
      This command will build the binary for the DiWheelDrive module
      (version 1.1) only if required, and flash it to the hardware.

  >$$ make clean all flash_DiWheelDrive_1-1
      This command will first clean all projects. In a second step the binaries
      for all modules are build from scratch. Finally the DiWheelDrive module
      (version 1.1) is updated with the latest software.

endef

.PHONY: help

OS_BASE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

help:
	$(info $(HELP_TEXT))
	@exit

include $(OS_BASE_DIR)/modules/Makefile
