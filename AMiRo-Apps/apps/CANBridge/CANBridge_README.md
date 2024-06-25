About & License
===============

CANBridge is an application for the Autonomous
Mini Robot (AMiRo) [1]. It is tightly coupled to the internal CANBus of the AMiRo and acts as the main connection between the three modules DiWheelDrive, PowerManagement and LightRing. For this it sends Data over the CANBus and receives Data from the CANBus. 

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

- Svenja Kenneweg


Send data over the CANBus
-------

After adding the CANBridge to your configuration you can send PubSub data over it by defining a `canBridge_subscriber_list_t` object which is the second argument of the `canBridgeInit` function. The structure of the `canBridge_subscriber_list_t` is as following: 
```c
typedef struct canBridge_subscriber_list {
  urt_nrtsubscriber_t* subscriber;
  urt_osEventMask_t* mask;
  size_t payload_size;
  urt_topicid_t topic_id;
  struct canBridge_subscriber_list* next;
}canBridge_subscriber_list_t;
```

When you want to send Service data over the internal CANBus you need to define a `canBridge_service_list_t` object which is the fifth argument of the `canBridgeInit` function. The structure of the `canBridge_service_list_t` is as following:

```c
typedef struct canBridge_service_list {
  urt_service_t* service;
  size_t payload_size;
  struct canBridge_service_list* next;
}canBridge_service_list_t;
```


Receive data from the CANBus
-------
When you want to receive PubSub data from the internal CANBus you need to define a `urt_topicid_t` array containing the topic ids where you want to receive data from. This is the third argument of the `canBridgeInit` function. Also you need to set the fourth argument which is the number of topic ids in your `urt_topicid_t` array. <br>
Receiving data from Service/Request data is similar: You need to define a `urt_serviceid_t` array containing the service ids where you want to receive data from. This is the sixth argument of the `canBridgeInit` function. Also you need to set the eighth argument which is the number of service ids in your `urt_serviceid_t` array.


CANFrames
-------
Each CANFrame has a 29 bit identifier and an array of size eight where each membery contains eight bit. The identifier is composed as follows:
```c
Bit 0 -3 : board_id (DiWheelDrive = 0, PowerManagement = 1, LightRing = 2, Unknown = 3)
Bit 3 -7 : type (topic data = 0, Request = 1(nrt)/3(frt)/5(srt)/7(hrt), Request Answer = 8)
Bit 7 -23: topic/service id 
Bit 23-29: counter (Meta Frame = 0, Others = Highest,...,1)
```
Before the data is sent, a "meta" frame is sent, whose payload having in total 8*8 = 64 Bits is structured as follows:
```c
Bit 0 -58: Timestamp 
Bit 58-59: fire and forget (for service/request data)
Bit 59-64: total number of frames which are send after the meta frame
```
