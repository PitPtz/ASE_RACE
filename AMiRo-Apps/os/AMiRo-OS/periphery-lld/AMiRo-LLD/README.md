About & License
===============

AMiRo-LLD is a compilation of low-level hardware drivers for the base version of
the Autonomous Mini Robot (AMiRo) [1]. It provides directional interfaces for an
operating system to access the drivers and for the drivers to access the
communication infrastructure via the operating system.

Copyright (C) 2016..2022 Thomas Schöpping et al. (a complete list of all authors
is given below)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.

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
- Robin Ewers
- Felix Wittenfeld

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

--------------------------------------------------------------------------------

Contents
========

1. [About the Project](#1-about-the-project)
2. [File Structure](#2-file-structure)
    1. [project root](#21-project-root)
    2. [docs/](#22-docs)
    3. [drivers/](#23-drivers)
    4. [templates/](#24-templates)
3. [Developer Guides](#3-developer-guides)
    1. [Adding a Device](#31-adding-a-device)
    2. [Implementing a Driver](#32-implementing-a-driver)

--------------------------------------------------------------------------------

1 - About the Project
=====================

AMiRo-LLD is a compilation of low-level hardware drivers, originally developed for the Autonomous Mini Robot (AMiRo) [1].
It uses a modular design, so that each driver can be used and configured individually as required.
Interface functions allow for bidirectional communication with an operating system.
On the one hand drivers access according hardware interfaces via defined interface functions (which need to be implemented by the operating system), on the other hand any applications (or the operating system itself) can take advantage of the drivers by their individual interfaces.
The abstraction layer of the hardware interfaces is called *periphAL*, which is defined by this project.
In order to further configure individual drivers, the project expects an according file `alldconf.h` to be found in the include paths when compiling the drivers.

Although this compilation was originally designed to be used in combination with the AMiRo operating system (AMiRo-OS; cf. <https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS>), it is not limited to this use case.
The included drivers may be used for any purpose and contributions of further drivers, even if the according hardware is not present on the AMiRo platform, are highly appreciated.



2 - File Structure
===================

Ths project is structured int three major parts: documentation, templates and - most impotantly - the drivers themselves.

2.1 - [project root](./)
------------------------

The project root directory contains [this file](./README.md), a [license file](./license.html) as well as a make script [`amiro-lld.mk`](./amiro-lld.mk) that allows to easily integrate AMiRo-LLD in other projects.
Furthermore, two interface headers are provided: [`amiro-lld.h`](./amiro-lld.h) and [`periphAL.h`](./periphAL.h).

2.2 - [docs/](./docs/)
----------------------

UML graphs (using PlantUML; see <https://plantuml.com> for further information) visualize the structure of the AMiRo-LLD project.
Doxygen related files can be used to generate a documentation of the whole project (work in progress).

2.3 - [drivers/](./drivers/)
---------------------------

For each supported hardware device, there is exactly one directory in this folder.
Further subfolders contain various versions of a driver (e.g. `v1/`, `v2/`, etc.).
By convention, the root directory of a driver is named by the exact product name of the according hardware, or the product family, if the driver is compatible with all parts.
Each driver must provide a make script, which adds the required include paths to the `AMIROLLD_INC` variable and all C source files to the `AMIROLLD_CSRC` variable.

2.4 - [templates/](./templates/)
--------------------------------

AMiRo-LLD expects a configuration header file [`alldconf.h`](./templates/alldconf.h) to be found in the include paths.
An according template for such a file can be found here.
There is no template for an implementation of *periphAL*, though, as the interface header in the root directory ([`periphAL.h`](./periphAL.h)) provides all required information for an implementation.
If there is something like a reference implementation of *periphAL*, it is the [integration in AMiRo-OS](https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS/-/tree/master/periphery-lld).



3 - Developer Guides
====================

In order to keep all code within this project as homogeneous as possible, the guides in this chapter should help developers to achieve functional and clean results, which are portable and maintainable for future use.
Whereas the textual descriptions of the guides provide in-depth information about the underlying concepts and mechanisms, a short summary is provided at the end of each chapter.

3.1 - Adding a Device
---------------------

When adding a new device to the project, the very first step is to create the according folder in the [`./drivers/`](./drivers/) directory.
For this guide, we will add the fictional device "DEVICE1234".
The folders to be created in this case are hence `./drivers/DEVICE1234/` and `./drivers/DEVICE1234/v1/`.
In case there already exists a driver implementation for this device, but you want to implement another version, the version subfolder must be named accordingly (e.g. `./drivers/DEVICE1234/v2/`).

Most drivers will consist of exactly three files:

- `alld_DEVICE1234.mk`
- `alld_DEVICE1234.h`
- `alld_DEVICE1234.c`

Some drivers, however, may feature multiple header and/or source files or even come with additional subfolders.
In any case, all those required folders, including the driver root folder (i.e. `./drivers/DEVICE1234/v1/`) as well as all source files must be added to the variables `AMIROLLD_INC` and `AMIROLLD_CSRC` by the make script.

It is highly recommended that files in the driver root directory (i.e. `./drivers/DEVICE1234/v1/`) use the prefix `alld_` in their names.
This not only helps to achieve an easy to understand file structure, but also prevents compilation issues due to naming conflicts of header files.

**Summing up, you have to**

1. Create device and version folders.
2. Add a make script.
3. Add header and source files as well as subfolders, implementing the driver.

3.2 - Implementing a Driver
---------------------------

Implementation of a new driver usually is very straightforward.
You will most probably start with a comprehensive datasheet of the device, or the manufacturer even provides a reference driver implementation.

For the former case, you should first write a comprehensive header, containing all information like constants, register maps, etc. and according abstract access functions (e.g. for reading and writing registers, and convenient access to common functionalities).
Only then you implement those functions, using *periphAL* to interface any hardware interfaces (e.g. I2C, SPI, etc.) in a separate C source file, or 'inline' in the header file itself.

For the latter case, the reference implementation will specify some interface functions to interact with the hardware (e.g. I2C, SPI etc.).
Even though all functionality should be covered by the reference driver, you still need to implement those interface functions and map them to *periphAL*.

Since AMiRo-LLD does not rely on specific hardware or operating system, the only valid way to interact with both is through *periphAL*.
Under no circumstances you must use any function of your operating system directly to interact with the hardware or the operating system!
For your driver, there is no knowledge about the world beyond *periphAL*.
If *periphAL* does not provide the functionality you need, you should do the following:

1. Think again if you really need that functionality or whether it can be replicated by the existing API.
2. File a feature request to extend *periphAL*.
3. Provide a patch that modifies *periphAL* to meet your requirements.

**Summing up, you have to**

1. Get and read the datasheet of the device (A) or acquire a copy of the reference implementation (B).
2. Case A: Define constants, register map and access functions in a header file.<br/>
   Case B: Identify the interface functions of the reference implementation.
3. Implement all required functions using *periphAL*.

