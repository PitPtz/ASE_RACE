################################################################################
# µRT is a lightweight publish-subscribe & RPC middleware for real-time        #
# applications. It was developed as part of the software habitat for the       #
# Autonomous Mini Robot (AMiRo) but can be used for other purposes as well.    #
# Copyright (C) 2018..2022  Thomas Schöpping et al.                            #
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



# absolute path to this directory
URT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# include paths
URT_INC = $(URT_DIR:/=) \
          $(URT_DIR)inc

# C sources
URT_CSRC = $(URT_DIR)src/urt_request.c \
           $(URT_DIR)src/urt_service.c \
           $(URT_DIR)src/urt_core.c \
           $(URT_DIR)src/urt_message.c \
           $(URT_DIR)src/urt_node.c \
           $(URT_DIR)src/urt_publisher.c \
           $(URT_DIR)src/urt_subscriber.c \
           $(URT_DIR)src/urt_sync.c \
           $(URT_DIR)src/urt_topic.c

# C++ sources
URT_CPPSRC =
