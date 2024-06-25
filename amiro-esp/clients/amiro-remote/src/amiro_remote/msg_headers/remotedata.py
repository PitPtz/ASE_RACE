r"""Wrapper for remotedata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//remotedata.h --no-embed-preamble -o remotedata.py

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

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/remotedata.h: 57
class struct_remote_data(Structure):
    pass

struct_remote_data.__slots__ = [
    'bytes',
]
struct_remote_data._fields_ = [
    ('bytes', uint8_t * int(4)),
]

remote_data_t = struct_remote_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/remotedata.h: 57

remote_data = struct_remote_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/remotedata.h: 57

# No inserted files

# No prefix-stripping

