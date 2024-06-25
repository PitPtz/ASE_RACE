r"""Wrapper for DWD_gravitydata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//DWD_gravitydata.h --no-embed-preamble -o DWD_gravitydata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gravitydata.h: 58
class struct_anon_1(Structure):
    pass

struct_anon_1.__slots__ = [
    'x',
    'y',
    'z',
]
struct_anon_1._fields_ = [
    ('x', c_float),
    ('y', c_float),
    ('z', c_float),
]

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gravitydata.h: 63
class union_anon_2(Union):
    pass

union_anon_2.__slots__ = [
    'data',
    'values',
]
union_anon_2._fields_ = [
    ('data', c_float * int(3)),
    ('values', struct_anon_1),
]

gravity_data_t = union_anon_2# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/DWD_gravitydata.h: 63

# No inserted files

# No prefix-stripping

