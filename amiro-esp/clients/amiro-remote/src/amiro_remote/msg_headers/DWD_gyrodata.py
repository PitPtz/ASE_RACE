r"""Wrapper for DWD_gyrodata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//DWD_gyrodata.h --no-embed-preamble -o DWD_gyrodata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 58
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'x',
    'y',
    'z',
]
struct_anon_2._fields_ = [
    ('x', c_float),
    ('y', c_float),
    ('z', c_float),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 63
class union_anon_3(Union):
    pass

union_anon_3.__slots__ = [
    'data',
    'values',
]
union_anon_3._fields_ = [
    ('data', c_float * int(3)),
    ('values', struct_anon_2),
]

gyro_converted_data_t = union_anon_3# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 63

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 71
class struct_anon_4(Structure):
    pass

struct_anon_4.__slots__ = [
    'x',
    'y',
    'z',
]
struct_anon_4._fields_ = [
    ('x', c_int16),
    ('y', c_int16),
    ('z', c_int16),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 76
class union_anon_5(Union):
    pass

union_anon_5.__slots__ = [
    'data',
    'values',
]
union_anon_5._fields_ = [
    ('data', c_int16 * int(3)),
    ('values', struct_anon_4),
]

gyro_sensor_t = union_anon_5# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 76

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 83
class struct_gyro_data(Structure):
    pass

struct_gyro_data.__slots__ = [
    'values',
]
struct_gyro_data._fields_ = [
    ('values', gyro_sensor_t),
]

gyro_data_t = struct_gyro_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 83

gyro_data = struct_gyro_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gyrodata.h: 83

# No inserted files

# No prefix-stripping

