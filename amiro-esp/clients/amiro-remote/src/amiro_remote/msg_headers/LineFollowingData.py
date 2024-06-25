r"""Wrapper for LineFollowingData.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//LineFollowingData.h --no-embed-preamble -o LineFollowingData.py

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

enum_lf_state = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 61

IDLE = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 61

LINEFOLLOWING = (IDLE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 61

EDGEFOLLOWING = (LINEFOLLOWING + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 61

lf_state_t = enum_lf_state# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 61

enum_LineFollowStrategy = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

NONE = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

CENTER = (NONE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

EDGE_LEFT = (CENTER + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

EDGE_RIGHT = (EDGE_LEFT + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

lf_strategy_t = enum_LineFollowStrategy# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 68

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 73
class struct_lf_activation_service(Structure):
    pass

struct_lf_activation_service.__slots__ = [
    'state',
    'strategy',
]
struct_lf_activation_service._fields_ = [
    ('state', lf_state_t),
    ('strategy', lf_strategy_t),
]

lf_activation_service_t = struct_lf_activation_service# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 73

lf_activation_service = struct_lf_activation_service# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LineFollowingData.h: 73

# No inserted files

# No prefix-stripping

