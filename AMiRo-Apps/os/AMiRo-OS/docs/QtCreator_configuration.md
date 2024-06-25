About & License
===============

AMiRo-OS is an operating system for the base version of the Autonomous Mini
Robot (AMiRo) [1]. It utilizes ChibiOS (a real-time operating system for
embedded devices developed by Giovanni di Sirio; see <http://chibios.org>) as
system kernel and extends it with platform specific configurations and further
functionalities and abstractions.

Copyright (C) 2016..2022  Thomas Schöpping et al. (a complete list of all
authors is given [here](../README.md))

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General (Lesser) Public License as published by the Free Software
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

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

--------------------------------------------------------------------------------

QtCreator IDE configuration
===========================

This manual shall help you to setup QtCreator projects for any module in AMiRo-OS.
It also provides instructions how to integrate the GCC ARM embedded toolchain into the IDE.


Setting up Projects
-------------------

To automatically generate all required files for QtCreator, run the provided setup script (in the [AMiRo-OS root folder](../)) and follow the instructions:

```
>$ ./setup.sh
```

You then just have to open the generated `.creator` files with the QtCreator IDE.


Integration of the GCC ARM Embedded Toolchain
---------------------------------------------

In order to integrate the compiler and flashing tools into your project, some further configurations are required.

### Toolkit Setup

Start by launching the QtCreator application, open the Tools->Options settings and select "Build & Run".
Under the tab "Compilers" add the installed `arm-none-eabi-gcc` compiler as another GCC.
Under the tab "Debuggers" add the according `arm-none-eabi-gdb` debugger.
Under the tab "Kits" you need to manually create a new kit with the following settings:

Name: AMiRo <br/>
Device type: Desktop <br/>
Sysroot: _gcc-arm-none-eabi include path_ <br/>
Compiler: _the compiler added above_ <br/>
Debugger: _the debugger added above_ <br/>
Qt version: none

Keep any other fields at their default values.
If there is a warning sign shown beside the new kit, check the settings for errors.

### Project Integration

For convenience, the absolute path to your module will be referenced in the following as `MODULE_PATH`.

After you have sucessfully created the project files, open the project by selecting the `.creator` file with QtCreator.
Click the "Projects" icon at the very left and switch to the "Build & Run" tab.
Change the used kit to "AMiRo" (right above the "Build" and "Run" buttons).

Next, add a new build configuration and call it "make":

Build directory: `MODULE_PATH` <br/>
Build Steps: `make --directory=MODULE_PATH all` <br/>
Clean Steps: `make --directory=MODULE_PATH clean`

Optionally, you can add the `-j` option to the build steps in order to parallelize the compilation process.
Add yet another build configuration by cloning the "make" configuration and name it "make flash".
For this copy, replace `all` at the end of the build step with `flash`.
Finally remove the "Default" configuration and repeat these steps for any further projects (= modules).

