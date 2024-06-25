# About & License

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


## Authors

- Thomas Schöpping (tschoepp[at]cit-ec.uni-bielefeld.de)


## References

**[1]** S. Herbrechtsmeier, T. Korthals, T. Schopping and U. Rückert, "AMiRo: A modular & customizable open-source mini robot platform," 2016 20th International Conference on System Theory, Control and Computing (ICSTCC), Sinaia, 2016, pp. 687-692.

[AMiRo-OS]: https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-OS
[µRT]: https://gitlab.ub.uni-bielefeld.de/AMiRo/uRtWare

--------------------------------------------------------------------------------

# Introduction

This tutorial provides a step-by-step guide to create a simple HelloWorld application and configuration from scratch.
It is designed to take about two to three hours.
Before you start, [make sure you have initialized the AMiRo-Apps project correctly as described in section "Required Software" in the README file](../README.md#1-required-software).
Since this tutorial will utilize [µRT] but not cover its core components and functional principles (i.e. nodes, publish-subscribe communication and remote procedure calls), it is highly recommended to have a look at its documentation as well.

The tutorial is divided into several sections:

1. [Initialization of Folders and Files](#1-initialization-of-folders-and-files)
2. [Setup of C skeleton files](#2-setup-of-c-skeleton-files)
3. [Setup of Build Scripts](#3-setup-of-build-scripts)
4. [Periodic Output](#4-periodic-output)
5. [Publish-Subscribe](#5-publish-subscribe)
6. [Remote Procedure Calls](#6-remote-procedure-calls)
7. [Custom Message String](#7-custom-message-string)

Please note that this tutorial presumes basic C programing language skills.
If you are rather inexperienced with the C programming language, you will be able to complete this tutorial, but you might not understand some details of the code, so the overall insight might be limited.
There are also some GNU Make scripts, so experience in that language is beneficial as well.
Since these scripts are rather a means to an end, however, those parts of the tutorial are subordinary and you do not need to comprehend those in their entirety.
If you are interested in the sample solution of the tutorial, such is provided [here](Tutorial.zip).


--------------------------------------------------------------------------------

# 1 - Initialization of Folders and Files

In this first section, all required folders and files for an application and a configuration are initialized.
For now, none of these files are filled with any content, though.

## Application

First of all, create a new application folder `apps/Tutorial/` and three empty files in there:

- `tutorial.h`: C header file
- `tutorial.c`: C source file
- `Tutorial.mk`: GNU Make script

> While the C files will eventually contain the logic of the application itself, the GNU Make script will configure all requirements (source files, include paths, dependencies etc.) to build the application and provide several variables for simple integration in configurations.

The content of your `apps/Tutorial/` folder should now look like this (output of `tree` command):

```
apps/Tutorial
├── Tutorial.mk
├── tutorial.c
└── tutorial.h
```

## Configuration

Similarly, create a new configuration folder `configurations/Tutorial/`, but only add a single file and an empty directory in there:

- `Makefile`: GNU Make script
- `modules/`: folder for module specific files

> While the GNU Make script will provide an entry point to build the entire configuration, any logic, configurations and such, which are more specific to modules, will be located in the subfolder.

Any configuration needs to define at least one module.
In order to configure a new module, create another directory like `configurations/Tutorial/modules/NUCLEO-L476RG/`, where `NUCLEO-L476RG` is the module name.
You can use any name here, but this tutorial utilizes a NUCLEO-L476RG, a STM32 Nucleo-64 development board with STM32L476RG MCU.
Since AMiRo-Apps employs [AMiRo-OS], only plattforms supported by [AMiRo-OS] can be used for configurations.

>If you are going to employ an AMiRo for this tutorial, it is recommended to use the *DiWheelDrive* module.
>In case the bottom side of your AMiRo is a white PCB, utilize version 1.1 (`DiWheelDrive_1-1`).
>If the bottom is a black PCB, you should use version 1.2 (`DiWheelDrive_1-2`).

Next, initialize five files in the module folder (e.g. `configurations/Tutorial/modules/NUCLEO-L476RG/`):

- `apps.h`: C header file
- `apps.c`: C source file
- `osconf.h`: configuration file for [AMiRo-OS]
- `urtconf.h`: configuration file for [µRT]
- `Makefile`: GNU Make script

> The C files will eventually contain logic for user interaction and "plumbing code" to connect application instances.
> Since [AMiRo-OS] expects the file `osconf.h` somewhere in the include paths and [µRT] similarly expects the file `urtconf.h`, those must be provided by each module.
> The GNU Make script serves as entry point to build this specific module only.

These files contain information and logic, which is very specific to the individual module.
Typically, however, many configuration settings are shared among all modules of a configuration.
Therefore, create two more files in the `configurations/Tutorial/modules/` directory:

- `Tutorial_urtconf.h`: common configuration file for [µRT]
- `modules.mk`: common GNU Make script

> The configuration header file is meant to be included by the according individual configuration header files of all modules and provides common (default) configuration settings.
> The GNU Make script can be used to include common dependencies and set values to the build variables.

Now, the content of your `configurations/Tutorial/` folder should look like this (output of `tree` command):

```
configurations/Tutorial
├── Makefile
└── modules
    ├── NUCLEO-L476RG
    │   ├── Makefile
    │   ├── apps.c
    │   ├── apps.h
    │   ├── osconf.h
    │   └── urtconf.h
    ├── Tutorial_urtconf.h
    └── modules.mk
```


# 2 - Setup of C skeleton files

In this section, all C files are filled with rudimentary skeleton code.
This code will not do much so far, but provide all required configuration flags and an entry point for the logic added later.
The goal is to create initial code, which is just enough to make the configuration (and the application) compile successfully.

## Application

For now, the application shall not contain any logic, but all files shall be accessed during the build process.
Therefore enter the following code in the two C files:

- `apps/Tutorial/tutorial.h`<br>
  Just include guards, but no interfaces yet.
  ```c
  #ifndef TUTORIAL_H
  #define TUTORIAL_H

  #endif
  ```
- `apps/Tutorial/tutorial.c`<br>
  Only includes the interface header, but contains no logic yet.
  ```c
  #include <tutorial.h>
  ```

## Configuration

Similarly to the application files, enter the following code to the C files of the module in the configuration:

- `configurations/Tutorial/modules/NUCLEO-L476RG/apps.h`<br>
  Just include guards, but no interfaces yet.
  ```c
  #ifndef APPS_H
  #define APPS_H

  #endif
  ```
- `configurations/Tutorial/modules/NUCLEO-L476RG/apps.c`<br>
  Only includes the interface header, but contains no logic yet.
  ```c
  #include <apps.h>
  ```

Finally, the [AMiRo-OS] and [µRT] configuration header files need to be filled with content as well:

- `configurations/Tutorial/modules/NUCLEO-L476RG/osconf.h`<br>
  Just include guards, but no settings yet.
  ```c
  #ifndef OSCONF_H
  #define OSCONF_H

  #endif
  ```
- `configurations/Tutorial/modules/NUCLEO-L476RG/urtconf.h`<br>
  This file just includes the common [µRT] configuration header.
  ```c
  #ifndef URTCONF_H
  #define URTCONF_H

  #include <Tutorial_urtconf.h>

  #endif
  ```
- `configurations/Tutorial/modules/Tutorial_urtconf.h`<br>
  Since there is no implicit default configuration for [µRT], all settings must be specified here.
  Just copy the [template file](https://gitlab.ub.uni-bielefeld.de/AMiRo/uRtWare/-/blob/main/templates/urtconf.h "Upstream version might differ from your local copy.") from [µRT] (or locally from [here](../middleware/uRT/templates/urtconf.h "Local resource only available if you cloned AMiRo-Apps and initialized the µRT submodule.")), modify the include guards and disable the `URT_CFG_OSAL_HEADER` setting:
  ```c
    ...
  #ifndef URTCONF_H                                 ──>    #ifndef TUTORIAL_URTCONF_H
  #define URTCONF_H                                 ──>    #define TUTORIAL_URTCONF_H
    ...
  #define URT_CFG_OSAL_HEADER <custom_urtosal.h>    ──>    //#define URT_CFG_OSAL_HEADER <custom_urtosal.h>
    ...
  ```
  > The `URT_CFG_OSAL_HEADER` setting is already specified by the AMiRo-Apps build configuration.
  > An additional definition in the `urtconf.h` header results in compilation errors.

Now that all C files contain skeleton code, configure the build process next in order to generate and flash a binary image.


# 3 - Setup of Build Scripts

In order to build an image for a module of the configuration, the build process must be specified, obviously.
AMiRo-Apps uses GNU Make, so all according scripts must be filled wil content.
At the end of this section, running `make` will produce a functional image, even though there will be no application logic yet.

## Application

Enter the following lines in the `apps/Tutorial/Tutorial.mk` file:

```make
# absolute path to this directory
Tutorial_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# middleware selection
CONFIG_MIDDLEWARE := µRT

# include path
Tutorial_INC = $(Tutorial_DIR)

# C source files
Tutorial_CSRC = $(Tutorial_DIR)/tutorial.c

# C++ source files
Tutorial_CPPSRC =
```

> The script sets the middleware to be used to [µRT] and defines a single include path (containing the `tutorial.h` C header file) as well as a single C source file.
> Any C++ source files have to be specified via a dedicated variable.

## Module

First, enter the following lines in the common script `configurations/Tutorial/modules/modules.mk`:

```make
# absolute path to this directory
APPSCONFIG_MODULES_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# environment setup
APPSCONFIG_DIR := $(realpath $(APPSCONFIG_MODULES_DIR)/..)
include $(APPSCONFIG_MODULES_DIR)/../../apps.mk

# include paths
APPS_INC += $(APPSCONFIG_DIR) \
            $(APPSCONFIG_MODULES_DIR)

# C source files
APPS_CSRC +=

# C++ source files
APPS_CPPSRC +=
```

> This script includes the `apps.mk` script, which resolves all general dependencies of AMiRo-Apps.
> Furthermore, common include paths, C source files and C++ source files are specified.

Next, the module specific entry script (e.g. `configurations/Tutorial/modules/NUCLEO-L476RG/Makefile`) has to be filled with code as well:

```make
# environment setup
include ../modules.mk

# include apps
include $(APPS_DIR)/Tutorial/Tutorial.mk

# middleware setup
include $(MIDDLEWARE_DIR)/middleware.mk

# C sources
APPS_CSRC += $(Tutorial_CSRC) \
             $(MIDDLEWARE_CSRC) \
             $(realpath .)/apps.c

# C++ sources
APPS_CPPSRC += $(Tutorial_CPPSRC) \
               $(MIDDLEWARE_CPPSRC)

# include directories for configurations
APPS_INC += $(Tutorial_INC) \
            $(MIDDLEWARE_INC) \
            $(realpath .)

# set the build directory
ifeq ($(BUILDDIR),)
  BUILDDIR = $(realpath .)/build
endif
export BUILDDIR

# call Makefile from OS
all:
	$(MAKE) -C $(OS_DIR)/AMiRo-OS/modules/NUCLEO-L476RG/

clean:
	$(MAKE) -C $(OS_DIR)/AMiRo-OS/modules/NUCLEO-L476RG/ clean

flash:
	$(MAKE) -C $(OS_DIR)/AMiRo-OS/modules/NUCLEO-L476RG/ flash
```

> First, all dependencies (environment, applications and middleware) are added, so that all include paths and source files can be aggregated.
> The script furthermore defines the `BUILDDIR` directory, where all compilation output files will be created.
> Finally, the three targets `all`, `clean` and `flash` are defined, which just execute the according GNU Make script in [AMiRo-OS] with the respective target.
> Make sure the paths point to the correct module.

Now that the entry point for the module is ready, you are able to build, flash and wipe the configuration for this module.
Just open a terminal at path `configuration/Tutorial/modules/NUCLEO-L476RG/` (or whatever the individual module directory is called) and execute the following three commands:

1. `make` or `make all`<br>
   This will compile the code and create several binary image files in the newly created `BUILDDIR` directory (e.g. `build/NUCLEO-L476RG.elf`).
   If the command fails, check the error messages and make sure that you have correctly set up a compiler toolchain (cf. [README](../README.md#1-required-software)), all GNU Make scripts are correct and all C files contain valid code.
2. `make clean`<br>
   This command will delete the `BUILDDIR` directory and therefore wipe any temporary build files.
   This can be useful to enforce a completely new build from scratch (also possible via `make -B all`).
3. `make flash`<br>
   If there is no binary image yet or it is out of date (some code has changed), this command will (re)compile the code and create a new image as needed.
   When the image is ready, the system will attempt to flash it to hardware.
   In case the image can not be flashed, check the error messages and make sure that you have a valid flash tool (e.g. OpenOCD) installed and no other similar devices (e.g. other Nucleo development boards) are connected.
   > In case your flash tool complains about permission issues, please refer to its documentation how to permit access to the connected device.
   > On Linux systems for instance, you will probably have to add additional udev rules.
   >
   > Further note, that some Nucleo development boards require the black reset button to be pressed when initiating a flash procedure.

If everything worked well, you should be able to access the device via a serial terminal (e.g. GTKTerm; [AMiRo-OS] uses a baudrate of 115200 for the serial interface) and interact with the [AMiRo-OS] integrated shell.
   
## Configuration

Now that the individual module is ready, the only thing missing is the common entry point for the entire configuration: `configurations/Tutorial/Makefile`.
It is rather a convenience script, which allows to build and wipe all modules of the entire configuration at once, but also flash individual devices from a single location.
Enter the following lines to the script:

```make
# absolue path to this directory
TutorialCONF_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# list of available/supported modules, retrieved by the directories in the modules/ folder
TutorialCONF_MODULES_DIR = $(TutorialCONF_DIR)/modules/
TutorialCONF_MODULES = $(patsubst $(TutorialCONF_MODULES_DIR)%/,%,$(sort $(dir $(wildcard $(TutorialCONF_MODULES_DIR)*/*))))

# generated make targets for cleaning build files and flashing modules
TutorialCONF_CLEANTRGS = $(TutorialCONF_MODULES:%=clean_%)
TutorialCONF_FLASHTRGS = $(TutorialCONF_MODULES:%=flash_%)

# force make to use these targets even if files with identical names exist
.PHONY: all clean $(TutorialCONF_MODULES) $(TutorialCONF_FLASHTRGS) $(TutorialCONF_CLEANTRGS)

# target to build all available modules (using multiple CPU cores)
all:
	$(MAKE) -j$(words $(TutorialCONF_MODULES)) $(TutorialCONF_MODULES)

# target to clean build files of all available modules
clean: $(TutorialCONF_CLEANTRGS)

# targets for all available modules to build individual binary images
$(TutorialCONF_MODULES):
	$(MAKE) -C $(TutorialCONF_MODULES_DIR)/$@

# targets for all available modules to flash individual modules
$(TutorialCONF_FLASHTRGS):
	$(MAKE) -C $(TutorialCONF_MODULES_DIR)/$(@:flash_%=%) flash

# targets for all available modules to clean build files of individual modules
$(TutorialCONF_CLEANTRGS):
	$(MAKE) -C $(TutorialCONF_MODULES_DIR)/$(@:clean_%=%) clean
```

> The two targets `all` and `clean` allow to build and wipe all modules of the entire configuration at once.
> Each module can be accessed individually still by its name, like
> - build: `make NUCLEO-L76RG`
> - clean: `make clean_NUCLEO-L76RG`
> - flash: `make flash_NUCLEO-L76RG`

Try the script and make sure everything works as intended.


# 4 - Periodic Output

Let us now create a simple application, which will print some output whenever it is triggered by an event.
On the configuration level, this event will be emitted periodically by a timer with frequency and duration defined by the user via a shell command.

## Application

Since the application will become more complex in the course of this tutorial, create two new files in the `apps/Tutorial/` directory: `tutorial_master.h` and `tutorial_master.c`.

```
apps/Tutorial/
├── Tutorial.mk
├── tutorial.c
├── tutorial.h
├── tutorial_master.c
└── tutorial_master.h
```

In the `apps/Tutorial/Tutorial.mk` script, add the new C source file to the according variable:

```make
Tutorial_CSRC = $(Tutorial_DIR)/tutorial.c \
                $(Tutorial_DIR)/tutorial_master.c
```

The new header also needs to be included by the common application interface header `apps/Tutorial/tutorial.h`, which also needs to include the [µRT] interface header file.
Furthermore, an event flag value is defined to identify trigger events for periodic message output.
Later in this tutorial, more such flag values will be added to this file.
For now, add the following lines to the `apps/Tutorial/tutorial.h` file:

```c
#include <urt.h>

#define TRIGGERFLAG_MESSAGE (1 << 0)

#include <tutorial_master.h>
```

> Make sure that `tutorial_master.h` is included *after* the definition.
> Even though that order is not relevant so far, it will become critical in the curse of this tutorial.

Next up, fill the two new C files with content:

* **`tutorial_master.h`**
  ```c
  #ifndef TUTORIAL_MASTER_H
  #define TUTORIAL_MASTER_H

  #include <tutorial.h>

  #define STACKSIZE_MASTER 256

  struct tutorial_master {
    URT_THREAD_MEMORY(thread, STACKSIZE_MASTER);
    urt_node_t node;
    urt_osEventSource_t* trigger_source;
    urt_osEventListener_t trigger_listener;
  };

  void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger);

  #endif
  ```
  This file defines `struct tutorial_master`, which is a [µRT] node with its individual thread and event related fields, so it can be triggered from an external source.
  Finally, it declares an initialization function, which is implemented next in the C source file.

* **`tutorial_master.c`**
  ```c
  #include <tutorial.h>

  #define TRIGGEREVENT (urtCoreGetEventMask() << 1)

  urt_osEventMask_t masterSetup(urt_node_t* node, void* master) {
    (void)node;
    urtEventRegister(((struct tutorial_master*)master)->trigger_source, &((struct tutorial_master*)master)->trigger_listener, TRIGGEREVENT, 0);
    return TRIGGEREVENT;
  }

  urt_osEventMask_t masterLoop(urt_node_t* node, urt_osEventMask_t event, void* master) {
    (void)node;

    // handle trigger events
    if (event & TRIGGEREVENT) {
      urt_osEventFlags_t flags = urtEventListenerClearFlags(&((struct tutorial_master*)master)->trigger_listener, ~0);

      // handle message trigger event
      if (flags & TRIGGERFLAG_MESSAGE) {
        urtPrintf("Hello World!\n");
      }
    }

    return TRIGGEREVENT;
  }

  void masterShutdown(urt_node_t* node, urt_status_t reason, void* master) {
    (void)node;
    (void)reason;
    urtEventUnregister(((struct tutorial_master*)master)->trigger_source, &((struct tutorial_master*)master)->trigger_listener);
    return;
  }

  void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger) {
    // initialize node
    urtNodeInit(&master->node, (urt_osThread_t*)master->thread, sizeof(master->thread), prio,
                masterSetup, master,
                masterLoop, master,
                masterShutdown, master);

    // initialize event data
    master->trigger_source = trigger;
    urtEventListenerInit(&master->trigger_listener);

    return;
  }
  ```
  First off, the common application interface header `tutorial.h` is included and an event mask for trigger events is defined.
  Then there are three local functions, which are required by [µRT] nodes.
  Execution of each [µRT] node is subdivided into three stages:

  1. **setup**: Executed right after the node thread starts.<br>
     The `masterSetup()` function registers the event source for trigger events to its according listener and returns a mask of events, which will trigger execution of the first loop.
  2. **loop**: Executed whenever the node thread is triggered by some sort of event or signal.<br>
     For now, the `masterLoop()` function reacts to a trigger event by simply printing a message.
     Again, a mask of events which will trigger execution of the next loop iteration is returned.
  3. **shutdown**: Executed when the node thread is about to terminate.<br>
     On termination, the `masterShutdown()` function just unregisters the trigger event.

  Finally, the implementation of interface function `tutorialMasterInit()` is provided.
  Since the application is a [µRT] node, an according initialization method is called, which employs the three functions above as callbacks for the node thread.
  Furthermore, trigger event related data is initialized as well.

At this point, you should be able to build and flash an image with these new changes.
However, since there is no way to fire a trigger event yet, it is not possible to make the application print its message.
To this end, a new command has to be registered to the [AMiRo-OS] shell by the configuration.

## Configuration

First, include the [µRT] interface header and declare an initialization function in the `configurations/Tutorial/modules/NUCLEO-L476RG/apps.h` header file:

```c++
#include <urt.h>

#if defined(__cplusplus)
extern "C" {
#endif
void appsInit(void);
#if defined(__cplusplus)
}
#endif
```

> The additional C++ logic is needed, because the file will be included by [AMiRo-OS] from within C++ context.
> While this tutorial omits those lines wherever possible, productive code should employ them in all C header files.

Next, add some logic to the `configurations/Tutorial/modules/NUCLEO-L476RG/apps.c` source file:

```c
#include <tutorial.h>
#include <stdlib.h>

#define STOPEVENT (1 << 8)

aos_timer_t triggertimer;
aos_timer_t terminationtimer;
urt_osEventSource_t triggerSource;
urt_osEventSource_t terminationSource;

struct tutorial_master master;

static void triggercb(virtual_timer_t* timer, void* flags)
{
  (void)timer;
  chEvtBroadcastFlagsI(&triggerSource, (urt_osEventFlags_t)flags);
  return;
}

static void terminationcb(virtual_timer_t* timer, void* params)
{
  (void)timer;
  (void)params;
  aosTimerResetI(&triggertimer);
  chEvtBroadcastI(&terminationSource);
  return;
}

int shellcb(BaseSequentialStream* stream, int argc, const char* argv[], urt_osEventFlags_t flags) {
  (void)stream;
  (void)argc;

  // parse arguments
  aos_longinterval_t duration = atof(argv[1]) * MICROSECONDS_PER_SECOND + 0.5f;
  aos_longinterval_t period = (1.0f / atof(argv[2])) * MICROSECONDS_PER_SECOND + 0.5f;

  // register to execution event
  event_listener_t listener;
  aosShellEventRegisterMask(&terminationSource, &listener, STOPEVENT);

  // activate the timers
  aosTimerPeriodicLongInterval(&triggertimer, period, triggercb, (void*)flags);
  aosTimerSetLongInterval(&terminationtimer, duration, terminationcb, NULL);

  // wait for the execution to finish and unregister from execution event
  chEvtWaitOne(STOPEVENT);
  aosShellEventUnregister(&terminationSource, &listener);

  // sleep until all output is printed
  aosThdMSleep(100);

  return AOS_OK;
}

static int shellcb_message(BaseSequentialStream* stream, int argc, const char* argv[]) {
  return shellcb(stream, argc, argv, TRIGGERFLAG_MESSAGE);
}
static AOS_SHELL_COMMAND(shellcmd_message, "Tutorial:PeriodicOutput", shellcb_message);

void appsInit(void) {
  // initialize timers and event sources
  aosTimerInit(&triggertimer);
  aosTimerInit(&terminationtimer);
  urtEventSourceInit(&triggerSource);
  urtEventSourceInit(&terminationSource);

  // initialize nodes
  tutorialMasterInit(&master, URT_THREAD_PRIO_NORMAL_MAX, &triggerSource);

  // add shell commands
  aosShellAddCommand(&shellcmd_message);

  return;
}
```

First, the additional headers `tutorial.h` and `stdlib.h` are included.
The former is the interface to the Tutorial application and the latter provides the standard function `atof()`, which converts a string to a float value.
Next, an event mask is defined, which will be used to terminate the shell command.
Furthermore, several static variables (timers, event sources and the tutorial master node) are declared, before a bunch of functions are defined.

- `triggercb()`<br>
  Callback to be executed by the `triggertimer`.
  Just broadcasts a trigger event.
- `terminationcb()`<br>
  Callback to be executed by the `terminationtimer`.
  It resets/stops the trigger timer and broadcasts a termination event.
- `shellcb()`<br>
  This callback will be executed indirectly by the new command, we are going to register to the [AMiRo-OS] shell.
  It expects two argument strings: duration (in seconds) and frequency (in Hz).
  First, it converts the arguments to interval values in system ticks ([AMiRo-OS] defines a system tick to be 1 µs).
  Then a temporary listener is registered to the termination event before the two timers are started.
  The command - and thereby the shell thread - will now suspend until a termination event (`STOPEVENT`) is received.
  Only then the temporary listener is unregisrtered and the callback function returns.
- `shellcb_message()`<br>
  This is the actual callback, which will be executed by the shell thread.
  It just calls the `shellcb()` function and sets the final argument so the `TRIGGERFLAG_MESSAGE` flag is emitted.

Now that the command callback is defined, it needs to be "wrapped" into a [AMiRo-OS] shell command structure.
This is done by the `AOS_SHELL_COMMAND()` macro, which generates the required data structure.
The new command will be available as `Tutorial:PeriodicOutput` in the [AMiRo-OS] shell.
Last but not least, the `appsInit()` function is implemented.
It initializes all local variables as well as the tutorial master node and finally registers the new shell command.

Before you can try the new application, there is still one important step missing, though.
You need to tell [AMiRo-OS] to execute the initialization function - as well as some further [µRT] related control functions - during startup.
This can be done via the `configurations/Tutorial/modules/NUCLEO-L476RG/osconf.h` configuration file by defining several hook macros:

```c
#define AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER apps.h

#define AMIROOS_CFG_MAIN_INIT_HOOK_2() \
  urtCoreInit(NULL);                   \
  appsInit();

#define AMIROOS_CFG_MAIN_INIT_HOOK_3() \
  urtCoreStartNodes();

#define AMIROOS_CFG_MAIN_SHUTDOWN_HOOK_1() \
  urtCoreStopNodes(URT_STATUS_OK);
```

> These hooks will make [AMiRo-OS] aware of the `apps.h` header file, initialize the [µRT] core component as well as the Tutorial configuration during startup of [AMiRo-OS], start all nodes and stop them on shutdown.
>
> Note that you can induce only a single header file via the `AMIROOS_CFG_MAIN_EXTRA_INCLUDE_HEADER` hook.
> For this reason, the [µRT] interface header `urt.h` had to be included in `apps.h` [earlier](#configuration-3).

You should now be able to build and flash the configuration to your device.
Go ahead and try to execute the new command with various arguments (e.g. `Tutorial:PeriodicOutput 1 10`).
If you encounter any errors or warnings during compilation or flash procedure, check the messages and compare your code to the snippets above.

## Exercise

As you have probably noticed very quickly, the implementation as given above does not apply any sanity checks to the input, so too few or invalid arguments (like a negative frequency) will cause undesired behaviour.
Add some lines, so the application is only triggered if valid arguments are given to the shell command.
If invalid arguments are specified by the user, the `shellcb()` function should return `AOS_INVALIDARGUMENTS` instead of `AOS_OK`.


# 5 - Publish-Subscribe

In this section, you will make two [µRT] nodes communicate according to the publish-subscribe paradigm.
Again, the master node will be triggered periodically by a timer, but instead of printing the whole output by itself, it will publish a message via a topic.
Another node - the slave - will subscribe to that topic and print some output as well, whenever is receives a message.

## Application

First of all, add two lines in `apps/Tutorial/tutorial.h`:

```c
#define TRIGGERFLAG_PUBSUB  (1 << 1)
  ...
#include <tutorial_slave.h>
```

`TRIGGERFLAG_PUBSUB` will be used to identify publish-subscribe events.
The included header file `tutorial_slave.h` does not exist yet, but we will get to that in a minute.
For now, extend the master node first.

In the header file `apps/Tutorial/tutorial_master.h`, add an additional field to the `tutorial_master` structure:

```c
struct tutorial_master {
    ...
  urt_publisher_t publisher;
};
```

Also, the initialization function is extended by an additional argument:

```c
void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger, urt_topicid_t topicid);
```

In the source file `apps/Tutorial/tutorial_master.c`, adapt the initialization function accordingly and initialize the publisher:

```c
void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger, urt_topicid_t topicid) {
    ...
  // initialize publish-subscribe data
  urtPublisherInit(&master->publisher, urtCoreGetTopic(topicid));
    ...
}
```

Finally, the `masterLoop()` function needs to be extended so to handle publish-subscribe events:

```c
urt_osEventMask_t masterLoop(urt_node_t* node, urt_osEventMask_t event, void* master) {
    ...
  if (event & TRIGGEREVENT) {
      ...
    // handle publish-subscribe trigger event
    if (flags & TRIGGERFLAG_PUBSUB) {
      urt_osTime_t t;
      urtTimeNow(&t);
      urtPrintf("Hello?\n");
      urtPublisherPublish(&((struct tutorial_master*)master)->publisher, NULL, 0, &t, URT_PUBLISHER_PUBLISH_DETERMINED);
    }
  }
    ...
}
```

When a publish-subscribe event is received, the output "Hello?" is printed and an empty message is published.

Now that we are done with the master node, create two new files in the `apps/Tutorial/` directory: `tutorial_slave.h` and `tutorial_slave.c`.

```
apps/Tutorial/
├── Tutorial.mk
├── tutorial.c
├── tutorial.h
├── tutorial_master.c
├── tutorial_master.h
├── tutorial_slave.c
└── tutorial_slave.h
```

The `tutorial_slave.h` header is quite similar to its master counterpart:

```c
#ifndef TUTORIAL_SLAVE_H
#define TUTORIAL_SLAVE_H

#include <tutorial.h>

#define STACKSIZE_SLAVE 256

struct tutorial_slave {
  URT_THREAD_MEMORY(thread, STACKSIZE_SLAVE);
  urt_node_t node;
  urt_topicid_t topicid;
  urt_nrtsubscriber_t subscriber;
};

void tutorialSlaveInit(struct tutorial_slave* slave, urt_osThreadPrio_t prio, urt_topicid_t topicid);

#endif
```

The `tutorial_slave.c` source file is structured similar as well, but the logic within the functions is very different, of course.

```c
#include <tutorial.h>

#define PUBLISHEVENT (urtCoreGetEventMask() << 1)

urt_osEventMask_t slaveSetup(urt_node_t* node, void* slave) {
  (void)node;
  urt_topic_t* topic = urtCoreGetTopic(((struct tutorial_slave*)slave)->topicid);
  urtNrtSubscriberSubscribe(&((struct tutorial_slave*)slave)->subscriber, topic, PUBLISHEVENT);
  return PUBLISHEVENT;
}

urt_osEventMask_t slaveLoop(urt_node_t* node, urt_osEventMask_t event, void* slave) {
  (void)node;

  // handle publish event
  if (event & PUBLISHEVENT) {
    urtNrtSubscriberFetchLatest(&((struct tutorial_slave*)slave)->subscriber, NULL, NULL, NULL, NULL);
    urtPrintf("\tWorld!\n");
  }

  return PUBLISHEVENT;
}

void slaveShutdown(urt_node_t* node, urt_status_t reason, void* slave) {
  (void)node;
  (void)reason;
  urtNrtSubscriberUnsubscribe(&((struct tutorial_slave*)slave)->subscriber);
  return;
}

void tutorialSlaveInit(struct tutorial_slave* slave, urt_osThreadPrio_t prio, urt_topicid_t topicid) {
  // initialize node
  urtNodeInit(&slave->node, (urt_osThread_t*)slave->thread, sizeof(slave->thread), prio,
              slaveSetup, slave,
              slaveLoop, slave,
              slaveShutdown, slave);

  // initialize publish-subscribe data
  slave->topicid = topicid;
  urtNrtSubscriberInit(&slave->subscriber);

  return;
}
```

While the `tutorialSlaveInit()` function initializes the [µRT] node and the subscriber, the three execution callbacks handle the actual publish-subscribe communication:

- **setup**:<br>
  The slave node subscribes to the topic via the specified identifier.
- **loop**:<br>
  When a publish event is received, the latest pending message is fetched (thereby dropping any older pending messages) and the output string "World!" is printed.
- **shutdown**:<br>
  The slave node unsubscribes from the topic.

Last but no least, the new C source file `apps/Tutorial/tutorial_slave.c` needs to be added to the GNU Make script, so it will be compiled during the build process:

```make
Tutorial_CSRC = $(Tutorial_DIR)/tutorial.c \
                $(Tutorial_DIR)/tutorial_master.c \
                $(Tutorial_DIR)/tutorial_slave.c
```

## Configuration

As for the configuration, only the `apps.c` file needs to be extended to exploit the new feature of the Tutorial application.
First, a topic identifier has to be defined and instances of a topic and a slave must be added:

```c
#define TOPICID   1

struct tutorial_slave slave;
urt_topic_t topic;
```

Furthermore, another shell command is defined:

```c
static int shellcb_pubsub(BaseSequentialStream* stream, int argc, const char* argv[]) {
  return shellcb(stream, argc, argv, TRIGGERFLAG_PUBSUB);
}
static AOS_SHELL_COMMAND(shellcmd_pubsub, "Tutorial:PubSub", shellcb_pubsub);
```

Finally, the initialiization function needs to be extended as well:

```c
void appsInit(void) {
    ...
  // initialize publish-subscribe data
  urtTopicInit(&topic, TOPICID, NULL);

  // initialize nodes
  tutorialMasterInit(&master, URT_THREAD_PRIO_NORMAL_MAX, &triggerSource, TOPICID);
  tutorialSlaveInit(&slave, URT_THREAD_PRIO_NORMAL_MAX, TOPICID);

  // add shell commands
    ...
  aosShellAddCommand(&shellcmd_pubsub);
    ...
}
```

With those modifications in place, you should now be able to build and flash a new image.
Go ahead and try the `Tutorial:PubSub` command.

## Exercise

[µRT] features multiple types (real-time classes) of communication: NRT (non real-time), SRT (soft real-time), FRT (firm real-time) and HRT (hard real-time).
The code above utilizes only a single NRT subscriber.
Change the real-time class of that subscriber and add further subscribers (of different real-time classes), which reside in the same node and listen to the same topic.


# 6 - Remote Procedure Calls

In this section, you will make the two [µRT] nodes communicate using the remote procedure call (RPC) paradigm.
When triggered by an according event, the master node will request a service, provided by the slave node.
The slave node will then process that request and eventually return a response.

## Application

As with the [publish-subscribe enhancement](#application-4), modify `apps/Tutorial/tutorial.h` by adding a single line, which defines another event flag:

```c
#define TRIGGERFLAG_RPC     (1 << 2)
```

Next up, enhance the master node to implement RPC communication.
In `apps/Tutorial/tutorial_master.h` two new fields are added to the `tutorial_master` structure and the initialization function is extended again by even a further argument:

```c
struct tutorial_master {
    ...
  urt_serviceid_t serviceid;
  urt_nrtrequest_t request;
};

void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger, urt_topicid_t topicid, urt_serviceid_t serviceid);
```

The source file `apps/Tutorial/tutorial_master.c` requires some more extensive modifications this time.
To begin with, adapt the initialization function to match the altered declaration and initialize the newly introduced fields:

```c
void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger, urt_topicid_t topicid, urt_serviceid_t serviceid) {
    ...
  // initialize RPC data
  master->serviceid = serviceid;
  urtNrtRequestInit(&master->request, NULL);
    ...
}
```

Next, we want to make the master node submit requests on RPC trigger events and receive responses by the service.
For the latter, a new event is introduced:

```c
#define SERVICEEVENT (urtCoreGetEventMask() << 2)
```

Now the loop callback can submit a request on RPC trigger events, thereby specify the new `SERVICEEVENT` and process the response when that event is eventually received:

```c
urt_osEventMask_t masterLoop(urt_node_t* node, urt_osEventMask_t event, void* master) {
    ...
  // handle trigger events
  if (event & TRIGGEREVENT) {
      ...
    // handle RPC trigger event
    if (flags & TRIGGERFLAG_RPC) {
      urtPrintf("Hello?\n");
      urt_service_t* service = urtCoreGetService(((struct tutorial_master*)master)->serviceid);
      urtNrtRequestAcquire(&((struct tutorial_master*)master)->request);
      urtNrtRequestSubmit(&((struct tutorial_master*)master)->request, service, 0, SERVICEEVENT);
    }
  }

  // handle service event
  if (event & SERVICEEVENT) {
    urtNrtRequestRetrieve(&((struct tutorial_master*)master)->request, URT_REQUEST_RETRIEVE_ENFORCING, NULL, NULL);
    urtPrintf(":)\n");
    urtNrtRequestRelease(&((struct tutorial_master*)master)->request);
  }

  return TRIGGEREVENT | SERVICEEVENT;
}
```

Obviously, the slave node needs to be extended as well, so it does provide a service the master can actually call.
In the header file `apps/Tutorial/tutorial_slave.h`, modifications are again marginal:

```c
struct tutorial_slave {
    ...
  urt_service_t service;
};

void tutorialSlaveInit(struct tutorial_slave* slave, urt_osThreadPrio_t prio, urt_topicid_t topicid, urt_serviceid_t serviceid);
```

In the source file `apps/Tutorial/tutorial_slave.c` the most obvious change is again the initialization function:

```c
void tutorialSlaveInit(struct tutorial_slave* slave, urt_osThreadPrio_t prio, urt_topicid_t topicid, urt_serviceid_t serviceid) {
    ...
  // initialize RPC data
  urtServiceInit(&slave->service, serviceid, slave->node.thread, REQUESTEVENT);
    ...
}
```

As you can see, the service is initilized with a yet undefined `REQUESTEVENT`.
Thus, define that event and make the loop function process requests:

```c
#define REQUESTEVENT (urtCoreGetEventMask() << 2)

urt_osEventMask_t slaveLoop(urt_node_t* node, urt_osEventMask_t event, void* slave) {
    ...
  // handle request event
  if (event & REQUESTEVENT) {
    urt_service_dispatched_t dispatched;
    urtServiceDispatch(&((struct tutorial_slave*)slave)->service, &dispatched, NULL, NULL, NULL);
    urtPrintf("\tWorld!\n");
    urtServiceAcquireRequest(&((struct tutorial_slave*)slave)->service, &dispatched);
    urtServiceRespond(&dispatched, 0);
  }

  return PUBLISHEVENT | REQUESTEVENT;
}
```

Still, we are not done yet as a small but important detail is missing.
So far the slave node would not react to a request if the loop was not executed yet.
To fix this, the `slaveSetup()` function has to return `REQUESTEVENT` as well:

```c
urt_osEventMask_t slaveSetup(urt_node_t* node, void* slave) {
    ...
  return PUBLISHEVENT | REQUESTEVENT;
}
```

## Configuration

Similar to the [publish-subscribe enhancement](#configuration-4), only `apps.c` requires some more lines of code, which define a service identifier as well as another shell command, and the initialization function needs to be extended once more:

```c
#define SERVICEID 1

static int shellcb_rpc(BaseSequentialStream* stream, int argc, const char* argv[]) {
  return shellcb(stream, argc, argv, TRIGGERFLAG_RPC);
}
static AOS_SHELL_COMMAND(shellcmd_rpc, "Tutorial:RPC", shellcb_rpc);

void appsInit(void) {
    ...
  // initialize nodes
  tutorialMasterInit(&master, URT_THREAD_PRIO_NORMAL_MAX, &triggerSource, TOPICID, SERVICEID);
  tutorialSlaveInit(&slave, URT_THREAD_PRIO_NORMAL_MAX, TOPICID, SERVICEID);

  // add shell commands
    ...
  aosShellAddCommand(&shellcmd_rpc);
    ...
}
```

That is all and you should be able to build and flash the new image.
Go ahead and try the new `Tutorial:RPC` command.

## Exercise

Again, the code above only uses a single NRT request.
Change the real-time class of that request and make the master node submit multiple requests of different real-time classes to the same service.


# 7 - Custom Message String

So far, all shell commands only print constant strings, which are defined at compile time by the code.
In this section, the Tutorial application is enhanced to allow a custom message and the configuration is adapted to provide an according user interface.

## Application

First off, another line is added to the `apps/Tutorial/tutorial.h` file, defining the maximum length of the custom message string:

```c
#define PAYLOAD_SIZE 64
```

Once again, the `tutorial_master` structure is extended by two further fields:

```c
struct tutorial_master {
    ...
  char payload[PAYLOAD_SIZE];
  char message[PAYLOAD_SIZE];
};
```

While `payload` will be associated to the request to pass the message string to the service, `message` serves as input and will be set externally by the configuration code (i.e. `apps.c`) later on.

The master node logic also needs to be updated to utilize those new fields:

```c
urt_osEventMask_t masterLoop(urt_node_t* node, urt_osEventMask_t event, void* master) {
    ...
  if (event & TRIGGEREVENT) {
      ...
    // handle message trigger event
    if (flags & TRIGGERFLAG_MESSAGE) {
      urtPrintf("Hello %s!\n", ((struct tutorial_master*)master)->message);
    }

    // handle publish-subscribe trigger event
    if (flags & TRIGGERFLAG_PUBSUB) {
         ...
      urtPublisherPublish(&((struct tutorial_master*)master)->publisher, &((struct tutorial_master*)master)->message, PAYLOAD_SIZE, &t, URT_PUBLISHER_PUBLISH_DETERMINED);
    }

    // handle RPC trigger event
    if (flags & TRIGGERFLAG_RPC) {
        ...
      strncpy(urtNrtRequestGetPayload(&((struct tutorial_master*)master)->request), ((struct tutorial_master*)master)->message, PAYLOAD_SIZE);
      urtNrtRequestSubmit(&((struct tutorial_master*)master)->request, service, PAYLOAD_SIZE, SERVICEEVENT);
    }
  }
    ...
}

void tutorialMasterInit(struct tutorial_master* master, urt_osThreadPrio_t prio, urt_osEventSource_t* trigger, urt_topicid_t topicid, urt_serviceid_t serviceid) {
    ...
  // initialize RPC data
    ...
  urtNrtRequestInit(&master->request, master->payload);
    ...
}
```

Analogously, the slave node must have its own buffer to store incoming messages to:

```c
struct tutorial_slave {
    ...
  char message[PAYLOAD_SIZE];
};
```

The logic consequently needs to store incoming data to that buffer and print its contents:

```c
urt_osEventMask_t slaveLoop(urt_node_t* node, urt_osEventMask_t event, void* slave) {
    ...
  // handle publish event
  if (event & PUBLISHEVENT) {
    urtNrtSubscriberFetchLatest(&((struct tutorial_slave*)slave)->subscriber, ((struct tutorial_slave*)slave)->message, NULL, NULL, NULL);
    urtPrintf("\t%s!\n", ((struct tutorial_slave*)slave)->message);
  }

  // handle request event
  if (event & REQUESTEVENT) {
      ...
    urtServiceDispatch(&((struct tutorial_slave*)slave)->service, &dispatched, ((struct tutorial_slave*)slave)->message, NULL, NULL);
    urtPrintf("\t%s!\n", ((struct tutorial_slave*)slave)->message);
      ...
  }
    ...
}
```

## Configuration

In order to pass a custom message from a publisher to a subscriber, an according buffer must be associated to the `topic` in the `apps.c` file:

```c
char payload[PAYLOAD_SIZE];

void appsInit(void) {
    ...
  // initialize publish-subscribe data
  urtTopicInit(&topic, TOPICID, payload);
    ...
}
```

Finally, the `shellcb()` function needs to evaluate a third (optional) command line argument:

```c
int shellcb(BaseSequentialStream* stream, int argc, const char* argv[], urt_osEventFlags_t flags) {
    ...
  // parse arguments
    ...
  if (argc > 3) {
    strncpy(master.message, argv[3], PAYLOAD_SIZE);
  } else {
    strncpy(master.message, "World", PAYLOAD_SIZE);
  }
    ...
}
```

Build and flash the new image and make the system print the custom messages.

## Exercise

So far, the complete message buffer will be copied with each communication, even though the actual message string is much shorter.
Optimize the code, so only as many bytes as required are transmitted.

--------------------------------------------------------------------------------

# Congratulations!

This completes the AMiRo-Apps tutorial.
You have learned to create an empty application and configuration from scratch and to configure the build process.
You created [µRT] nodes and made them periodically print a custom string, thereby exploiting publish-subscribe and RPC communication.

If you are interested in the sample solution of the tutorial, such is provided [here](Tutorial.zip).
Furthermore, you can check out the [HelloWorld application](../apps/HelloWorld/) and [configuration](../configurations/HelloWorld/), which features more advanced code and provides performance metrics to the user.
