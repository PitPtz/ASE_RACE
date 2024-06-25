r"""Wrapper for chargedata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//chargedata.h --no-embed-preamble -o chargedata.py

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

__uint16_t = c_ushort# /usr/include/x86_64-linux-gnu/bits/types.h: 40

__uint32_t = c_uint# /usr/include/x86_64-linux-gnu/bits/types.h: 42

uint16_t = __uint16_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 25

uint32_t = __uint32_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 26

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 68
class struct_chargedata_u(Structure):
    pass

struct_chargedata_u.__slots__ = [
    'volts',
    'charging',
]
struct_chargedata_u._fields_ = [
    ('volts', uint32_t),
    ('charging', c_bool),
]

chargedata_u = struct_chargedata_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 68

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 84
class struct_chargedata_si(Structure):
    pass

struct_chargedata_si.__slots__ = [
    'volts',
    'charging',
]
struct_chargedata_si._fields_ = [
    ('volts', c_float),
    ('charging', c_bool),
]

chargedata_si = struct_chargedata_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 84

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 102
class struct_battery_data(Structure):
    pass

struct_battery_data.__slots__ = [
    'percentage',
    'size',
]
struct_battery_data._fields_ = [
    ('percentage', uint16_t),
    ('size', uint16_t),
]

battery_data_t = struct_battery_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 102

chargedata_u = struct_chargedata_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 68

chargedata_si = struct_chargedata_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 84

battery_data = struct_battery_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/chargedata.h: 102

# No inserted files

# No prefix-stripping

