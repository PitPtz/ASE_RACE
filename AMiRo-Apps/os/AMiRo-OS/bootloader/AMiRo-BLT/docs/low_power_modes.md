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

Low-Power Modes
===============

Three low-power modes that can be entered after system shutdown have been defined for the AMiRo system so far.


Hibernate
---------

Most of the time the system will stay in standby and thus draw as little power as possible.
It wakes up periodically, though, and measures the system supply voltage.
If it is found to be too low to charge the batteries, the system will enter standby again, or enable charging otherwise.

The main feature of this low-power mode is the capability to charge via the charging pins of the _DiWheelDrive_ using a station.
The drawback is, that both modules, _PowerManagement_ and _DiWheelDrive_, need to start up and stay awake for about 50 to 100 milliseconds for each voltage measurement.
Even though the rest of the robot is still turned off, this periodical power draw massively reduces the time the system can be stored without charging.


DeepSleep
---------

When the system entered DeepSleep mode, it will only wake up if one of the following events occurs:

- A power plug is inserted to the _PowerManagement_.
- The touch sensor fires an interrupt event.
- The acceleration sensor fires an interrupt event.
- Any RTC (real-time clock) event occurs on the _PowerManagement_ or the _DiWheelDrive_.
- An independent watchdog reset event occurs on one of the _PowerManagement_ or the _DiWheelDrive_.

In most cases the system will react by completely starting up, except when a power plug is detected, which makes it switch to hibernate mode.
This way the system usually draws as little power as possible and can still be charged using a power cord.
The worst case would be when a plug is detected but not powered, so that the system will wake up periodically (see Hibernate mode) and draw energy.


Transportation
--------------

Very similar to DeepSleep mode, the system enters standby for maximum power saving.
However, the aforementioned events, which were used to wake the system again, are completely deactivated in this mode.
This is particularly useful during transportation of the robot, so shaking for instance will not wake the system.
The drawbacks, however, are the missing possibility to charge the system and that the only way to wake it up again is by activating the `SYS_COLDRST_N` signal (see [signals documentation](./system_signals.md)) via a programming cable.

