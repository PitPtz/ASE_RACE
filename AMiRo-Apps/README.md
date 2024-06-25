About & License
===============

AMiRo-Apps is a collection of applications and configurations for the Autonomous
Mini Robot (AMiRo) [1]. It is tightly coupled to the AMiRo-OS and uRT projects,
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
- Aleksandrs Stier


References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

[AMiRo-OS]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS
[µRT]: https://gitlab.ub.uni-bielefeld.de/AMiRo/uRtWare

--------------------------------------------------------------------------------

Contents
========

1. [Required Software](#1-required-software)
2. [File Structure](#2-file-structure)
3. [Guide](#3-guide)

--------------------------------------------------------------------------------

1 - Required Software
=====================

Since AMiRo-Apps depends on the [AMiRo-OS] and [µRT] projects, both are included as Git submodules.
Use the provided `./setup.sh` script to initialize those.
When [AMiRo-OS](./os/AMiRo-OS/) is fully configured, there is no additional software required by AMiRo-Apps.
For further details about the initialization follow the instructions in the script and refer to the `README.md` files of [AMiRo-OS](./os/AMiRo-OS/README.md) and [µRT](./middleware/uRT/README.md) respectively.


2 - File Structure
==================

At its root, the AMiRo-Apps project has seven folders to distinguish between the most fundamental parts:


- **[`apps/`](./apps/)**<br>
  This folder contains individual applications, which can used by configurations (cf. `configurations/`).
  Apps will usually be implemented as nodes for the middleware and provide according interfaces via topics and/or services type(s) (cf. `messagetypes/`).

- **[`configurations/`](./configurations/)**<br>
  Each configuration combines multiple apps and distributes them among any supported modules.
  Furthermore, configurations may modify parameters for each module individually.
  Finally, each configuration should come with a Makefile, so a user can just use this to build and flash the whole setup.
  For better understanding, have a look at this example:

    - module A: using apps 1, 2 and 3 with parameters X and Y.
    - module B: using apps 1 and 4 with parameter X.
    - module C: using apps 1, 5, 6 and 7 with parameters X and Z.
    - Makefile: setup and build everything.

  Note that each configuration needs to explicitly support a module and that there can never be an implicit default configuration.

- **[`docs/`](./docs/)**<br>
  Any documentation related files are located in this directory.

- **[`messagetypes/`](./messagetypes/)**<br>
  Apps should take advantage of the middleware and thus implement nodes, which communicate via messages and/or requests.
  Such messages/requests may have payloads of arbitrary types, which can be defined in this folder.

- **[`middleware/`](./middleware/)**<br>
  This folder contains the [µRT] submodule and interface files.

- **[`os/`](./os/)**<br>
  This folder contains the [AMiRo-OS] submodule and interface files.

- **[`tools/`](./tools/)**<br>
  Helpful tools can be found in this directory.


3 - Guide
=========

To get a better understanding how [AMiRo-OS], [µRT], applications and configurations work together, a [HelloWorld example](./configurations/HelloWorld/) is provided with [detailed guides](./configurations/HelloWorld/README.md).
That configuration utilizes the [HelloWorld application](./apps/HelloWorld/) to demonstrate how the individual components are plumbed together and how the most important interactions - between application nodes and with the user - can be realized.
Furthermore, there is a [tutorial](./docs/Tutorial.md) to guide new users how to create the HelloWorld application and configuration from scratch.
