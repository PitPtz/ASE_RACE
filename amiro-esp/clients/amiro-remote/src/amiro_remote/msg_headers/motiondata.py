r"""Wrapper for motiondata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//motiondata.h --no-embed-preamble -o motiondata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 79
class struct_translation_euler_u(Structure):
    pass

struct_translation_euler_u.__slots__ = [
    'axes',
]
struct_translation_euler_u._fields_ = [
    ('axes', c_int32 * int(3)),
]

translation_euler_u = struct_translation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 79

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 92
class struct_translation_euler_si(Structure):
    pass

struct_translation_euler_si.__slots__ = [
    'axes',
]
struct_translation_euler_si._fields_ = [
    ('axes', c_float * int(3)),
]

translation_euler_si = struct_translation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 92

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 116
class struct_translation_polar_u(Structure):
    pass

struct_translation_polar_u.__slots__ = [
    'radial_distance',
    'angles',
]
struct_translation_polar_u._fields_ = [
    ('radial_distance', uint32_t),
    ('angles', uint32_t * int((3 - 1))),
]

translation_polar_u = struct_translation_polar_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 116

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 140
class struct_translation_polar_si(Structure):
    pass

struct_translation_polar_si.__slots__ = [
    'radial_distance',
    'angles',
]
struct_translation_polar_si._fields_ = [
    ('radial_distance', c_float),
    ('angles', c_float * int((3 - 1))),
]

translation_polar_si = struct_translation_polar_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 140

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 161
class struct_rotation_vector_u(Structure):
    pass

struct_rotation_vector_u.__slots__ = [
    'vector',
    'angle',
]
struct_rotation_vector_u._fields_ = [
    ('vector', uint32_t * int(3)),
    ('angle', c_int32),
]

rotation_vector_u = struct_rotation_vector_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 161

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 179
class struct_rotation_vector_si(Structure):
    pass

struct_rotation_vector_si.__slots__ = [
    'angle',
]
struct_rotation_vector_si._fields_ = [
    ('angle', c_float),
]

rotation_vector_si = struct_rotation_vector_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 179

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 192
class struct_rotation_vector_csi(Structure):
    pass

struct_rotation_vector_csi.__slots__ = [
    'vector',
]
struct_rotation_vector_csi._fields_ = [
    ('vector', c_float * int(3)),
]

rotation_vector_csi = struct_rotation_vector_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 192

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 203
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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 217
class struct_rotation_euler_u(Structure):
    pass

struct_rotation_euler_u.__slots__ = [
    'rotation',
]
struct_rotation_euler_u._fields_ = [
    ('rotation', struct_anon_2 * int(3)),
]

rotation_euler_u = struct_rotation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 217

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 228
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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 242
class struct_rotation_euler_si(Structure):
    pass

struct_rotation_euler_si.__slots__ = [
    'rotation',
]
struct_rotation_euler_si._fields_ = [
    ('rotation', struct_anon_3 * int(3)),
]

rotation_euler_si = struct_rotation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 242

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 261
class struct_rotation_quaternion_i(Structure):
    pass

struct_rotation_quaternion_i.__slots__ = [
    'vector',
    'scalar',
]
struct_rotation_quaternion_i._fields_ = [
    ('vector', c_int32 * int(3)),
    ('scalar', c_int32),
]

rotation_quaternion_i = struct_rotation_quaternion_i# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 261

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 278
class struct_rotation_quaternion_f(Structure):
    pass

struct_rotation_quaternion_f.__slots__ = [
    'vector',
    'scalar',
]
struct_rotation_quaternion_f._fields_ = [
    ('vector', c_float * int(3)),
    ('scalar', c_float),
]

rotation_quaternion_f = struct_rotation_quaternion_f# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 278

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 291
class struct_rotation_quaternion_cf(Structure):
    pass

struct_rotation_quaternion_cf.__slots__ = [
    'vector',
]
struct_rotation_quaternion_cf._fields_ = [
    ('vector', c_float * int(3)),
]

rotation_quaternion_cf = struct_rotation_quaternion_cf# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 291

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 312
class struct_motion_ev_u(Structure):
    pass

struct_motion_ev_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_ev_u._fields_ = [
    ('translation', translation_euler_u),
    ('rotation', rotation_vector_u),
]

motion_ev_u = struct_motion_ev_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 312

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 331
class struct_motion_ev_si(Structure):
    pass

struct_motion_ev_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_ev_si._fields_ = [
    ('translation', translation_euler_si),
    ('rotation', rotation_vector_si),
]

motion_ev_si = struct_motion_ev_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 331

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 350
class struct_motion_ev_csi(Structure):
    pass

struct_motion_ev_csi.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_ev_csi._fields_ = [
    ('translation', translation_euler_si),
    ('rotation', rotation_vector_csi),
]

motion_ev_csi = struct_motion_ev_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 350

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 369
class struct_motion_ee_u(Structure):
    pass

struct_motion_ee_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_ee_u._fields_ = [
    ('translation', translation_euler_u),
    ('rotation', rotation_euler_u),
]

motion_ee_u = struct_motion_ee_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 369

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 388
class struct_motion_ee_si(Structure):
    pass

struct_motion_ee_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_ee_si._fields_ = [
    ('translation', translation_euler_si),
    ('rotation', rotation_euler_si),
]

motion_ee_si = struct_motion_ee_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 388

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 407
class struct_motion_eq_u(Structure):
    pass

struct_motion_eq_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_eq_u._fields_ = [
    ('translation', translation_euler_u),
    ('rotation', rotation_quaternion_i),
]

motion_eq_u = struct_motion_eq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 407

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 426
class struct_motion_eq_si(Structure):
    pass

struct_motion_eq_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_eq_si._fields_ = [
    ('translation', translation_euler_si),
    ('rotation', rotation_quaternion_f),
]

motion_eq_si = struct_motion_eq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 426

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 445
class struct_motion_eq_csi(Structure):
    pass

struct_motion_eq_csi.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_eq_csi._fields_ = [
    ('translation', translation_euler_si),
    ('rotation', rotation_quaternion_cf),
]

motion_eq_csi = struct_motion_eq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 445

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 464
class struct_motion_pv_u(Structure):
    pass

struct_motion_pv_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pv_u._fields_ = [
    ('translation', translation_polar_u),
    ('rotation', rotation_vector_u),
]

motion_pv_u = struct_motion_pv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 464

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 483
class struct_motion_pv_si(Structure):
    pass

struct_motion_pv_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pv_si._fields_ = [
    ('translation', translation_polar_si),
    ('rotation', rotation_vector_si),
]

motion_pv_si = struct_motion_pv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 483

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 502
class struct_motion_pv_csi(Structure):
    pass

struct_motion_pv_csi.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pv_csi._fields_ = [
    ('translation', translation_polar_si),
    ('rotation', rotation_vector_csi),
]

motion_pv_csi = struct_motion_pv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 502

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 521
class struct_motion_pe_u(Structure):
    pass

struct_motion_pe_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pe_u._fields_ = [
    ('translation', translation_polar_u),
    ('rotation', rotation_euler_u),
]

motion_pe_u = struct_motion_pe_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 521

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 540
class struct_motion_pe_si(Structure):
    pass

struct_motion_pe_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pe_si._fields_ = [
    ('translation', translation_polar_si),
    ('rotation', rotation_euler_si),
]

motion_pe_si = struct_motion_pe_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 540

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 559
class struct_motion_pq_u(Structure):
    pass

struct_motion_pq_u.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pq_u._fields_ = [
    ('translation', translation_polar_u),
    ('rotation', rotation_quaternion_i),
]

motion_pq_u = struct_motion_pq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 559

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 578
class struct_motion_pq_si(Structure):
    pass

struct_motion_pq_si.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pq_si._fields_ = [
    ('translation', translation_polar_si),
    ('rotation', rotation_quaternion_f),
]

motion_pq_si = struct_motion_pq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 578

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 597
class struct_motion_pq_csi(Structure):
    pass

struct_motion_pq_csi.__slots__ = [
    'translation',
    'rotation',
]
struct_motion_pq_csi._fields_ = [
    ('translation', translation_polar_si),
    ('rotation', rotation_quaternion_cf),
]

motion_pq_csi = struct_motion_pq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 597

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 51
try:
    MOTIONDATA_AXES = 3
except:
    pass

translation_euler_u = struct_translation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 79

translation_euler_si = struct_translation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 92

translation_polar_u = struct_translation_polar_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 116

translation_polar_si = struct_translation_polar_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 140

rotation_vector_u = struct_rotation_vector_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 161

rotation_vector_si = struct_rotation_vector_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 179

rotation_vector_csi = struct_rotation_vector_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 192

rotation_euler_u = struct_rotation_euler_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 217

rotation_euler_si = struct_rotation_euler_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 242

rotation_quaternion_i = struct_rotation_quaternion_i# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 261

rotation_quaternion_f = struct_rotation_quaternion_f# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 278

rotation_quaternion_cf = struct_rotation_quaternion_cf# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 291

motion_ev_u = struct_motion_ev_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 312

motion_ev_si = struct_motion_ev_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 331

motion_ev_csi = struct_motion_ev_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 350

motion_ee_u = struct_motion_ee_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 369

motion_ee_si = struct_motion_ee_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 388

motion_eq_u = struct_motion_eq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 407

motion_eq_si = struct_motion_eq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 426

motion_eq_csi = struct_motion_eq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 445

motion_pv_u = struct_motion_pv_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 464

motion_pv_si = struct_motion_pv_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 483

motion_pv_csi = struct_motion_pv_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 502

motion_pe_u = struct_motion_pe_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 521

motion_pe_si = struct_motion_pe_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 540

motion_pq_u = struct_motion_pq_u# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 559

motion_pq_si = struct_motion_pq_si# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 578

motion_pq_csi = struct_motion_pq_csi# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/motiondata.h: 597

# No inserted files

# No prefix-stripping

