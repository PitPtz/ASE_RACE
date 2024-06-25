r"""Wrapper for positiondata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//positiondata.h --no-embed-preamble -o positiondata.py

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

__uint32_t = c_uint# /usr/include/x86_64-linux-gnu/bits/types.h: 42

uint8_t = __uint8_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 24

uint32_t = __uint32_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 26

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 79
class struct_location_cartesian_u(Structure):
    pass

struct_location_cartesian_u.__slots__ = [
    'axes',
]
struct_location_cartesian_u._fields_ = [
    ('axes', c_int32 * int(3)),
]

location_cartesian_u = struct_location_cartesian_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 79

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 92
class struct_location_cartesian_si(Structure):
    pass

struct_location_cartesian_si.__slots__ = [
    'axes',
]
struct_location_cartesian_si._fields_ = [
    ('axes', c_float * int(3)),
]

location_cartesian_si = struct_location_cartesian_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 92

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 116
class struct_location_polar_u(Structure):
    pass

struct_location_polar_u.__slots__ = [
    'radial_distance',
    'angles',
]
struct_location_polar_u._fields_ = [
    ('radial_distance', uint32_t),
    ('angles', uint32_t * int((3 - 1))),
]

location_polar_u = struct_location_polar_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 116

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 140
class struct_location_polar_si(Structure):
    pass

struct_location_polar_si.__slots__ = [
    'radial_distance',
    'angles',
]
struct_location_polar_si._fields_ = [
    ('radial_distance', c_float),
    ('angles', c_float * int((3 - 1))),
]

location_polar_si = struct_location_polar_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 140

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 161
class struct_orientation_vector_u(Structure):
    pass

struct_orientation_vector_u.__slots__ = [
    'vector',
    'angle',
]
struct_orientation_vector_u._fields_ = [
    ('vector', uint32_t * int(3)),
    ('angle', c_int32),
]

orientation_vector_u = struct_orientation_vector_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 161

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 179
class struct_orientation_vector_si(Structure):
    pass

struct_orientation_vector_si.__slots__ = [
    'angle',
]
struct_orientation_vector_si._fields_ = [
    ('angle', c_float),
]

orientation_vector_si = struct_orientation_vector_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 179

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 192
class struct_orientation_vector_csi(Structure):
    pass

struct_orientation_vector_csi.__slots__ = [
    'vector',
]
struct_orientation_vector_csi._fields_ = [
    ('vector', c_float * int(3)),
]

orientation_vector_csi = struct_orientation_vector_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 192

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 203
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'axis',
    'angle',
]
struct_anon_2._fields_ = [
    ('axis', uint8_t),
    ('angle', c_int32),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 217
class struct_orientation_euler_u(Structure):
    pass

struct_orientation_euler_u.__slots__ = [
    'rotation',
]
struct_orientation_euler_u._fields_ = [
    ('rotation', struct_anon_2 * int(3)),
]

orientation_euler_u = struct_orientation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 217

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 228
class struct_anon_3(Structure):
    pass

struct_anon_3.__slots__ = [
    'axis',
    'angle',
]
struct_anon_3._fields_ = [
    ('axis', uint8_t),
    ('angle', c_float),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 242
class struct_orientation_euler_si(Structure):
    pass

struct_orientation_euler_si.__slots__ = [
    'rotation',
]
struct_orientation_euler_si._fields_ = [
    ('rotation', struct_anon_3 * int(3)),
]

orientation_euler_si = struct_orientation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 242

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 261
class struct_orientation_quaternion_i(Structure):
    pass

struct_orientation_quaternion_i.__slots__ = [
    'vector',
    'scalar',
]
struct_orientation_quaternion_i._fields_ = [
    ('vector', c_int32 * int(3)),
    ('scalar', c_int32),
]

orientation_quaternion_i = struct_orientation_quaternion_i# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 261

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 278
class struct_orientation_quaternion_f(Structure):
    pass

struct_orientation_quaternion_f.__slots__ = [
    'vector',
    'scalar',
]
struct_orientation_quaternion_f._fields_ = [
    ('vector', c_float * int(3)),
    ('scalar', c_float),
]

orientation_quaternion_f = struct_orientation_quaternion_f# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 278

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 292
class struct_orientation_quaternion_cf(Structure):
    pass

struct_orientation_quaternion_cf.__slots__ = [
    'vector',
]
struct_orientation_quaternion_cf._fields_ = [
    ('vector', c_float * int(3)),
]

orientation_quaternion_cf = struct_orientation_quaternion_cf# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 292

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 313
class struct_position_cv_u(Structure):
    pass

struct_position_cv_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cv_u._fields_ = [
    ('location', location_cartesian_u),
    ('oriantation', orientation_vector_u),
]

position_cv_u = struct_position_cv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 313

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 332
class struct_position_cv_si(Structure):
    pass

struct_position_cv_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cv_si._fields_ = [
    ('location', location_cartesian_si),
    ('oriantation', orientation_vector_si),
]

position_cv_si = struct_position_cv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 332

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 351
class struct_position_cv_csi(Structure):
    pass

struct_position_cv_csi.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cv_csi._fields_ = [
    ('location', location_cartesian_si),
    ('oriantation', orientation_vector_csi),
]

position_cv_csi = struct_position_cv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 351

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 370
class struct_position_ce_u(Structure):
    pass

struct_position_ce_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_ce_u._fields_ = [
    ('location', location_cartesian_u),
    ('oriantation', orientation_euler_u),
]

position_ce_u = struct_position_ce_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 370

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 389
class struct_position_ce_si(Structure):
    pass

struct_position_ce_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_ce_si._fields_ = [
    ('location', location_cartesian_si),
    ('oriantation', orientation_euler_si),
]

position_ce_si = struct_position_ce_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 389

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 408
class struct_position_cq_u(Structure):
    pass

struct_position_cq_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cq_u._fields_ = [
    ('location', location_cartesian_u),
    ('oriantation', orientation_quaternion_i),
]

position_cq_u = struct_position_cq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 408

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 427
class struct_position_cq_si(Structure):
    pass

struct_position_cq_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cq_si._fields_ = [
    ('location', location_cartesian_si),
    ('oriantation', orientation_quaternion_f),
]

position_cq_si = struct_position_cq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 427

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 446
class struct_position_cq_csi(Structure):
    pass

struct_position_cq_csi.__slots__ = [
    'location',
    'oriantation',
]
struct_position_cq_csi._fields_ = [
    ('location', location_cartesian_si),
    ('oriantation', orientation_quaternion_cf),
]

position_cq_csi = struct_position_cq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 446

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 465
class struct_position_pv_u(Structure):
    pass

struct_position_pv_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pv_u._fields_ = [
    ('location', location_polar_u),
    ('oriantation', orientation_vector_u),
]

position_pv_u = struct_position_pv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 465

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 484
class struct_position_pv_si(Structure):
    pass

struct_position_pv_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pv_si._fields_ = [
    ('location', location_polar_si),
    ('oriantation', orientation_vector_si),
]

position_pv_si = struct_position_pv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 484

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 503
class struct_position_pv_csi(Structure):
    pass

struct_position_pv_csi.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pv_csi._fields_ = [
    ('location', location_polar_si),
    ('oriantation', orientation_vector_csi),
]

position_pv_csi = struct_position_pv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 503

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 522
class struct_position_pe_u(Structure):
    pass

struct_position_pe_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pe_u._fields_ = [
    ('location', location_polar_u),
    ('oriantation', orientation_euler_u),
]

position_pe_u = struct_position_pe_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 522

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 541
class struct_position_pe_si(Structure):
    pass

struct_position_pe_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pe_si._fields_ = [
    ('location', location_polar_si),
    ('oriantation', orientation_euler_si),
]

position_pe_si = struct_position_pe_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 541

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 560
class struct_position_pq_u(Structure):
    pass

struct_position_pq_u.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pq_u._fields_ = [
    ('location', location_polar_u),
    ('oriantation', orientation_quaternion_i),
]

position_pq_u = struct_position_pq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 560

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 579
class struct_position_pq_si(Structure):
    pass

struct_position_pq_si.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pq_si._fields_ = [
    ('location', location_polar_si),
    ('oriantation', orientation_quaternion_f),
]

position_pq_si = struct_position_pq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 579

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 598
class struct_position_pq_csi(Structure):
    pass

struct_position_pq_csi.__slots__ = [
    'location',
    'oriantation',
]
struct_position_pq_csi._fields_ = [
    ('location', location_polar_si),
    ('oriantation', orientation_quaternion_cf),
]

position_pq_csi = struct_position_pq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 598

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 51
try:
    POSITIONDATA_AXES = 3
except:
    pass

location_cartesian_u = struct_location_cartesian_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 79

location_cartesian_si = struct_location_cartesian_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 92

location_polar_u = struct_location_polar_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 116

location_polar_si = struct_location_polar_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 140

orientation_vector_u = struct_orientation_vector_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 161

orientation_vector_si = struct_orientation_vector_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 179

orientation_vector_csi = struct_orientation_vector_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 192

orientation_euler_u = struct_orientation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 217

orientation_euler_si = struct_orientation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 242

orientation_quaternion_i = struct_orientation_quaternion_i# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 261

orientation_quaternion_f = struct_orientation_quaternion_f# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 278

orientation_quaternion_cf = struct_orientation_quaternion_cf# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 292

position_cv_u = struct_position_cv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 313

position_cv_si = struct_position_cv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 332

position_cv_csi = struct_position_cv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 351

position_ce_u = struct_position_ce_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 370

position_ce_si = struct_position_ce_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 389

position_cq_u = struct_position_cq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 408

position_cq_si = struct_position_cq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 427

position_cq_csi = struct_position_cq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 446

position_pv_u = struct_position_pv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 465

position_pv_si = struct_position_pv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 484

position_pv_csi = struct_position_pv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 503

position_pe_u = struct_position_pe_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 522

position_pe_si = struct_position_pe_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 541

position_pq_u = struct_position_pq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 560

position_pq_si = struct_position_pq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 579

position_pq_csi = struct_position_pq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/positiondata.h: 598

# No inserted files

# No prefix-stripping

