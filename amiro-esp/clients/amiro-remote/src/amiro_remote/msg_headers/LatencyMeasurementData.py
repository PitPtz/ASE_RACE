r"""Wrapper for LatencyMeasurementData.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//LatencyMeasurementData.h --no-embed-preamble -o LatencyMeasurementData.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 59
class struct_lm_pub_data(Structure):
    pass

struct_lm_pub_data.__slots__ = [
    'callback_msg',
]
struct_lm_pub_data._fields_ = [
    ('callback_msg', c_int32),
]

lm_callback_data_t = struct_lm_pub_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 59

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 67
class struct_lm_trigger_data(Structure):
    pass

struct_lm_trigger_data.__slots__ = [
    'trigger_data',
]
struct_lm_trigger_data._fields_ = [
    ('trigger_data', uint8_t),
]

lm_trigger_data_t = struct_lm_trigger_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 67

lm_pub_data = struct_lm_pub_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 59

lm_trigger_data = struct_lm_trigger_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LatencyMeasurementData.h: 67

# No inserted files

# No prefix-stripping

