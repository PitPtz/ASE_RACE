r"""Wrapper for LightRing_leddata.h

Generated with:
/homes/cklarhor/AMiRo/ml4proflow/env/bin/ctypesgen ../../../../../../AMiRo-Apps/messagetypes//LightRing_leddata.h --no-embed-preamble -o LightRing_leddata.py

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

enum_anon_2 = c_uint8# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

OFF = 0# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

RED = (OFF + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

GREEN = (RED + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

BLUE = (GREEN + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

YELLOW = (BLUE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

TURQUOISE = (YELLOW + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

MAGENTA = (TURQUOISE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

PURPLE = (MAGENTA + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

ORANGE = (PURPLE + 1)# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

color_t = enum_anon_2# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 120

# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 127
class struct_light_led_data(Structure):
    pass

struct_light_led_data.__slots__ = [
    'color',
]
struct_light_led_data._fields_ = [
    ('color', color_t * int(24)),
]

light_led_data_t = struct_light_led_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 127

light_led_data = struct_light_led_data# /homes/cklarhor/AMiRo/AMiRo-Apps/messagetypes/LightRing_leddata.h: 127

# No inserted files

# No prefix-stripping

