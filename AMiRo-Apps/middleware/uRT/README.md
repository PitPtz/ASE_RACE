About & License
===============

µRT is a lightweight publish-subscribe & RPC middleware for real-time
applications. It was developed as part of the software habitat for the
Autonomous Mini Robot [1] (AMiRo) but can be used for other purposes as well.

Copyright (C) 2018..2022 Thomas Schöpping et al. (a complete list of all authors
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
- Svenja Kenneweg (skenneweg[at]techfak.uni-bielefeld.de)
- Aleksndrs Stier

References
----------

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

--------------------------------------------------------------------------------

Contents
========

1. [Introduction](#1-introduction)
2. [Components and Modules](#2-components-and-modules)
   1. [OSAL](#21-osal)
   2. [Core](#22-core)
   3. [Publish-Subscribe](#23-publish-subscribe)
   4. [Remote Procedure Calls (RPC)](#24-remote-procedure-calls-rpc)
3. [File Structure](#3-file-structure)

--------------------------------------------------------------------------------

# 1 - Introduction

µRT (pronounced like "*Marty*") is a light-weight real-time middleware, featuring an operating system abstraction layer (OSAL), publish-subscribe message passing and remote procedure calls (RPC) as well as integrated quality of service (QoS) to detect and handle violations of real-time constraints.
Completey written in C, it provides best compatibility to be deployed in any environment.
µRT is highly configurable at compile time in order to offer the best performance for any specific use case.
If desired, it is even possible to completely disable publish-subscribe, RPC and/or QoS.
Even though µRT was primarily designed to be used on microncontrollers (MCUs), it can be deployed in any real-time environment.

The following list provides an overview about the most important features of µRT:
* Small memory footprint (<1kB) for deployment on MCUs.
* Completely event-based for high responsiveness.
* Fully real-time capable with integrated QoS:
  * Static memory only (no dynamic allocation).
  * Constant to linear complexity in all dimensions.
  * Integrated QoS to detect and handle any timing violations.
  * Support for several real-time classes:
    * HRT (hard real-time) strictly expects certain timing conditions to be met.
      Violations are assumed to be caused by severe failures and result in either a recovery attempt or a system panic.
    * For SRT (soft real-time) a usefullness depending on the latency is calculated for each data.
      The mapping function is arbitrary, returning a value in range $`[0, 1]`$.
    * FRT (firm real-time) is a special case of SRT, where the usefullness is binary and each data can be either valid or invalid.
      This not only simplifies calculations, but also allows to efficiently track jitter.
    * NRT (non-real-time) is another special case of SRT, where usefulness is of no concern and each data stays valid for an infinite amount of time.
* Similar message passing paradigms as known from other popular frameworks (e.g. ROS):
  * Topic-based publish-subscribe for simple broadcast message passing.
  * Service-based remote procedure calls for spontaneous requests.
* Space and synchronization decoupling of communicating *nodes*.
  Time decoupling is not supported due to the real-time nature of µRT (unavailability of *nodes* is considered a severe failure).
* Explicitely no support (and therewith no dependency) of any specific inter-process communication or network interface.
  All data is passed between *nodes* (threads) via shared memory.
  *Bridge nodes* can be used to connect multiple µRT instances via arbitrary interfaces.

# 2 - Components and Modules

µRT is divided into several modules, some of which can be opted out.
Two major and mandatory components are the operating system abstraction layer (OSAL) and the µRT core.
The two most important, but yet optional, modules are the publish-subscibe and RPC.
All of these components are again highly configurable and can be adapted in many ways to best fit an individual application scenario.

## 2.1 - OSAL

The operating system abstraction layer consist of POSIX-like functions and types to provide an API for the most commonly used functions by an operating system, such as mutex locks, timers, thread handling and more.
While µRT defines those interfaces, the including project has provide implementations of all functions and types and map the OSAL API to the underlying operating system. Any function may be implemented as actual method or as C macro for best flexibility and minimum performance impact.

As an important non-POSIX interface, µRT does not provide it's own event system, but expects such to be provided externally via OSAL.
The reason for this design decision was that many RTOSes (real-time operating systems) already feature such event systems, which are highly intregrated into the system kernel and thus are the most efficient solution for the respective application scenario.

## 2.2 - Core

First of all, a central control component (*core*) handles high-level logic, such as registration of *topics* and *services*.

Secondly, *nodes* are the fundamental building blocks of any application.
Each node is an individual thread, which may spawn further threads but has to manage those by itself.
Execution of a node is defined to be subdivided into three stages:
1. The *setup* is executed at the very beginning when the system starts up.
2. Each time an event is received, the *loop* is executed.
3. When the node is requested to terminate, the *shutdown* is executed.

µRT synchronizes all nodes to execute each of these stages simultanously.
Vice versa, a node will only enter the *loop* stage after all nodes have finished their *setup*.

Furthermore, *sync groups* form another synchronization mechanism to synchronize multiple nodes during the *loop* stage.
Nodes can be dynamically join and leave those and each node can be part of multiple *sync groups* at any time.

## 2.3 - Publish-Subscribe

Information is distributed in broadcast fashion from *publishers* to *subscribers* by sending *messages* via *topics*.
First of all, each *topic* can be identified by a unique key via the µRT *core*.
Each topic holds a list of *messages*, arranged as ring buffer.
In order to provide information, *publishers* broadcast messages via their associated *topic*.
All *subscribers* of that *topic* are then informed by an event and can fetch the data.

In order to provide real-time capabilities, *subscribers* are distinguished into the four classes *HRT subscribers*, *FRT subscribers*, *SRT subscribers* and *NRT subscribers* respectively.
Each *subscriber* can define its individual timing constraints, which can be checked on demand (FRT and SRT) or are verified automatically by the integrated QoS mechanics (HRT).
QoS for the publish-subscribe module supports three types of constraints:
* *Latency*:
  By defining a maximum latency (deadline) information becomes invalid if it takes a *subscriber* too long to fetch the according *message*.
* *Jitter*:
  When a maximum expected jitter is defined, information becomes invalid if a *message* is fetched too late or too early, depending on previous latencies so far.
* *Rate*:
  After information was provided, *HRT subscribers* may expect the next data to become available within a certain time frame.
  If this expected minimum rate is violated, µRT will execute a system panic.

## 2.4 - Remote Procedure Calls (RPC)

Remote procedure calls provide a mechanism to trigger actions on demand by submitting *requests* to *services*.
Each *node* can offer any number of *services*, which are registerd in the µRT *core* and are identified by uniques keys.
Even though RPCs are typically considered as synchronous actions, in µRT responses are received asynchronously, due to its event-driven nature.
Each RPC sequence is thus divided into multiple steps:
1. The requesting *node* has to
   1. acquire a *request* in order to ensure exclusive access to the data structure,
   2. prepate the *request*, i.e. setting up parameter data and
   3. submit the *request* to a *service*, which will notify the provding *node* via an event.
2. As soon as the *service node* is notified, it has to:
   1. dispatch the *request* and copy the argument data,
   2. process the *request*,
   3. reacquire the *request* and set up return data and
   4. respond the *request*, which will notify the requesting *node* via an event.
3. As soon as the requesting *node* is notified abou the response, it has to
   1. retrieve the *request* and
   2. release the *request* to make it available again for future utilizations.

Since RPCs always involve bidirectional communication, overhead is higher than with publish-subscribe.
It is very useful, however, for triggering remote actions (e.g. setting LED or motor values) or requesting data, which is processed remotely (library-like function calls).

In order to provide real-time capabilities, *requests* are distinguished into the four classes *HRT requests*, *FRT requests*, *SRT requests* and *NRT requests* respectively.
Each *request* can define its individual timing constraints, which can be checked on demand (FRT and SRT) or are verified automatically by the integrated QoS mechanics (HRT).
QoS for the RPC module supports two types of constraints:
* *Latency*:
  By defining a maximum latency (deadline) information becomes invalid if it takes a *service* too long to process a *request* or the *request* is not retrieved in time.
* *Jitter*:
  When a maximum expected jitter is defined, information becomes invalid if a *request* is responded and retrieved too late or too early, depending on previous latencies so far.

# 3 - File Structure

While all source code of µRT is located in the [`inc/`](./inc/) and [`src/`](./src/) folders, interface files are placed in the [root folder](./) alongside this document.
Additional documentation files can be found in the [`docs/`](:/docs/) directory.
Finally, a template for µRT configuration header files is provided in the [`templates/`](./templates/) folder.
