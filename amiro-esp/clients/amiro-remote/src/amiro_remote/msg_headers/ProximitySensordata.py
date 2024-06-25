r"""Wrapper for ProximitySensordata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//ProximitySensordata.h --no-embed-preamble -o ProximitySensordata.py

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

proximitysensor_proximity_t = uint16_t# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 59

proximitysensor_ambient_t = uint16_t# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 60

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 67
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'nnw',
    'wnw',
    'wsw',
    'ssw',
    'sse',
    'ese',
    'ene',
    'nne',
]
struct_anon_2._fields_ = [
    ('nnw', proximitysensor_proximity_t),
    ('wnw', proximitysensor_proximity_t),
    ('wsw', proximitysensor_proximity_t),
    ('ssw', proximitysensor_proximity_t),
    ('sse', proximitysensor_proximity_t),
    ('ese', proximitysensor_proximity_t),
    ('ene', proximitysensor_proximity_t),
    ('nne', proximitysensor_proximity_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 77
class union_anon_3(Union):
    pass

union_anon_3.__slots__ = [
    'data',
    'sensors',
]
union_anon_3._fields_ = [
    ('data', proximitysensor_proximity_t * int(8)),
    ('sensors', struct_anon_2),
]

proximitysensor_proximitydata_t = union_anon_3# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 77

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 84
class struct_anon_4(Structure):
    pass

struct_anon_4.__slots__ = [
    'nnw',
    'wnw',
    'wsw',
    'ssw',
    'sse',
    'ese',
    'ene',
    'nne',
]
struct_anon_4._fields_ = [
    ('nnw', proximitysensor_ambient_t),
    ('wnw', proximitysensor_ambient_t),
    ('wsw', proximitysensor_ambient_t),
    ('ssw', proximitysensor_ambient_t),
    ('sse', proximitysensor_ambient_t),
    ('ese', proximitysensor_ambient_t),
    ('ene', proximitysensor_ambient_t),
    ('nne', proximitysensor_ambient_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 94
class union_anon_5(Union):
    pass

union_anon_5.__slots__ = [
    'data',
    'sensors',
]
union_anon_5._fields_ = [
    ('data', proximitysensor_ambient_t * int(8)),
    ('sensors', struct_anon_4),
]

proximitysensor_ambientdata_t = union_anon_5# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 94

enum_anon_11 = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 107

PROXIMITYSENSOR_CALIBRATION_GET = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 107

PROXIMITYSENSOR_CALIBRATION_SETONE = (PROXIMITYSENSOR_CALIBRATION_GET + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 107

PROXIMITYSENSOR_CALIBRATION_SETALL = (PROXIMITYSENSOR_CALIBRATION_SETONE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 107

PROXIMITYSENSOR_CALIBRATION_AUTO = (PROXIMITYSENSOR_CALIBRATION_SETALL + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 107

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 121
class struct_anon_6(Structure):
    pass

struct_anon_6.__slots__ = [
    'sensor',
    'value',
]
struct_anon_6._fields_ = [
    ('sensor', uint32_t),
    ('value', proximitysensor_proximity_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 117
class union_anon_7(Union):
    pass

union_anon_7.__slots__ = [
    'offset',
    'offsets',
    'measurements',
]
union_anon_7._fields_ = [
    ('offset', struct_anon_6),
    ('offsets', proximitysensor_proximitydata_t),
    ('measurements', uint32_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 103
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

enum_anon_12 = c_int# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 152

PROXIMITYSENSOR_CALIBSTATUS_OK = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 152

PROXIMITYSENSOR_CALIBSTATUS_FAIL = (PROXIMITYSENSOR_CALIBSTATUS_OK + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 152

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 148
class struct_anon_9(Structure):
    pass

struct_anon_9.__slots__ = [
    'status',
    'offsets',
]
struct_anon_9._fields_ = [
    ('status', enum_anon_12),
    ('offsets', proximitysensor_proximitydata_t),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 161
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

proximitysensor_calibration_t = union_anon_10# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 161

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/ProximitySensordata.h: 45
try:
    PROXIMITYSENSOR_NUM_SENSORS = 8
except:
    pass

# No inserted files

# No prefix-stripping

