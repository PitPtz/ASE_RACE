r"""Wrapper for DWD_compassdata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//DWD_compassdata.h --no-embed-preamble -o DWD_compassdata.py

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

uint16_t = __uint16_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 25

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 59
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'x',
    'y',
    'z',
]
struct_anon_2._fields_ = [
    ('x', uint16_t),
    ('y', uint16_t),
    ('z', uint16_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 64
class union_anon_3(Union):
    pass

union_anon_3.__slots__ = [
    'data',
    'values',
]
union_anon_3._fields_ = [
    ('data', uint16_t * int(3)),
    ('values', struct_anon_2),
]

compass_sensor_t = union_anon_3# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 64

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 71
class struct_compass_data(Structure):
    pass

struct_compass_data.__slots__ = [
    'values',
]
struct_compass_data._fields_ = [
    ('values', compass_sensor_t),
]

compass_data_t = struct_compass_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 71

compass_data = struct_compass_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_compassdata.h: 71

# No inserted files

# No prefix-stripping

