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

[AMiRo-OS]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS
[µRT]: https://gitlab.ub.uni-bielefeld.de/AMiRo/uRtWare

--------------------------------------------------------------------------------

Contents
========

1. [Introduction](#1-introduction)
2. [Files](#2-files)
3. [Master Node](#3-master-node)
4. [Slave Node](#4-slave-node)
5. [Create Your Own Application](#5-create-your-own-application)

--------------------------------------------------------------------------------

1 - Introduction
================

This file shall provide some helpful insights, how the HelloWorld application works.
For better understanding of the operating principles of the [µRT] it is highly recommended to read its [readme file](../../middleware/uRT/README.md).
Since this HelloWorld application is of no use by its own but must be utilized by a configuration, it is also recommended to read the [readme file](../../configurations/HelloWorld/README.md) of the [HelloWorld configuration](../../configurations/HelloWorld/).


2 - Files
=========

This folder contains eight files:

- **[HelloWorld.mk](./HelloWorld.mk)**<br>
  This GNU Make script defines the required middleware for the HelloWorld application, all required include paths, as well as any C/C++ source files.
- **[README.md](./README.md)**<br>
  This Markdown file provides useful information on how the HelloWorld application works and shall empower you to develop your own applications.
- **[helloworld.h](./helloworld.h)** and **[helloworld.c](./helloworld.c)**<br>
  These C files provide a function to calculate the usefullness of SRT communication.
  This method is used by the master and slave nodes.
  It furthermore includes the master and slave header files in order to provide an interface for the entire HelloWorld application with this single file.
- **[helloworld_master.h](./helloworld_master.h)** and **[helloworld_master.c](./helloworld_master.c)**<br>
  These C files contain the master node of the HelloWorld application.
  The master publishes new messages (publish-subscribe communciation) and emits requests (a remote procedure call).
- **[helloworld_slave.h](./helloworld_slave.h)** and **[helloworld_slave.c](./helloworld_slave.c)**<br>
  These C files contain the slave node of the HelloWorld application.
  The slave receives messages (publish-subscribe communication) and provides a service (for remote procedure calls).


3 - Master Node
===============

The master node consists of several structures (cf. [helloworld_master.h](./helloworld_master.h)):

- A thread in which the node is executed.
- A µRT node object to interact with the µRT middleware.
- An event and listener, so the user can trigger the node to execute some action.
- A publisher for communication using the publish-subscribe system of µRT.
- Data for communication using the remote procedure call system of µRT:
  - An identifier to address a service.
  - A deadline for FRT and HRT requests.
  - A set of requests for NRT, SRT, FRT and HRT communication.
- A boolean flag to enable output.

As with all µRT nodes, the logic of the master node is separated into four primary functions (cf. [helloworld_master.c](./helloworld_master.c)):

- **helloWorldMasterInit()**<br>
  All data of the master node is initialized.
  This is usually done during system startup.
- **_helloworld_masterSetup()**<br>
  This callback is executed when the µRT node thread starts.
  In case of the master node, the thread sets its own name and registers to the trigger event.
  Note that the function returns the event mask which shall be used to "wake" the thread later on to execute its loop callback.
- **_helloworld_masterLoop()**<br>
  This callback is executed whenever the thread received an event that matches the previously set mask.
  The loop callback function typically is the most complex part of a node, because this is where the actual processing is done during operation.
  The master node acts differently, depending on the event(s) propagated to this method.
  In case of a trigger event, the trigger listener is evaluated for further flags.
  These flags encode the information whether the user requested the master node to initiate a publish-subscribe communication or a remote procedure call (RPC) interaction.
  In case of publish-subscribe, a message is published.
  In case of RPC, four requests (NRT, SRT, FRT and HRT) are submitted.
  However, the loop callback may also be executed due to a servic event.
  This is the case whenever the servic responds to a submitted request.
  The requests are evaluated and some information is printed.
  Finally, an event mask is returned, defining which events may wake the thread next time.
- **_helloworld_masterShutdown()**<br>
  This callback is executed when the µRT node terminates.
  In case of the master node, it unregisters from the trigger event.


4 - Slave Node
==============

The slave node consists of several structures as well (cf. [helloworld_slave.h](./helloworld_slave.h)):

- A thread in which the node is executed.
- A µRT node object to interact with the µRT middleware.
- Data for communication using the publish-subscribe system of µRT:
  - An identifier to address a topic.
  - Timing constraints for deadline, jitter and rate.
  - A set of subscribers for NRT, SRT, FRT and HRT communication.
- A service for communication using the remote procedure call system of µRT.
- A boolean flag to enable output.

Similar to the master node, the logic of the slave node is sperated into four primary functions (cf. [helloworld_slave.c](./helloworld_slave.c)):

- **helloWorldSlaveInit()**<br>
  All data of the slave node is initialized.
  This method should be called during system startup.
- **_helloworld_slaveSetup()**<br>
  This callback is executed when the µRT node thread starts.
  In case of the slave node, the thread sets its own name, retrieves a topic using its identifier and subscribes the four subscribers to that topic.
  Likewise to the master node, the function returns the event mask to "wake" the thread during operation.
- **_helloworld_slaveLoop()**<br>
  The slave node differentiates two events.
  It is either triggered via the topic - a "publish event" - whenever a new message is available to be fecthed.
  In that case, the slave node fetches all pending messages for each of the four subscribers and prints a brief statement.
  Whenever a request has been submitted to the service if the slave node, the loop callback is executed due to a "request event".
  In this case, all submitted requests are dispatched and responded to.
  Typically there would be some processing inbetween, but for the sake of simplicity, the slave node responds immediately.
  Finally, an event mask is returned, defining which events may wake the thread next time.
- **_helloworld_slaveShutdown()**<br>
  When the slave node terminates, all its subscribers unsubscribe from the topic.


5 - Create Your Own Application
================================

In order to develop a new application, you should proceed according to the folloowing steps:

1. Initialize a new folder for the application and the three fundamental files (C header, C source and GNU Make script) with only very basic logic.
2. Initialize a new configuration or add the application to an existing one.
3. Develop the logic of the application and test it using the configuration.
   You may add further files and subfolders as required, but there should still be a single GNU Make script and a single C header file to interface the entire application.
