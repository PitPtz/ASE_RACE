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

Contents
========

1. [Introduction](#1-introduction)
2. [Required Software](#2-required-software)
    1. [Git](#21-git)
    2. [GNU Make](#22-gnu-make)
    3. [GCC](#23-gcc)
    4. [stm32flash](#24-stm32flash)
    5. [GNU ARM Embedded Toolchain](#25-gnu-arm-embedded-toolchain)
    6. [CMake](#26-cmake)
3. [Recommended Software](#3-recommended-software)
    1. [PlantUML](#31-plantuml)
    2. [QtCreator IDE](#32-qtcreator-ide)
4. [File Structure](#4-file-structure)
    1. [project root](#41-project-root)
    2. [docs/](#42-docs)
    3. [Host/](#43-host)
    4. [Target/](#44-target)
    5. [tools/](#45-tools)
5. [Compiling the Source Code](#5-compiling-the-source-code)
    1. [Host Software](#51-host-software)
    2. [Target Software](#52-target-software)

--------------------------------------------------------------------------------

1 - Introduction
================

AMiRo-BLT is the bootloader and flashing toolchain for the base version of the Autonomous Mini Robot (AMiRo) [1].
It is based on OpenBLT developed by Feaser (see <http://feaser.com/en/openblt.php>) and implements the [Startup Shutdown Synchronization Protocol (SSSP)](https://gitlab.ub.uni-bielefeld.de/AMiRo/sssp).
While this file provides information for end users of AMiRo-BLT, developers can refer to the [`./docs/`](./docs/) folder for further readings:

- [AMiRo system signals](./docs/system_signals.md)
- [low-power modes](./docs/low_power_modes.md)
- [OS interface](./docs/os_interface.md)



2 - Required Software
=====================

In order to compile and flash the AMiRo bootloader, some additional software is required, all of which are described in detail in the following.


2.1 - Git
---------

Since all main- and subprojects are available as Git repositories, installing a recent version of the tool is mandatory.
Most Linux distributions like Ubuntu provide a sufficient version in their software repositories.


2.2 - GNU Make
--------------

GNU Make usually comes as preinstalled tool on Ubuntu based operating systems.
If your system is missing GNU Make, it is recommended to install it from the standard repositories since no special requirements (i.e. features of a very recent version) are required.


2.3 - GCC
---------

In order to build some required tools from source, GCC is required.
It usually comes as preinstalled tool on Ubuntu based operating systems.
If your system is missing GCC, it is recommended to install it from the standard repositories since no special requirements (e.g. features of a very recent version) are required.


2.4 - stm32flash
----------------

This tool is required to flash the bootloader binaries to the microcontrollers of the AMiRo base modules.
Since it is included in this project as a Git submodule, you can just run the [setup script](./setup.sh) in the [project root directory](./) using a terminal:

```sh
>$ ./setup.sh
```

Follow the instructions to download the source code and compile the tool.
The resulting binary path is `./Host/Source/stm32flash/stm32flash`.
Other scripts that require stm32flash will search for the binary at this location by default.

The setup script does not install the tool to your system path, though, since this usually requires root permissions.
However, stm32flash provides a [`Makefile`](./Host/Source/stm32flash/Makefile) with installation capabilities.
Just Follow the instructions given in the [`INSTALL`](./Host/Source/stm32flash/INSTALL) file.

Alternatively, some Linux distributions provide the tool in their software repositories.
If you do have root permissions on your system, you can install stm32flash this way.
However, the scripts will always check for a "local" installation in [`./Host/Source/stm32flash/`](./Host/Source/stm32flash/) first and only try a system-wide installation afterwards.


2.5 - GNU ARM Embedded Toolchain
--------------------------------

Various versions of the GCC for ARM embedded devices can be found at <https://developer.arm.com/open-source/gnu-toolchain/gnu-rm> (old versions are available at <https://launchpad.net/gcc-arm-embedded>).
For installation of the compiler toolchain and managing of multiple versions, it is highly recommended to use the provided setup script.
Alternatively you can install the compiler manually by following the instructions that can be found on the web page.

If you are going to install an old version, which is not available as 64-bit package, but you are running a 64-bit operating system, you have to install several 32-bit libraries.
The required packages are `libc6`, `libstdc++6`, and `libncurses5`.
You can run the following shell commands to install the according 32-bit versions of those packages:

```sh
>$ sudo dpkg --add-architecture i386
>$ sudo apt-get update
>$ sudo apt-get install libc6:i386 libstdc++6:i386 libncurses5:i386
```

2.6 - CMake
-----------

In order to build the SerialBoot host application, CMake version 2.8 or later is required.
If possible, it is recommended to instal it from the standard repositories of your operating system.



3 - Recommended Software
========================

The software tools named in this section are not essential for simply using or further development of AMiRo-BLT, but are useful in both scenarios.


3.1 - PlantUML
--------------

PlantUML is a free and open source Java tool to generate UML diagrams via scrips (see <https://plantuml.com>).
AMiRo-BLT provides according scripts in the [`./Target/docs/`](./Target/docs/) directory.
Please refer to the PlantUML documentation for how to generate figures from these script files.


3.2 - QtCreator IDE
-------------------

AMiRo-BLT provides support for the QtCreator IDE.
In order to setup according projects, use the setup script and follow the instructions.
It will automatically generate the required files and you can import the projects by opening the `.creator` files with QtCreator IDE.


4 - File Structure
==================

The AMiRo-BLT root directory contains four folder.
First, the [`./docs/`](./docs/) folder contains files related to documentation.
Actual code of the bootloader and the toolchain for host systems is located in the folders [`./Host/`](./Host/) and [`./Target/`](./Target/).
Finally, several useful tools for installation, maintenance and development can be found in the [`./tools/`](./tools/) directory.

4.1 - [project root](./)
------------------------

The project root directory contains [this file](./README.md), a [license file](./license.html) as well as the [setup.sh](./setup.sh) bash script.
While the purpuse of the former two files is obvious, the latter provides an easy to use tool for initialization of the AMiRo-BLT project as well as installation and managing of cross compilers.

4.2 - [docs/](./docs/)
----------------------

Since all code is documented inline, this directory just contains several further Markdown files, detailing the following aspects:

- Low power modes available on the AMiRo platform and supported by AMiRo-BLT.
- Interface for an operating system to interact with the bootloader.
- System signals used by the Bootloader during system startup and shutdown.


4.3 - [Host/](./Host/)
----------------------

This directory contains all source code for the essential tools to be executed on a host machine.
First, there is [SerialBoot](./Host/Source/SerialBoot/), which can be used to interact with the bootloader on a device to update the firmware.
Second, [stm32flash](./Host/Source/stm32flash/), which is required to flash the bootloader to a device, is included here as Git submodule.
When compiled using the provided Makefiles, the resulting binaries are placed in according subfolders.


4.4 - [Target/](./Target/)
--------------------------

This directory contains all source code for the bootloader itself.
While the [`./Target/docs/`](./Target/docs/) folder contains some further documentation, specifically for this part of AMiRo-BLT, all bootloader code is located in the folders [`./Target/Modules/`](./Target/Modules/) and [`./Target/Source/`](./Target/Source/).
Any code which is individual for each device can be found in the former, whereas all general code which is identical for all devices is located in the latter.
When compiled using the provided make scripts, the resulting binary images are placed in according subfolders within the folders for the individula modules (e.g. [`./Target/Modules/DiWheelDrive_1-1/`](./Target/Modules/DiWheelDrive_1-1/)).


4.5 - [tools/](./tools/)
------------------------

This directory contains several useful tools to initialize and configure AMiRo-BLT, install and manage cross compilers as well as setting up IDE projects.
For convenience, all these tools can be accessed via the [setup script](./setup.sh) in the AMiRo-BLT [root directory](./).


5 - Compiling the Source Code
=============================

The AMiRo-BLT project is separated into two major parts: target- and host-related software.
The former comprises the bootloaders for the base modules of the AMiRo platform.
The latter are the stm32flash tool as already mentioned above, and the SerialBoot tool, which can be used to flash further binaries (e.g. an operating system) to the microcontrollers without connecting to the module directly (data is passed through via CAN bus).
Since the programming connector of the lowermost AMiRo module (_DiWheelDrive_) is the only one accessible when the robot is fully set up, this enables to update the firmware even for other modules without the need to disassemble the device.


5.1 - Host Software
-------------------

The stm32flash tool is required to flash bootloader binaries to the MCUs.
Instructions on how to build and install the tool are given in chapter 2.4 of this file.

The SerialBoot tool can be built by using cmake.
The according [`CMakeLists.txt`](./Host/Source/SerialBoot/CMakeLists.txt) file can be found in the [`./Host/Source/SerialBoot/`](./Host/Source/SerialBoot/) directory.
To ensure compatibility with other software (e.g. [AMiRo-OS](https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS)) it is highly recommended to use the provided setup script to build SerialBoot.
In the end, the binary path should be `./Host/Source/SerialBoot/build/SerialBoot`, which is the default for any scripts and tools that use SerialBoot.


5.2 - Target Software
---------------------

Module specific code for the several AMiRo base modules is located in the individual subfolders in the [`./Target/Modules/`](./Target/Modules/) directory.
To compile (and flash; please read further) the bootloaders, it is recommended to run make in the [`./Target/`](./Target/) folder.

In order to flash the bootloader to a microcontroller, you first have to set full read and write permissions to the USB ports of your system when a programming cable is plugged in.
To do so, first create a new file by executing the following command (root permissions required):

```sh
>$ sudo touch /etc/udev/rules.d/50-usb-serial.rules
```

Open the file in a text editor of your choice (with root permissions) and add the following lines:

```
# Future Technology Devices International Ltd. - TTL-232RG
SUBSYSTEMS=="usb", ACTION=="add", KERNEL=="ttyUSB[0-9]*", SYMLINK+="ttyAMiRo%n",
ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666"

# Future Technology Devices International Ltd. - FT231X
SUBSYSTEMS=="usb", ACTION=="add", KERNEL=="ttyUSB[0-9]*", SYMLINK+="ttyAMiRo%n",
ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6015", MODE="0666"
```

Now connect the module you want to flash directly to your system (note that indirect flashing using SerialBoot is not possible for the bootloader itself) and run the command

```sh
>$ make flash_<module>
```

where `<module>` specifies, which module you are going to flash (e.g. `DiWheelDrive_1-1`).

If the procedure was not successful, the following hints might help:

- Did your system apply the new udev rules?<br/>
  Re-login (or reboot) and try again!
- Could make execute the stm32flash tool?<br/>
  Check the stm32flash installation (reinitialize the submodule if required) and
  try again!
- Are the permissions for USB ports set correctly?<br/>
  Check the udev rules!
- Are there any other applications using the serial connection?<br/>
  Close any other applications using the serial connection!
- Is the AMiRo module connected to your system?<br/>
  Use the programming cable to connect the module to your system!
- Is the AMiRo module powered up?<br/>
  Keep a charger plugged in during flashing!
- Was there an error when opening the ports?<br/>
  Please read on!

By default, the scripts use the first matching port (i.e. `/dev/ttyAMiRo0` or `/dev/ttyUSB0`) for flashing.
If you have connected multiple AMiRo modules to your system, those will be listed with increasing numbers in their identifiers.
Furthermore, other USB devices (possibly internal components) might be listed as such as well.
In those cases, you have to specify the correct port manually when calling make:

```sh
>$ make flash STM32FLASH_PORT=/dev/ttyAMiRo#
```

where you have to replace the trailing `#` with the according integer value.
You can easily list all connected devices via

```sh
>$ ls /dev/ | grep -E "^tty(USB|AMiRo)[0-9]+$"
```

**Attention**:<br/>
Never flash a bootloader to the wrong module!
Doing so might cause severe errors and damage the hardware.

