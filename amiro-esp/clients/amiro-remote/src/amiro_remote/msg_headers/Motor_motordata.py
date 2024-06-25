r"""Wrapper for Motor_motordata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//Motor_motordata.h --no-embed-preamble -o Motor_motordata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/Motor_motordata.h: 60
class struct_motor_data(Structure):
    pass

struct_motor_data.__slots__ = [
    'translation',
    'rotation',
]
struct_motor_data._fields_ = [
    ('translation', c_float),
    ('rotation', c_float),
]

motor_data_t = struct_motor_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/Motor_motordata.h: 60

motor_data = struct_motor_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/Motor_motordata.h: 60

# No inserted files

# No prefix-stripping

