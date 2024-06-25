About & License
===============

AMiRo-Apps is a collection of applications and configurations for the Autonomous
Mini Robot (AMiRo) [1]. It is tightly coupled to the AMiRo-OS and µRT projects,
which provide a sophisticated operating system (based on ChibiOS) and a
real-time middleware for embedded devices.

Copyright (C) 2018..2022 Thomas Schöpping et al. (a complete list of all authors
is given below)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU (Lesser) General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

This research/work was supported by the Cluster of Excellence Cognitive
Interaction Technology 'CITEC' (EXC 277) at Bielefeld University, which is
funded by the German Research Foundation (DFG).


Authors
-------

- Thomas Schöpping (tschoepp[at]cit-ec.uni-bielefeld.de)
- Svenja Kenneweg


References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

[ChibiOS]: https://www.chibios.org
[AMiRo-OS]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS
[µRT]: https://gitlab.ub.uni-bielefeld.de/AMiRo/uRtWare

--------------------------------------------------------------------------------

Contents
========

1. [Introduction](#1-introduction)
2. [File Structure](#2-file-structure)
3. [Operating Principle](#3-operating-principle)
4. [Create your own Configuration](#4-create-your-own-configuration)


--------------------------------------------------------------------------------

1 - Introduction
================

This file shall provide some helpful insights, how the HelloWorld configuration works.
For better understanding of the operating principles of the [µRT] it is highly recommended to read its [readme file](../../middleware/uRT/README.md).
Since the HelloWorld configuration utilizes the [HelloWorld application](../../apps/HelloWorld/), it is alos recommended to read its [readme file](../../apps/HelloWorld/README.md).


2 - File Structure
==================

The root folder of this configuration contains four files:

- **[HelloWorld_apps.h](./HelloWorld_apps.h)** and **[HelloWorld_apps.c](./HelloWorld_apps.c)**<br>
  These C files provide all logic to initialize the entire configuration at system startup, as well as data structures and functions for interaction of the node and with a user.
- **[Makefile](./Makefile)**<br>
  This GNU Make script is rather simple and primarily for convenience.
  It provides an interface to build and flash all supported modules of this configuration from a single file.
- **[README.md](./README.md)**<br>
  This Markdown file provides useful information on how the HelloWorld configuration works and shall empower you to develop your own configurations.
  
Furthermore, there is the [modules/](./modules/) directory, wich contains all module specific (or platform specific if you will) configurations and logic.
Up to this point, all logic and files are generic and can be used on any hardware, including the [HelloWorld application](../../apps/HelloWorld/).

The [modules/](./modules/) directory itslef contains three more files:

- **[HelloWorld_osconf.h](./HelloWorld_osconf.h)** and **[HelloWorld_urtconf.h](./HelloWorld_urtconf.h)**<br>
  These two C headers contain configuration information for the [AMiRo-OS] real-time operating system (including its underlying [ChibiOS] kernel) and the [µRT] middleware.
  While each module has to configure those individually, these files provide default configurations.
  Modules can still override these settings by their custom configuration files, though.
- **[modules.mk](./modules/modules.mk)**<br>
  This GNU Make script loads all required environments (e.g. operating system and middleware) and accumulates all include paths, C source files and C++ source files into three variables.
  The script should be called by the Makefile of each individual module.

Then, there are the multiple folders of specific hardware platforms, this configurations currently supports.
Tpically, the contents of each of those folders should be very similar.
However, if the individual modules shall execute very different applications, the contents may differ strongly.

In any case, the following three files must be present within each module folder:

- **osconf.h** and **urtconf.h**<br>
  Those files are expected by AMiRo-OS and µRT and must contain configuration information for those.
  In many cases, these files are rather empty, though, and just include the general configurations provided by [HelloWorld_osconf.h](./HelloWorld_osconf.h) and [HelloWorld_urtconf.h](./HelloWorld_urtconf.h).
- **Makefile**<br>
  This GNU Make script is the actual entry point when building the code of a module.
  It configures several compiler and build options, includes dependencies (e.g. environments and applications), accumulates include paths, C source files and C++ source files, and finally provides targets to build and flash the binary image.
  Typically, those targets should point to the appropriate Makefile in AMiRo-OS, so all further hardware specific particularities are handled there.


3 - Operating Principle
=======================

First of all, the entire logic of the HelloWorld configuration is located in the [HelloWorld_apps.c](./HelloWorld_apps.c) C source file.
As expected, the `helloworldAppsInit()` method initializes the configuration, including its events and timers, messages and topic for publish-subscribe communication, the two nodes (master and slave) and eventually adds two commands to the AMiRo-OS system shell.
These shell commands are named "HelloWorld:PubSub" and "HelloWorld:RPC" and execute the callback functions `_helloworld_pubsubshellcb()` and `_helloworld_rpcshellcb()` respectively.
Since those two methods are very similar in their structure, only the former is described in detail by this guide.

The `_helloworld_pubsubshellcb()` function takes three arguments: a stream for printing messages, the number of argument strings and the according list of strings.
Except for the stream, the latter two arguments should be familiar from other command line applications.
In the first few lines, the arguments are checked and a help message (instructions) is printed to the stream.
Only then the arguments are parsed and the user defined duration and frequency are stored.
Next, the executing thread - the shell thread that is - temporarily registers to the termination event and saves some profiling data.
Two timers are started: one to periodically trigger the interaction between master and slave node and the user defined frequency, and another to terminate that interaction after the user defined duration.
With those timers armed, the master node will be triggered periodically and execute its loop callback, so the shell thread is done for now and waits for the termination event.
As soon as the interaction has been terminated, the shell thread unregisters from the termination event, checks whether the received event matches the expected one and prints a bunch of profiling information.

Before examining how the `helloworldAppsInit()` is actually "induced" to the system and called during startup, let us have a brief look at callback function for the two timers.
`_helloworld_triggercb()` is executed every time the periodic timer fires and just broadcasts the trigger event.
`_helloworld_terminationcb()` is executed when the termination timer expires and thus resets/halts the periodic timer and broadcasts the termination event.
Note the suffix 'I' of all functions that are called from within those callbacks.
Following a convention of ChibiOS, only functions suffixed by an 'I' or an 'X' are safe to be called from within ISR (interrupt service routine) context.
Even though most details should be of no further concern at this point, the distinction betwenn ISR context and thread context is important in two ways (please refer to the AMiRo-OS and ChibiOS documentation for more information):

- Functions with an 'I' suffix must only be called from (locked) ISR context.
  Functions with an 'S' suffix must only be called from locked thread context.
  Functions with an 'X' suffic may be called from any locked context.
  All functions without any suffix can be used in both contexts, but that rule may not apply, if any of the aforementioned suffixed functions are called internally.
- While it is valid to call blocking functions (e.g. I/O operations) from thread context, such must be omitted for ISRs.
  The rational behind this is, that idle threads (e.g. waiting for an I/O operation to complete) can be scheduled and another thread can be executed in the meantime.
  ISRs, however will always occupy the CPU until they return.
  This not only wastes processing time, but may also prevent other ISRs (e.g. timers, I/O) to be executed in time.

Now that all logic for initialization, user interaction and the nodes is ready, the final step is to "induce" that code to the operating system.
This is done via several hooks, which are provided by AMiRo-OS and configured in [HelloWorld_osconf.h](./HelloWorld_osconf.h):

- Via `AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER` a C header file is defined to be included in the [main.cpp](../../os/AMiRo-OS/core/src/main.cpp) of AMiRo-OS - the entry point of the entire system.
- `AMIROOS_CFG_MAIN_INIT_HOOK_2` and `AMIROOS_CFG_MAIN_INIT_HOOK_3` are two of several hooks to induce code in the initialization sequence of AMiRo-OS.
  The former initializes the µRT core and the HelloWorld configuration.
  The latter registers to the emergency event of µRT (in case any HRT timing constraints are violated) and starts all µRT nodes.
- Similarly, code is iduced to the shutdown sequence of AMiRo-OS via the `AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1` hook.
  In this case, µRT is shut down and the system main thread unregisters from the emergency event.


4 - Create Your Own Configuration
==================================

In order to create a new configuration, you should proceed according to the folloowing steps:

1. Initialize a new configuration folder with the most fundamental files and folders (GNU Make script, C interface and at least one module) containing only very basic logic.
2. Setup initialization and shutdown code to "induce" the configuration to the operating system.
3. Add applications to the configuration and add data structures and logic to establish interation between nodes (e.g. topics for publish-subscribe communciation) and for user interaction (e.g. shell commands) as required.
4. If your configuration is to support multiple platforms, repeat those steps for each module.
