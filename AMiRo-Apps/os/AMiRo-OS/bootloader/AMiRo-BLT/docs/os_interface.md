About & License
===============

AMiRo-BLT is the bootloader and flashing toolchain for the base version of the
Autonomous Mini Robot (AMiRo) [1]. It is based on OpenBLT developed by Feaser
(see <http://feaser.com/en/openblt.php>).

Copyright (C) 2016..2021 Thomas Schöpping et al. (a complete list of all authors
is given below)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

This research/work was supported by the Cluster of Excellence Cognitive
Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is
funded by the German Research Foundation (DFG).

Authors
-------

- Thomas Schöpping (tschoepp[at]cit-ec.uni-bielefeld.de)
- Stefan Herbrechtsmeier
- Marvin Barther
- Aleksandrs Stier

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

--------------------------------------------------------------------------------

OS Interface
============

The bootloader provides an interface (cf. [`amiroblt.h`](../Target/Source/AMiRo/amiroblt.h)) for the operating system providing various information.
Most importantly, these include the definition of the "callback table" and its memory address as well as the definition of the contents of the backup registers for the _PowerManagement_ module.

The 64 byte callback table contains information about the AMiRo-BLT version and several function pointers.
The former can be used to detect whether the installed bootloader corresponds to the interface or if a legacy version is installed (or none at all).
The latter shall be used as callbacks for restarting the system or shutdown.
This way, the operating system is freed from most aspects of the [Startup Shutdown Synchronization Protocol (SSSP)](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp).


Callback Table
--------------

| bytes | content                   | description                                                                                                               |
|:-----:|:--------------------------|:--------------------------------------------------------------------------------------------------------------------------|
| 1-4   | magic number              | Can be used to check whether it is valid to interpret the data at this address as callback table .                       |
| 5-8   | bootloader version        | Version information about the installed bootloader.                                                                       |
| 9-12  | SSSP version              | Information about the implemented version of [SSSP](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp).                       |
| 13-16 | compiler version          | Version information about the used compiler to build the bootloader.                                                      |
| 17-20 | hibernate callback        | Pointer to a function that shuts the system down to Hibernate mode (cf. [low-power modes](./low_power_modes.md)).         |
| 21-24 | deepsleep callback        | Pointer to a function that shuts the system down to DeepSleep mode (cf. [low-power modes](./low_power_modes.md)).         |
| 25-28 | transportation callback   | Pointer to a function that shuts the system down to Transportation mode (cf. [low-power modes](./low_power_modes.md)).    |
| 29-32 | restart callback          | Pointer to a function that restarts the system.                                                                           |
| 33-36 | shutdown request callback | Pointer to a function that handles passive shutdown requests (cf. [SSSP](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp)). |
| 37-40 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 41-44 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 45-48 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 49-52 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 53-56 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 57-60 | n/a                       | Yet unused slot in the callback table.                                                                                    |
| 61-64 | n/a                       | Yet unused slot in the callback table.                                                                                    |

