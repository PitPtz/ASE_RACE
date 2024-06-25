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
- Georg Alberding
- Cung Sang
- Svenja Kenneweg (skenneweg[at]techfak.uni-bielefeld.de)
- Marc Rothmann
- Aleksandrs Stier
- Simon Welzel
- Felix Wittenfeld
- Robin Ewers

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

[AMiRo-BLT]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-BLT
[AMiRo-LLD]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-LLD
[SSSP]: https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp
[ChibiOS]: http://www.chibios.org
[OpenOCD]: http://openocd.org/

--------------------------------------------------------------------------------

Contents
========

1. [Introduction](#1-introduction)
2. [Required Software](#2-required-software)
    1. [Git](#21-git)
    2. [Bootloader & Tools (AMiRo-BLT)](#22-bootloader-tools-amiro-blt)
    3. [System Kernel (ChibiOS)](#23-system-kernel-chibios)
    4. [Low-Level Drivers (AMiRo-LLD)](#24-low-level-drivers-amiro-lld)
    5. [OpenOCD](#25-openocd)
3. [Recommended Software](#3-recommended-software)
    1. [gtkterm and hterm](#31-gtkterm-and-hterm)
    2. [PlantUML](#32-plantuml)
    3. [Doxygen & Graphviz](#33-doxygen-graphviz)
    4. [QtCreator IDE](#34-qtcreator-ide)
4. [File Structure](#4-file-structure)
    1. [project root](#41-project-root)
    2. [bootloader/](#42-bootloader)
    3. [core/](#43-core)
    4. [docs/](#44-docs)
    5. [kernel/](#45-kernel)
    6. [modules/](#46-modules)
    7. [periphery-lld/](#47-periphery-lld)
    8. [test/](#48-test)
    9. [tools/](#49-tools)
5. [Building and Flashing](#5-building-and-flashing)
6. [User Interface](#6-user-interface)
    1. [Connecting via Serial Terminal](#61-connecting-via-serial-terminal)
    2. [Using the Shell](#62-using-the-shell)
7. [Developer Guides](#7-developer-guides)
    1. [Adding a Module](#71-adding-a-module)
    2. [Adding a Shell Command](#72-adding-a-shell-command)
    3. [Handling a Custom I/O Event in the Main Thread](#73-handling-a-custom-io-event-in-the-main-thread)
    4. [Implementing a Low-Level Driver](#74-implementing-a-low-level-driver)
    5. [Writing a Test](#75-writing-a-test)
8. [FAQ](#8-faq)

--------------------------------------------------------------------------------

1 - Introduction
================

AMiRo-OS is an real-time operating system originally developed for the base modules of the Autonomous Mini Robot (AMiRo) [1].
It utilizes ChibiOS (a real-time operating system for embedded devices developed by Giovanni di Sirio; cf. <http://chibios.org>) as system kernel and extends it with platform specific configurations and further functionalities and abstractions.
It also incorporates further projects - [AMiRo-BLT] and [AMiRo-LLD] - and implements the [Startup Shutdown Synchronization Protocol (SSSP)][SSSP].



2 - Required Software
=====================

In order to compile the source code, you need to install the GNU ARM Embedded Toolchain.
Since this project uses GNU Make for configuring and calling the compiler, this tool is required too. AMiRo-OS uses ChibiOS as system kernel, so you need a copy of that project as well.


2.1 - Git
---------

Since all main- and subprojects are available as Git repositories, installing a recent version of the tool is mandatory.
Most Linux distributions like Ubuntu provide a sufficient version in their software repositories.


2.2 - Bootloader & Tools (AMiRo-BLT)
------------------------------------

AMiRo-OS can take advantage of an installed bootloader and provides an according interface.
By default, [AMiRo-BLT] is included as Git submodule and can easily be initialized via the provided setup script.
Simply run

```sh
>$ ./setup.sh
```
from a command line.

If required, it is possible to replace the used bootloader by adding an according subfolder in the [`./bootloader/`](./bootloader/) directory.
Note that you will have to adapt the makefiles and scripts, and probably the operating system as well.

[AMiRo-BLT] furthermore has its own required and recommended software & tools as described in its own [`README.md`](./bootloader/AMiRo-BLT/README.md) file.
Follow the instructions to initialize the development environment manually or use the setup script.


2.3 - System Kernel (ChibiOS)
-----------------------------

Since AMiRo-OS uses [ChibiOS] as underlying system kernel, you need to acquire a copy of it as well.
For the sake of compatibility, it is included in AMiRo-OS as Git submodule.
It is highly recommended to use the setup script for initialization.
Moreover, you have to apply the [patches](./kernel/patches/) to [ChibiOS](./kernel/ChibiOS/) in order to make AMiRo-OS work properly.
It is recommended to use the setup script for this purpose as well.

If you would like to use a different kernel, you can add an according subfolder in the [`./kernel/`](./kernel/) directory and adapt the scripts and operating system source code.


2.4 - Low-Level Drivers (AMiRo-LLD)
-----------------------------------

Any required low-level drivers for the AMiRo hardware are available in an additional project: [AMiRo-LLD].
It is included as Git submodule and can be initialized via the setup script.
Since [AMiRo-LLD] is also used for experimentation and prototyping, it contains drivers even for some hardware that is not available on the AMiRo platform.


2.5 - OpenOCD
-------------

When running AMiRo-OS on non-AMiRo modules (e.g. NUCLEO development boards), those can be flashed using the OpenOCD toolchain (<http://openocd.org/>).
It can be either installed from the software repositories of your operating system (requires root permissions) or built from source (no root required).

For a list of supported boards, please refer to the [OpenOCD] documentation.



3 - Recommended Software
========================

The software tools named in this section are not essential for simply using or further development of AMiRo-OS, but are useful in both scenarios.


3.1 - gtkterm and hterm
-----------------------

Depending on your operating system, it is recommended to install 'gtkterm' for Linux (available in the Ubuntu repositories), or 'hterm' for Windows.


3.2 - PlantUML
--------------

PlantUML is a free and open source Java tool to generate UML diagrams via scrips (see <https://plantuml.com>).
AMiRo-OS provides according scripts in the [`./docs/`](./docs/) directory.
Please refer to the PlantUML documentation for how to generate figures from these script files.


3.3 - Doxygen & Graphviz
------------------------

In order to generate the documentation from the source code, Doxygen and Graphviz are required.
It is recommended to install these tools using the default versions for your system. Ubuntu users should simply run

```sh
>$ sudo apt-get install doxygen graphviz
```


3.4 - QtCreator IDE
-------------------

AMiRo-OS provides support for the QtCreator IDE.
In order to setup according projects, use the setup script and follow the instructions.
It will automatically generate the required files and you can import the projects by opening the `.creator` files with QtCreator IDE.

Further instructions for a more advanced configuration of the IDE are provided in a dedicated [documentation](./docs/QtCreator_configuration.md).



4 - File Structure
==================

AMiRo-OS is divided into several logical parts, which are located in individual directories.
While the folders [`./bootloader/`](./bootloader/), [`./kernel/`](./kernel/) and [`./periphery-lld/`](./periphery-lld/) contain other projects as Git submodules, the actual source code of AMiRo-OS is located in the folders [`./core/`](./core/) and [`./modules/`](./modules/).
Further directories contain documentation ([`./docs/`](./docs/)), a test framework ([`./test/`](./test/)) and useful tools ([`./tools/`](./tools/)).

4.1 - [project root](./)
------------------------

The project root directory contains [this file](./README.md), a [license file](./license.html), an interface header file ([`amiroos.h`](./amiroos.h)), make scripts as well as the [setup.sh](./setup.sh) bash script.
While the purpose of the former two is obvious, the make scripts can be used to compile code for any supported module from this directory.
Most importantly, however, the [setup script](./setup.sh) provides an easy to use tool for initialization and managing of the AMiRo-OS project and its dependencies.


4.2 - [bootloader/](./bootloader/)
----------------------------------

This directory contains the [AMiRo-BLT] Git submodule as well as further interface files.


4.3 - [core/](./core/)
----------------------

The actual source code of AMiRo-OS can be found in this directory.
Whilst all C header files are located in the [`./core/inc/`](./core/inc/) folder, `.c` source files are placed in [`./core/src/`](./core/src/).


4.4 - [docs/](./docs/)
----------------------

This directory contains some further documentation of AMiRo-OS.


4.5 - [kernel/](./kernel/)
--------------------------

This directory contains the [ChibiOS] Git submodule as well as further interface files.
Furthermore, there is a [`./kernel/patches/`](./kernel/patches/) folder, which contains several patches, which need to be applied to [ChibiOS].
Instead of applying the patches manually, it is highly recommended to use the [setup script](./setup.sh) in the [root directory](./) for this purpose.
Further details about the patches are given in a [dedicated document](./kernel/patches/README.md).


4.6 - [modules/](./modules/)
----------------------------

This directory contains all code, which is not part of the general AMiRo-OS but specific to individual modules.
Thus, there several subfolders, one for each supported device.


4.7 - [periphery-lld/](./periphery-lld/)
----------------------------------------

This directory contains the [AMiRo-LLD] Git submodule as well as further interface files.


4.8 - [test/](./test/)
----------------------

AMiRo-OS provides a simplistic test framework for validation of software and handware.
Since such tests themselves are not part of AMiRo-OS core functionality, those are located in this dedicated folder.


4.9 - [tools/](./tools/)
------------------------

This directory contains several useful tools, e.g. for setting up IDE projects.
For convenience, all these tools can be accessed via the [setup script](./setup.sh) in the AMiRo-OS [root directory](./).



5 - Building and Flashing
=========================

Each time you modify any part of AMiRo-OS, you need to recompile the whole project for the according module.
Therefore you can use the [`root make script`](./Makefile) by simply executing make in the [AMiRo-OS root directory](./) and follow the instructions.
Alternatively, you can call make in the [`./modules/`](./modules/) folder, which acts very similar, or in a specific module folder (e.g. [`./modules/DiWheelDrive_1-1/`](./modules/DiWheelDrive_1-1/)).

After the build process has finished successfully, you always have to flash the generated program to the module.
Therefore you need an appropriate tool, such as SerialBoot for the AMiRo base modules (provided by [AMiRo-BLT]) or [OpenOCD].
Similar to the compilation procedure as described above, you can flash either each module individually, or all modules at once by using the same makefiles.

When using SerialBoot, please note that you must connect the programming cable to either the _DiWheelDrive_ or the _PowerManagement_ for flashing the operating system.
All other modules are powered off after reset so that only these two offer a running bootloader, which is required for flashing.

As already described for the serial connection via gtkterm or hterm, all scripts use the first matching port (i.e. `/dev/ttyAMiRo0`, `/dev/ttyUSB0` or `/dev/ttyACM0` respectively) by default.
If you need to specify a custom port, you can do so by passing an additional argument to the make call:

```sh
>$ make flash FLASH_PORT=/dev/ttyAMiRo#
```

where you have to replace the trailing `#` with the according integer value.



6 - User Interface
==================

In order to interact with AMiRo-OS, you can connect to the system shell via a serial terminal.
This section first describes how to connect to the servial interface, before giving some advice on how to use the AMiRo-OS shell efficiently.


6.1 - Connecting via Serial Terminal
------------------------------------

The following explanation applies to the 'gtkterm' serial terminal.
For alternative tools, the procedure is similar, but will differ in application specific details, obviously.

In order to get started, you need to modify the configuration file `~/.gtktermrc` (generated automatically when you start the application for the first time).
For AMiRo modules the configuration is:

```
[AMiRo]
port = /dev/ttyAMiRo0
speed = 115200
bits = 8
stopbits = 1
parity = none
flow = none
wait_delay = 0
wait_char = -1
rs485_rts_time_before_tx = 30
rs485_rts_time_after_tx = 30
echo = False
crlfauto = True
```

If you are using an old version of [AMiRo-BLT], the `/dev/ttyAMiRo0` device might not be available.
In order to enable legacy support, replace the port value by `/dev/ttyUSB0`.

For NUCLEO boards the according configuration is similar:

```
[NUCLEO]
port = /dev/ttyACM0
speed = 115200
bits = 8
stopbits = 1
parity = none
flow = none
wait_delay = 0
wait_char = -1
rs485_rts_time_before_tx = 30
rs485_rts_time_after_tx = 30
echo = False
crlfauto = True
```

When running gtkterm from the command line, you can select a defined configuration via the `-c` option:

```sh
>$ gtkterm -c AMiRo
>$ gtkterm -c NUCLEO
```

The configurations above use the first matching port (i.e. `/dev/ttyAMiRo0`, `/dev/ttyUSB0` or `/dev/ttyACM0` respectively).
If you have connected multiple devices to your system, those will be listed with increasing numbers in their identifiers.
Furthermore, other USB devices (possibly internal components) might be listed as such as well.
In those cases you can either adapt the configuration, or specify the correct port manually.
For gtkterm the according call on the command line would be:

```sh
>$ gtkterm -c AMiRo -p=/dev/ttyAMiRo#
```

where you have to replace the trailing `#` with the according integer value.
You can easily list all connected devices via

```sh
>$ ls /dev/ | grep -E "^tty(USB|AMiRo|ACM)[0-9]+$"
```

Note that those interfaces are ordered by the time when they have been detected by the operating system, so detaching a cable and plugging it in again may result in a different port name.

When connecting to an AMiRo module, note that gtkterm sets the RTS signal to active on startup.
On the AMiRo hardware, this signal is used to reset the whole robot.
Hence, the module/robot will startup as soon as you deactivate RTS.

You can turn off the hardware at any time by simply activating RTS again.
As soon as you deactivate it again, the system will boot from scratch.
If you want connect to a module without reseting it, you should start gtkterm before connecting the cable to the AMiRo hardware, deactivate RTS and only then plug the cable in.


6.2 - Using the Shell
---------------------

The AMiRo-OS shell is a central component for a user to interact with the system.
Although it can be deactivated via the `aosconf.h` configuration file, the system shell is typically available on every platform.
Similar to other shells, you can navigate the cursor using the 'arrow left', 'arrow right', 'home' and 'end' keys.
You can also delete text using backspace or delete.

The shell also features a history, the size of which is configured via the `AMIROOS_CFG_SHELL_HISTLENGTH` setting in `aosconf.h`.
Even when set to zero, the shell can retrieve the most recent call with certain caveats.
Intuitively, the history can be scrolled through via the 'arrow up', 'arrow down', 'page up' and 'page down' keys.

Furthermore, the AMiRo-OS shell features auto-complete and suggest capabilities.
When pressing the 'tab' key once, the current input (at the current cursor position) is completed as possible.
Pressing 'tab' again will print all commands that are ambiguous to the current input.
This behaviour can be used to print all commands available by pressing 'tab' twice when there is not input at all.

The following table provides a comprehensive overview of all available key combinations (cheat sheet):

| key combination     | action                                                          |
|:-------------------:|:----------------------------------------------------------------|
| backspace           | delete left                                                     |
| delete              | delete right                                                    |
| insert              | toggle insert/overwrite                                         |
| enter               | execute command                                                 |
| arrow left          | move cursor one character left                                  |
| arrow right         | move cursor one character right                                 |
| ctrl + arrow left   | move cursor one word left                                       |
| ctrl + arrow right  | move cursor one word right                                      |
| home                | move cursor to the very left                                    |
| end                 | move cursor to the very right                                   |
| arrow up            | recall previous entry from history                              |
| arrow down          | recall subsequent entry from history or clear the current input |
| ctrl + arrow up     | recall oldest entry from history                                |
| ctrl + arrow down   | recall or clear the current input                               |
| page up             | recall oldest entry from history                                |
| page down           | recall or clear the current input                               |
| tab (1st time)      | auto-complete current input                                     |
| tab (consecutively) | suggest matching commands                                       |



7 - Developer Guides
====================

Due to the complexity of AMiRo-OS it can be quite troublesome to get started with the framework.
The guides in this chapter will help you getting things done, without thorough knowledge of the software structure.
Whereas the textual descriptions of the guides provide in-depth information about the underlying concepts and mechanisms, a short summary is provided at the end of each chapter.


7.1 - Adding a Module
---------------------

The very first thing to do when adding a new module to support AMiRo-OS, is to create an according folder in the [`./modules/`](./modules/) directory.
The name of this folder should be as unambiguous as possible (e.g. containing name and version number).
All files, which directly depend on the hardware, and thus are not portable, belong here.
Conversely, any code that can be reused on different hardware should not be placed in this module folder.

In a second step you have to initialize all required files (see below) in the newly created module directory.
It is recommended to use an existing module as template for your configuration:

- `alldconf.h`<br/>
  Configuration header for the [AMiRo-LLD] project, which is part of AMiRo-OS.
  There are probably only very few configurations done here, since most settings depend on the content of `aosconf.h` and are handled module unspecifically in the [`./modules/aos_alldconf.h`](./modules/aos_alldconf.h) file.
- `aosconf.h`<br/>
  Configuration header for the AMiRo-OS project.
  Existing configuration files are well documented and name all available settings.
- `board.h` & `board.c`<br/>
  Contains definitions of GPIO names and initialization settings of those, as well as initialization functions.
  These configurations highly depend on the hardware setup.
  For further information, please refer to the [ChibiOS] documentation.
  [ChibiOS] also provides demo code for various devices.
- `chconf.h`<br/>
  Configuration header for the [ChibiOS]/RT system kernel.
  There are probably only very few configurations done here, since most settings depend on the content of `aosconf.h` and are handled module unspecifically in the [`./modules/aos_chconf.h`](./modules/aos_chconf.h) file.
- `halconf.h`<br/>
  Configuration header for [ChibiOS]/HAL (hardware abstraction layer).
  Existing files are well documented and name all available settings.
  Please refer to [ChibiOS] for further details.
- `Makefile`<br/>
  The GNU make script to build and flash AMiRo-OS for the module.
- `mcuconf.h`<br/>
  Configuration file for [ChibiOS]/HAL to initialize the microcontroller (MCU).
  It is recommended to check the [`./kernel/ChibiOS/demos/`](./kernel/ChibiOS/demos/) directory for an example using the according MCU and copy the `mcuconf.h` from there.
  Depending on your hardware setup you may have to modify it nevertheless, though.
- `module.h` & `module.c`<br/>
  These files act as some sort of container, where all module specific aliases for interfaces and GPIOs, configurations, hooks, low-level drivers, and tests are defined.
  These are the most comprehensive files in the module folder.
- `<mcu>.ld`<br/>
  Linker script, defining the memory layout and region aliases for the MCU.
  It is recommended to check [ChibiOS] ([`./kernel/ChibiOS/os/common/startup/`](./kernel/ChibiOS/os/common/startup/)) whether a linker script for the according MCU already exists.

Since all these files are specific to the module hardware, you will have to modify the contents according to your setup in a third step.
Most settings are described in detail within the configuration files, but for others you will have to consult the datasheet of your MCU and even take a closer look at how certain settings are used in other modules.

Finally, you need to build and flash the project.
The compiler might even help you getting everything set up correctly.
Take the time needed to understand compilation errors and warnings and get rid of all of those.
Warnings should not be ignored since they are hints that something might be amiss and the program will not act as intended.

As you will probably notice, for most modules there is an additional `test/` folder.
This folder contains module specific wrapper code for tests (e.g. for hardware devices).
Since tests are not essential but a more advanced feature, a separate guide describes how to write a test in section 7.5.

**Summing up, you have to**

1. create a module directory.
2. initialize all files (use an existing module or a ChibiOS demo as template).
3. configure all files according to your hardware setup and preferences.
4. compile, flash and check for issues.


7.2 - Adding a Shell Command
----------------------------

Before going into the technical details, how a new shell command is initialized and registered to a shell, some basic concepts of the AMiRo-OS shell should be covered first.
Most fundamentally, although for most use cases a single shell instance on a module will suffice, there can be an arbitrary number of shells.
Each shell runs in its own thread but accesses a common list of shell commands.
That said, each shell command is registered module-wide and can be accessed by all shells.

Another important aspect of the AMiRo-OS shell are I/O streams.
Each shell reads and writes from/to a shell stream.
Such a stream may again contain an arbitrary number of channels.
Whilst only one of those channels can be selected as input, each and all channels can be configured as output.
As a result, if a hardware module features multiple I/O interfaces, according configuration of the shell stream and its channels allows to still use only a single shell instance.
If not disabled in the `aosconf.h` file, AMiRo-OS already runs a system shell in a low-priority thread.

Depending on the configuration, several commands are registered to the system shell by default (e.g. `kernel:test`, `module:info`), which are defined in the AMiRo-OS core.
In order to add additional custom commands, those should be defined in the `module.h` and `module.c` files.
First you need to _declare_ the shell command - an instance of the memory structure representing a command - in the `module.h` file.
Second, you have to _define_ that structure in the `module.c` file via the `AOS_SHELL_COMMAND(var, name, callback)` macro function.
This macro takes three arguments:

1. `var`<br/>
   Name of the variable (must be identical to the _declaration_).
2. `name`<br/>
   Command string which will be shown and used in the shell.
   By convention, command names follow a colon notation, e.g. `module:info`, where the first part denotes the scope of the command (e.g. kernel, module, tests, etc.) and the second part specifies the command in this scope.
3. `callback`<br/>
   Callback function to be executed by the command.

The callback function is typically defined right before the `AOS_SHELL_COMMAND()` macro is called and should be a mere wrapper, calling another function.
Keep in mind, though, that those callbacks are executed within the shell thread and thus inherit its (typically very low) priority and there is no way to call a command in a non-blocking manner.
If you want to use a shell command to start/stop other components in your system, those should be implemented in dedicated threads and the shell command callbacks have to use inter process communication to manage and interface those components.

Finally, you have to register the command.
This is very important and a common mistake, but naturally a shell can only access commands, which are known to it.
Registration is done via the `aosShellAddCommand()` function, preferably before the shell thread is started.
Since test commands are the most common use case, AMiRo-OS provides the hook `MODULE_INIT_TESTS()`, which is defined in each `module.h` file.

**Summing up, you have to**

1. declare and define a command.
2. implement a callback function.
3. register the command.


7.3 - Handling a Custom I/O Event in the Main Thread
----------------------------------------------------

In order to handle custom I/O events in the main thread, AMiRo-OS offers several hooks to be used.
First of all, you need to configure and enable the interrupt for the according GPIO.
This can be done by implementing the `MODULE_INIT_INTERRUPTS()` hook in the `module.h` file.
For information how to use this hook, please have a look at existing modules.
In the end, the according interrupt callback function has to emit an I/O event with the according bit in the flags mask set (such as the `_gpioCallback()` function in [`./core/src/aos_system.c`](./core/src/aos_system.c)).
As result, whenever a rising or falling edge (depends on configuration) is detected on that particular GPIO, the interrupt service routine is executed and hence an I/O event is emitted, which can be received by any thread in the system.

Next, you have to explicitly whitelist the event flag for the main thread, because it ignores all I/O events other than power down and such by default.
This is done via the optional `AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK` macro, which should be defined in the `module.h` file, for example:

```c
#define AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK \
        (AOS_GPIOEVENT_FLAG(padX) | AOS_GPIOEVENT_FLAG(padY) | AOS_GPIOEVENT_FLAG(padZ))
```

When `AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK` has been defined correctly, the main thread will be notified by the according events and execute its event handling routine.
Hence you have to implement another macro in `module.h` to handle the custom event(s) appropriately: `MODULE_MAIN_LOOP_GPIOEVENT(eventflags)`.
As you can see, the variable `eventflags` is propagated to the hook.
This variable is a mask, that allows to identify the GPIO pad(s), which caused the event, by the individually set bits.
Following the example above, you can check which GPIOs have caused events by using if-clauses in the implementation of the hook:

```c
#define MODULE_MAIN_LOOP_GPIOEVENT(eventflags) {                              \
  if (eventflags & AOS_GPIOEVENT_FLAG(padX)) {                                \
    /* handle event */                                                        \
  }                                                                           \
  if (eventflags & (AOS_IOEVENT_FLAG(padY) | AOS_GPIOEVENT_FLAG(padZ))) {     \
    /* handle combined event */                                               \
  }                                                                           \
}
```

**Summing up, you have to**

1. configure and enable the GPIO interrupt.
2. define the `AMIROOS_CFG_MAIN_LOOP_GPIOEVENT_FLAGSMASK` macro.
3. implement the `MODULE_MAIN_LOOP_GPIOEVENT(eventflags)` hook.


7.4 - Implementing a Low-Level Driver
-------------------------------------

In the AMiRo-OS framework, low-level drivers are located in the additional Git project [AMiRo-LLD], which is included in AMiRo-OS as Git submodule at [`./periphery-lld/AMiRo-LLD/`](./periphery-lld/AMiRo-LLD/) and acts similar to a static library.
When adding a new low-level driver to the framework, you first have to implement it of course.
For details how to do so, please follow the instructions given in the [`README.md`](./periphery-lld/AMiRo-LLD/README.md) file in the [AMiRo-LLD root directory](./periphery-lld/AMiRo-LLD/).

Now that the new driver is available, it can be enabled by simply including the driver's make script in the `Makefile` of the module, you are working on.
In order to make actual use of the driver, you have to add according memory structures to the `module.h` and `module.c` files - just have a look at existing modules how this is done.
In some cases you will have to configure additional interrupts and/or alter the configuration of a communication interface (e.g. I2C).
Once again, you should take a look at existing modules and search the `module.h` for the hooks `MODULE_INIT_INTERRUPTS()`, `MODULE_INIT_PERIPHERY_INTERFACES()` and `MODULE_SHUTDOWN_PERIPHERY_INTERFACES()`.

Finally, you will probably want to validate your implementation via a test.
How this can be done is explained in detail in the next guide.

**Summing up, you have to**

1. implement the driver in AMiRo-LLD using periphAL only.
2. add the driver to a module (`Makefile`, `module.h` and `module.c`).
3. configure interrupts and interfaces as required.
4. write a test to verify your setup.


7.5 - Writing a Test
--------------------

AMiRo-OS provides a test framework for convenient testing and the ability to opt-out all tests via the `aosconf.h` configuration file.
There is also a dedicated [`./test/`](./test/) folder, where all test code belongs to.
In case you want to implement a test for a newly developed low-level driver, you should have a look at the folder [`./test/periphery-lld/`](./test/periphery-lld/).
As with the low-level drivers, tests are placed in individual subfolders (e.g. `./test/periphery-lld/DEVICE1234_v1`) and all files should use the prefix `aos_test_` in their name.
Moreover, all code must be fenced by guards that disable it completely if the `AMIROOS_CFG_TESTS_ENABLE` flag is set to false in the `aosconf.h` configuration file.

Now you have to add the test to a specific module.
Therefore, you should create a `test/` directory in the module folder, if such does not exist yet.
In this directory, you create another subfolder, e.g. `DEVICE1234/`, and three additional files in there:

- `module_test_DEVICE1234.mk`
- `module_test_DEVICE1234.h`
- `module_test_DEVICE1234.c`

The make script is not required, but recommended to achieve maintainable code.
This script should add the folder to the `MODULE_INC` variable and all C source files to `MODULE_CSRC`.
The header and source files furthermore define module specific data structures and a test function.
In order to clearly indicate that these files are module specific wrappers, their names should begin with the `module_test_` prefix.

In order to be able to call such test function as a command via the AMiRo-OS shell, you need to add an according shell command to the `module.h` and `module.c` files.
While the command itself is typically very simple, just calling the callback function defined in the `./test/DEVICE1234/module_test_DEVICE1234.h`/`.c` files, you must not forget to register it, so it can be access by a shell.
AMiRo-OS provides the hook `MODULE_INIT_TESTS()` for this purpose, which has to be implemented in the `module.h` file.
Once again it is recommended to have a look at an existing module, how to use this hook.
Furthermore, there is a more detailed guide on adding shell commands.

**Summing up, you have to**

1. implement the common test in the `./test/` folder.
2. implement a module specific wrapper in the `./modules/<module>/test/` folder.
3. register the shell command via the `MODULE_INIT_TESTS()` hook in `module.h`.


8 - FAQ
====================
