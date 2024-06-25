r"""Wrapper for DWD_floordata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//DWD_floordata.h --no-embed-preamble -o DWD_floordata.py

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

floor_proximity_t = uint16_t# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 59

floor_ambient_t = uint16_t# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 60

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 67
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'left_wheel',
    'left_front',
    'right_front',
    'right_wheel',
]
struct_anon_2._fields_ = [
    ('left_wheel', floor_proximity_t),
    ('left_front', floor_proximity_t),
    ('right_front', floor_proximity_t),
    ('right_wheel', floor_proximity_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 73
class union_anon_3(Union):
    pass

union_anon_3.__slots__ = [
    'data',
    'sensors',
]
union_anon_3._fields_ = [
    ('data', floor_proximity_t * int(4)),
    ('sensors', struct_anon_2),
]

floor_proximitydata_t = union_anon_3# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 73

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 80
class struct_anon_4(Structure):
    pass

struct_anon_4.__slots__ = [
    'left_wheel',
    'left_front',
    'right_front',
    'right_wheel',
]
struct_anon_4._fields_ = [
    ('left_wheel', floor_ambient_t),
    ('left_front', floor_ambient_t),
    ('right_front', floor_ambient_t),
    ('right_wheel', floor_ambient_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 86
class union_anon_5(Union):
    pass

union_anon_5.__slots__ = [
    'data',
    'sensors',
]
union_anon_5._fields_ = [
    ('data', floor_ambient_t * int(4)),
    ('sensors', struct_anon_4),
]

floor_ambientdata_t = union_anon_5# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 86

enum_anon_11 = c_int# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 99

FLOOR_CALIBRATION_GET = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 99

FLOOR_CALIBRATION_SETONE = (FLOOR_CALIBRATION_GET + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 99

FLOOR_CALIBRATION_SETALL = (FLOOR_CALIBRATION_SETONE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 99

FLOOR_CALIBRATION_AUTO = (FLOOR_CALIBRATION_SETALL + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 99

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 113
class struct_anon_6(Structure):
    pass

struct_anon_6.__slots__ = [
    'sensor',
    'value',
]
struct_anon_6._fields_ = [
    ('sensor', uint32_t),
    ('value', floor_proximity_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 109
class union_anon_7(Union):
    pass

union_anon_7.__slots__ = [
    'offset',
    'offsets',
    'measurements',
]
union_anon_7._fields_ = [
    ('offset', struct_anon_6),
    ('offsets', floor_proximitydata_t),
    ('measurements', uint32_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 95
class struct_anon_8(Structure):
    pass

struct_anon_8.__slots__ = [
    'command',
    'data',
]
struct_anon_8._fields_ = [
    ('command', enum_anon_11),
    ('data', union_anon_7),
]

enum_anon_12 = c_int# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 144

FLOOR_CALIBSTATUS_OK = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 144

FLOOR_CALIBSTATUS_FAIL = (FLOOR_CALIBSTATUS_OK + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 144

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 140
class struct_anon_9(Structure):
    pass

struct_anon_9.__slots__ = [
    'status',
    'offsets',
]
struct_anon_9._fields_ = [
    ('status', enum_anon_12),
    ('offsets', floor_proximitydata_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 153
class union_anon_10(Union):
    pass

union_anon_10.__slots__ = [
    'request',
    'response',
]
union_anon_10._fields_ = [
    ('request', struct_anon_8),
    ('response', struct_anon_9),
]

floor_calibration_t = union_anon_10# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 153

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_floordata.h: 45
try:
    FLOOR_NUM_SENSORS = 4
except:
    pass

# No inserted files

# No prefix-stripping

