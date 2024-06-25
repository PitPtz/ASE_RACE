r"""Wrapper for amirostatus.h

Generated with:
/home/pit/anaconda3/envs/bivital/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//amirostatus.h --no-embed-preamble -o amirostatus.py

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

__uint8_t = c_ubyte# /usr/include/x86_64-linux-gnu/bits/types.h: 37

uint8_t = __uint8_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 24

# /home/pit/ASE/AMiRo-Apps/messagetypes/amirostatus.h: 59
class struct_status_data_t(Structure):
    pass

struct_status_data_t.__slots__ = [
    'status',
]
struct_status_data_t._fields_ = [
    ('status', uint8_t),
]

status_data_t = struct_status_data_t# /home/pit/ASE/AMiRo-Apps/messagetypes/amirostatus.h: 59

status_data_t = struct_status_data_t# /home/pit/ASE/AMiRo-Apps/messagetypes/amirostatus.h: 59

# No inserted files

# No prefix-stripping

