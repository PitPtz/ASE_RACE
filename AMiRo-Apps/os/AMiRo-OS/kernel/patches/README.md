About & License
===============

AMiRo-OS is an operating system for the base version of the Autonomous Mini
Robot (AMiRo) [1]. It utilizes ChibiOS (a real-time operating system for
embedded devices developed by Giovanni di Sirio; see <http://chibios.org>) as
system kernel and extends it with platform specific configurations and further
functionalities and abstractions.

Copyright (C) 2016..2022  Thomas Schöpping et al. (a complete list of all
authors is given below)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU (Lesser) General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.

This research/work was supported by the Cluster of Excellence Cognitive
Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is
funded by the German Research Foundation (DFG).

Authors
-------

- Thomas Schöpping (tschoepp[at]cit-ec.uni-bielefeld.de)

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

[ChibiOS]: http://www.chibios.org

--------------------------------------------------------------------------------

Patches
=======

AMiRo-OS comes with several patches for the [ChibiOS] kernel, which are briefly described in this file.

[0001_I2C-without-DMA.patch](./0001_I2C-without-DMA.patch) 
----------------------------------------------------------

Enhances the STM32 I2Cv1 LLD to support a non-DMA mode.
Implementation is straight-forward according to reference manual (ST RM0008 Reference Manual for STM32F101xx, STM32F102xx, STM32F103xx, STM32F105xx and STM32F107xx advanced Arm®-based 32-bit MCUs)

[0002_QEI-driver.patch](./0002_QEI-driver.patch)
------------------------------------------------

Introduces QEI functionality to the STM32 TIMv1 LLD.
This patch is cherry picked from the [ChibiOS Contribution project](https://github.com/ChibiOS/ChibiOS-Contrib).

[0003_chprintf-enhanced.patch](./0003_chprintf-enhanced.patch)
--------------------------------------------------------------

Enhanced the `chprintf()` method (and its derivatives) by two features:

- Special IEEE 754 floating point values result in according strings "(inf)", "(nan)" and "(err)".
  Without this patch, `chprintf()` would try to interprete special values as regular ones.
- Introduces support for 64 bit integer values (`long long`).
  The feature is disabled by default and can be enabled via `CHPRINTF_SUPPORT_LONGLONG` setting in `chconf.h`.

[0004_tests-with-fast-16bit-ST.patch](./0004_tests-with-fast-16bit-ST.patch)
----------------------------------------------------------------------------

The original issue was, that the [ChibiOS] test suite assumes, that `systime_t` can represent time frames of at least one second.
With fast ST frequencies (AMiRo-OS configures 1 MHz by default) but small timers (e.g. 16 bit) this is not the case.
As result, test suite benchmarks would output incorrect performance mentrics.
This patch hence enhances the [ChibiOS] test suite to work with small but fast ST timers (still assumes that at least 50 ms can be represented by `systime_t`).

[0005_introduce-thread-hierarchy.patch](./0005_introduce-thread-hierarchy.patch)
--------------------------------------------------------------------------------

This patch introduces a new feature: thread hierarchy.
All threads are ordered in a hierarchy, with the original main thread as root.
Each thread thus has exactly one parent (excpet for the root thread), an arbitrary number of children and an arbitrary number of siblings.
