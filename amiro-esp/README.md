# ESPAMiRo

This repository contains everything related to the ESP module of an AMiRo. The ESP is a microcontroller with an integrated wifi module, to allow access to the CAN bus of an AMiRo remotely.


## Contents 

+ The files to flash the ESPBoard are located in `ESP/`.
+ The Python package `amiro-remote`, located in `Clients/amiro-remote`, allows to connect with an ESP over wifi and read/write to the CAN bus remotely. It provides easy to use methods to retrieve sensor data and request services, mirroring the C-definitions in AMiRo-Apps. 
+ ROS 2 AMiRo-packages that expose the remote functionality via a ROS 2 nodes are located in `ros2_amiro_ws/`. The AMiRo's sensor data is published as topics, and services (such as light and motor) can be requested. It also provides mouse teleop of an AMiRo with a gui interface. 


## Folder Structure
 
``` 
main
|   README.md                   # Contains TOC, FAQs 
└── ESP (-> PlatformIO)         # Contains the PlatformIO-Project (all files necessary to flash the ESPBoard)
└── Clients                     # Contains all Client Applications
│   │   Clients.md              # Description of all Applications
|   └── linux_dev               # VirtualCan Application:  Mirrors CAN traffic on virtual can device
│   └── Logger                  # can_print.py: simple program to print can traffic
│   └── amiro-remote            # Python package to interact remotely with an AMiRo
│   └── highscore-project       # all Files of highscore-project
└── ros2_amiro_ws               # ROS 2 workspace containing AMiRo related packages
│   └── src
│   │   └── amiro_ros           # ROS 2 AMiRo main package
│   │   └── amiro_ros_msgs      # Seperate package containing related messages and services
|   |   └── teleop_tools        # Teleop package (mouse, keyboard)
```

## Setup

To provide the ESP module with power from the AMiRo, you have to add an instruction to the LightRing module inside the AMiRo-OS of your AMiRo-Apps directory. Add the following line to `AMiRo-Apps/os/AMiRo-OS/modules/LightRing_1-2/module.c`:
```C
...
563
564  // ESP Power
565  mic9404x_lld_set(&moduleLldPowerSwitchV50, MIC9404x_LLD_STATE_ON);
566
...
```

