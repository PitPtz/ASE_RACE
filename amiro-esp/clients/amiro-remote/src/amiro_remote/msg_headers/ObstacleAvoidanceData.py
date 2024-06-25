r"""Wrapper for ObstacleAvoidanceData.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//ObstacleAvoidanceData.h --no-embed-preamble -o ObstacleAvoidanceData.py

Do not modify this file.
"""

__docformat__ = "restructuredtext"

# Begin preamble for Python

from .ctypes_preamble import *
from .ctypes_preamble import _variadic_function

# End preamble

_libs = {}
_libdirs = []

# Begin loader

from .ctypes_loader import *

# End loader

add_library_search_dirs([])

# No libraries

# No modules

__uint8_t = c_ubyte# /usr/include/x86_64-linux-gnu/bits/types.h: 38

uint8_t = __uint8_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 24

enum_oa_state = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 60

OA_IDLE = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 60

OBSTACLE_AVOIDANCE = (OA_IDLE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 60

oa_state_t = enum_oa_state# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 60

enum_ObstacleAvoidanceStrategy = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 65

OA_OFF = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 65

OA_ON = (OA_OFF + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 65

oa_strategy_t = enum_ObstacleAvoidanceStrategy# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 65

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 69
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'enable_light',
]
struct_anon_2._fields_ = [
    ('enable_light', uint8_t),
]

oa_enable_light_t = struct_anon_2# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 69

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 74
class struct_oa_activation_service(Structure):
    pass

struct_oa_activation_service.__slots__ = [
    'strategy',
    'light',
]
struct_oa_activation_service._fields_ = [
    ('strategy', oa_strategy_t),
    ('light', oa_enable_light_t),
]

oa_activation_service_t = struct_oa_activation_service# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 74

oa_activation_service = struct_oa_activation_service# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ObstacleAvoidanceData.h: 74

# No inserted files

# No prefix-stripping

