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

System Signals
==============

The electrical interface of AMiRo, which connects all AMiRo Modules (AMs), defines six control- and status signals:

- `SYS_COLDRST_N`<br/>
  Resets the whole system the hard way.
  Whenever the signal is active (low), all microcontrollers (MCUs) are reset and power is completely turned off. As soon as the signal turns inactive (high) again, the MCUs restart.
- `SYS_PD_N`<br/>
  This signal is usually set inactive (high) and not used during operation.
  Any AM can activate it (pull it low) to initiate a system shutdown or restart (cf. [SSSP](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp)).
- `SYS_REG_EN`<br/>
  When set active (high), this signal indicates that it is safe to activate any voltage converters that use the system power supply (`VSYS`) to generate custom voltages.
  Whenever it is inactive (low), such converters must be turned off.
- `SYS_SYNC_N` (aka `SYS_INT_N`)<br/>
  Originally meant for system wide interrupts, this signal is now used to synchronize the AMs during startup, shutdown and operation (cf. [SSSP](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp)).
- `SYS_UART_UP/DN`<br/>
  A neighbor signal, which can be used to communicate between adjacent AMs.
  Except for the _DiWheelDrive_ and the _LightRing_, which are the top- and lowermost modules, each AM must provide both signals.
  Note that neither signal is used for UART communication anymore despite their naming.
- `SYS_WARMRST_N`<br/>
  Resets all slave modules the hard way.
  Only the _PowerManagement_ and the _DiWheelDrive_ are not defined as slave modules.


Pull-up/down Configuration
--------------------------

All control signals are designed for open-drain access and thus are passively pulled up or down to a specific voltage:

| signal           | pull-up/down voltage | logically active | default state   |
|:-----------------|:--------------------:|:----------------:|:---------------:|
| `SYS_COLDRST_N`  | `VDD` (3.3 V)        | low              | inactive (high) |
| `SYS_PD_N`       | `VIO3.3` (3.3 V)     | low              | inactive (high) |
| `SYS_REG_EN`     | `GND` (0 V)          | high             | inactive (low)  |
| `SYS_SYNC_N`     | `VIO3.3` (3.3 V)     | low              | inactive (high) |
| `SYS_UART_UP/DN` | `VDD` (3.3 V)        | low              | inactive (high) |
| `SYS_WARMRST_N`  | `VIO3.3` (3.3 V)     | low              | inactive (high) |

This design allows to use the active state of each signal as logical OR (one or more AMs have activated the signal), and the inactive state as logical AND (no AM has activated the signal).
It is very important to note the different pull-up voltages.
While `VDD` is available at any time (even when the system is turned off), `VIO3.3` is enabled/disabled via `SYS_REG_EN`.
Consequently, the signals `SYS_PD_N`, `SYS_SYNC_N`, and `SYS_WARMRST_N` can only be low (or undefined) as long as `SYS_REG_EN` is inactive.
When activated, `SYS_REG_EN` is pushed to `VDD` (3.3V).


Read/Write Permissions
----------------------

Whereas some signals are bus-like and can be read and written from any AM, others are read only except for a single master:

|                   | `SYS_COLDRST_N` | `SYS_PD_N`   | `SYS_REG_EN` | `SYS_SYNC_N` | `SYS_UART_UP/DN` | `SYS_WARMRST_N` |
|:------------------|:---------------:|:------------:|:------------:|:------------:|:----------------:|:---------------:|
| _LightRing_       | read only       | read & write | read only    | read & write | read & write     | read only       |
| extension modules | read only       | read & write | read only    | read & write | read & write     | read only       |
| _PowerManagement_ | read & write    | read & write | read & write | read & write | read & write     | read & write    |
| _DiWheelDrive_    | 	read only     | read & write | read only    | read & write | read & write     | read only       |

