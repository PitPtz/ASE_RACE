r"""Wrapper for TouchSensordata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//TouchSensordata.h --no-embed-preamble -o TouchSensordata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 57
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'nnw',
    'wnw',
    'wsw',
    'ssw',
]
struct_anon_2._fields_ = [
    ('nnw', uint8_t),
    ('wnw', uint8_t),
    ('wsw', uint8_t),
    ('ssw', uint8_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 63
class union_anon_3(Union):
    pass

union_anon_3.__slots__ = [
    'data',
    'values',
]
union_anon_3._fields_ = [
    ('data', uint8_t * int(4)),
    ('values', struct_anon_2),
]

touch_sensors_t = union_anon_3# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 63

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 70
class struct_touch_data(Structure):
    pass

struct_touch_data.__slots__ = [
    'values',
]
struct_touch_data._fields_ = [
    ('values', touch_sensors_t),
]

touch_data_t = struct_touch_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 70

touch_data = struct_touch_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/TouchSensordata.h: 70

# No inserted files

# No prefix-stripping

