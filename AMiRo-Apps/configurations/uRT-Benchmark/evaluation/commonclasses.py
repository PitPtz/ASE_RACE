"""
AMiRo-Apps is a collection of applications and configurations for the
Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2018..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU (Lesser) General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU (Lesser) General Public License for more details.

You should have received a copy of the GNU (Lesser) General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import datetime
import enum
import time

#-------------------------------------------------------------------------------
# local functions and classes
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# interface functions and classes
#-------------------------------------------------------------------------------

class RTClass(enum.Enum):
    NRT = enum.auto()
    SRT = enum.auto()
    FRT = enum.auto()
    HRT = enum.auto()

    def fromstring(s):
        if str(s).upper() == 'NRT':
            return RTClass.NRT
        elif str(s).upper() == 'SRT':
            return RTClass.SRT
        elif str(s).upper() == 'FRT':
            return RTClass.FRT
        elif str(s).upper() == 'HRT':
            return RTClass.HRT
        else:
            raise ValueError('invalid argument')

    def __str__(self):
        if self == RTClass.NRT:
            return 'NRT'
        elif self == RTClass.SRT:
            return 'SRT'
        elif self == RTClass.FRT:
            return 'FRT'
        elif self == RTClass.HRT:
            return 'HRT'
        else:
            raise ValueError('invalid object')

class Benchmark(enum.Enum):
    SYSTEM_KERNEL           = enum.auto()
    SYSTEM_MODULE           = enum.auto()
    URT_MEMORY              = enum.auto()
    URT_NODE                = enum.auto()
    URT_PUBSUB_HOPS         = enum.auto()
    URT_PUBSUB_PAYLOAD      = enum.auto()
    URT_PUBSUB_MESSAGE      = enum.auto()
    URT_PUBSUB_PUBLISHER    = enum.auto()
    URT_PUBSUB_TOPIC        = enum.auto()
    URT_PUBSUB_SUBSCRIBER   = enum.auto()
    URT_PUBSUB_QOS_DEADLINE = enum.auto()
    URT_PUBSUB_QOS_JITTER   = enum.auto()
    URT_PUBSUB_QOS_RATE     = enum.auto()
    URT_RPC_HOPS            = enum.auto()
    URT_RPC_PAYLOAD         = enum.auto()
    URT_RPC_REQUEST         = enum.auto()
    URT_RPC_SERVICE         = enum.auto()
    URT_RPC_QOS_DEADLINE    = enum.auto()
    URT_RPC_QOS_JITTER      = enum.auto()

    def __str__(self):
        if self == Benchmark.SYSTEM_KERNEL:
            return 'system_kernel'
        elif self == Benchmark.SYSTEM_MODULE:
            return 'system_module'
        elif self == Benchmark.URT_MEMORY:
            return 'uRT_memory'
        elif self == Benchmark.URT_NODE:
            return 'uRT_node'
        elif self == Benchmark.URT_PUBSUB_HOPS:
            return 'uRT_PubSub_hops'
        elif self == Benchmark.URT_PUBSUB_PAYLOAD:
            return 'uRT_PubSub_payload'
        elif self == Benchmark.URT_PUBSUB_MESSAGE:
            return 'uRT_PubSub_message'
        elif self == Benchmark.URT_PUBSUB_PUBLISHER:
            return 'uRT_PubSub_publisher'
        elif self == Benchmark.URT_PUBSUB_TOPIC:
            return 'uRT_PubSub_topic'
        elif self == Benchmark.URT_PUBSUB_SUBSCRIBER:
            return 'uRT_PubSub_subscriber'
        elif self == Benchmark.URT_PUBSUB_QOS_DEADLINE:
            return 'uRT_PubSub_QoS_deadline'
        elif self == Benchmark.URT_PUBSUB_QOS_JITTER:
            return 'uRT_PubSub_QoS_jitter'
        elif self == Benchmark.URT_PUBSUB_QOS_RATE:
            return 'uRT_PubSub_QoS_rate'
        elif self == Benchmark.URT_RPC_HOPS:
            return 'uRT_RPC_hops'
        elif self == Benchmark.URT_RPC_PAYLOAD:
            return 'uRT_RPC_payload'
        elif self == Benchmark.URT_RPC_REQUEST:
            return 'uRT_RPC_request'
        elif self == Benchmark.URT_RPC_SERVICE:
            return 'uRT_RPC_service'
        elif self == Benchmark.URT_RPC_QOS_DEADLINE:
            return 'uRT_RPC_QoS_deadline'
        elif self == Benchmark.URT_RPC_QOS_JITTER:
            return 'uRT_RPC_QoS_jitter'
        else:
            raise ValueError('invalid object')

    def fromstring(s: str):
        s = s.upper()

        if s == 'ALL':
            return [Benchmark.URT_MEMORY, Benchmark.URT_NODE,
                    Benchmark.URT_PUBSUB_HOPS, Benchmark.URT_PUBSUB_PAYLOAD, Benchmark.URT_PUBSUB_MESSAGE, Benchmark.URT_PUBSUB_PUBLISHER, Benchmark.URT_PUBSUB_TOPIC, Benchmark.URT_PUBSUB_SUBSCRIBER, Benchmark.URT_PUBSUB_QOS_DEADLINE, Benchmark.URT_PUBSUB_QOS_JITTER, Benchmark.URT_PUBSUB_QOS_RATE,
                    Benchmark.URT_RPC_HOPS, Benchmark.URT_RPC_PAYLOAD, Benchmark.URT_RPC_REQUEST, Benchmark.URT_RPC_SERVICE, Benchmark.URT_RPC_QOS_DEADLINE, Benchmark.URT_RPC_QOS_JITTER,
                    Benchmark.SYSTEM_KERNEL, Benchmark.SYSTEM_MODULE]
        elif s == 'URT':
            return [Benchmark.URT_MEMORY, Benchmark.URT_NODE,
                    Benchmark.URT_PUBSUB_HOPS, Benchmark.URT_PUBSUB_PAYLOAD, Benchmark.URT_PUBSUB_MESSAGE, Benchmark.URT_PUBSUB_PUBLISHER, Benchmark.URT_PUBSUB_TOPIC, Benchmark.URT_PUBSUB_SUBSCRIBER, Benchmark.URT_PUBSUB_QOS_DEADLINE, Benchmark.URT_PUBSUB_QOS_JITTER, Benchmark.URT_PUBSUB_QOS_RATE,
                    Benchmark.URT_RPC_HOPS, Benchmark.URT_RPC_PAYLOAD, Benchmark.URT_RPC_REQUEST, Benchmark.URT_RPC_SERVICE, Benchmark.URT_RPC_QOS_DEADLINE, Benchmark.URT_RPC_QOS_JITTER]
        elif s == 'KERNEL':
            return [Benchmark.SYSTEM_KERNEL]
        elif s == 'MODULE':
            return [Benchmark.SYSTEM_MODULE]
        elif s == 'MEMORY':
            return [Benchmark.URT_MEMORY]
        elif s == 'NODE':
            return [Benchmark.URT_NODE]
        elif s == 'PUBSUB':
            return [Benchmark.URT_PUBSUB_HOPS, Benchmark.URT_PUBSUB_PAYLOAD, Benchmark.URT_PUBSUB_MESSAGE, Benchmark.URT_PUBSUB_PUBLISHER, Benchmark.URT_PUBSUB_TOPIC, Benchmark.URT_PUBSUB_SUBSCRIBER, Benchmark.URT_PUBSUB_QOS_DEADLINE, Benchmark.URT_PUBSUB_QOS_JITTER, Benchmark.URT_PUBSUB_QOS_RATE]
        elif s == 'PUBSUB-HOPS':
            return [Benchmark.URT_PUBSUB_HOPS]
        elif s == 'PUBSUB-PAYLOAD':
            return [Benchmark.URT_PUBSUB_PAYLOAD]
        elif s == 'PUBSUB-MESSAGE':
            return [Benchmark.URT_PUBSUB_MESSAGE]
        elif s == 'PUBSUB-PUBLISHER':
            return [Benchmark.URT_PUBSUB_PUBLISHER]
        elif s == 'PUBSUB-TOPIC':
            return [Benchmark.URT_PUBSUB_TOPIC]
        elif s == 'PUBSUB-SUBSCRIBER':
            return [Benchmark.URT_PUBSUB_SUBSCRIBER]
        elif s == 'PUBSUB-QOS':
            return [Benchmark.URT_PUBSUB_QOS_DEADLINE, Benchmark.URT_PUBSUB_QOS_JITTER, Benchmark.URT_PUBSUB_QOS_RATE]
        elif s == 'PUBSUB-QOS-DEADLINE':
            return [Benchmark.URT_PUBSUB_QOS_DEADLINE]
        elif s == 'PUBSUB-QOS-JITTER':
            return [Benchmark.URT_PUBSUB_QOS_JITTER]
        elif s == 'PUBSUB-QOS-RATE':
            return [Benchmark.URT_PUBSUB_QOS_RATE]
        elif s == 'RPC':
            return [Benchmark.URT_RPC_HOPS, Benchmark.URT_RPC_PAYLOAD, Benchmark.URT_RPC_REQUEST, Benchmark.URT_RPC_SERVICE, Benchmark.URT_RPC_QOS_DEADLINE, Benchmark.URT_RPC_QOS_JITTER]
        elif s == 'RPC-HOPS':
            return [Benchmark.URT_RPC_HOPS]
        elif s == 'RPC-PAYLOAD':
            return [Benchmark.URT_RPC_PAYLOAD]
        elif s == 'RPC-REQUEST':
            return [Benchmark.URT_RPC_REQUEST]
        elif s == 'RPC-SERVICE':
            return [Benchmark.URT_RPC_SERVICE]
        elif s == 'RPC-QOS':
            return [Benchmark.URT_RPC_QOS_DEADLINE, Benchmark.URT_RPC_QOS_JITTER]
        elif s == 'RPC-QOS-DEADLINE':
            return [Benchmark.URT_RPC_QOS_DEADLINE]
        elif s == 'RPC-QOS-JITTER':
            return [Benchmark.URT_RPC_QOS_JITTER]
        else:
            raise ValueError('invalid argument')

    def toShellCommand(self):
        root = 'uRT-Benchmark:'
        pubsub = root + 'PubSub '
        rpc = root + 'RPC '

        if self == Benchmark.SYSTEM_KERNEL:
            return 'kernel:test'
        elif self == Benchmark.SYSTEM_MODULE:
            return 'module:info'
        elif self == Benchmark.URT_MEMORY:
            return root + 'Memory'
        elif self == Benchmark.URT_NODE:
            return root + 'Node'
        elif self == Benchmark.URT_PUBSUB_HOPS:
            return pubsub + '--hops'
        elif self == Benchmark.URT_PUBSUB_PAYLOAD:
            return pubsub + '--payload'
        elif self == Benchmark.URT_PUBSUB_MESSAGE:
            return pubsub + '--message'
        elif self == Benchmark.URT_PUBSUB_PUBLISHER:
            return pubsub + '--publisher'
        elif self == Benchmark.URT_PUBSUB_TOPIC:
            return pubsub + '--topic'
        elif self == Benchmark.URT_PUBSUB_SUBSCRIBER:
            return pubsub + '--subscriber'
        elif self == Benchmark.URT_PUBSUB_QOS_DEADLINE:
            return pubsub + '--qos deadline'
        elif self == Benchmark.URT_PUBSUB_QOS_JITTER:
            return pubsub + '--qos jitter'
        elif self == Benchmark.URT_PUBSUB_QOS_RATE:
            return pubsub + '--qos rate'
        elif self == Benchmark.URT_RPC_HOPS:
            return rpc + '--hops'
        elif self == Benchmark.URT_RPC_PAYLOAD:
            return rpc + '--payload'
        elif self == Benchmark.URT_RPC_REQUEST:
            return rpc + '--request'
        elif self == Benchmark.URT_RPC_SERVICE:
            return rpc + '--service'
        elif self == Benchmark.URT_RPC_QOS_DEADLINE:
            return rpc + '--qos deadline'
        elif self == Benchmark.URT_RPC_QOS_JITTER:
            return rpc + '--qos jitter'
        else:
            raise ValueError('invalid object')

def waitForFilesystem(arg):
    if arg == None:
        return
    if not type(arg) is list:
        arg = [arg]
    all_done = False
    while all_done == False:
        all_done = True
        for i in arg:
            if not i.exists():
                all_done = False
                time.sleep(1)
                break
    return

def log(*args, **kwargs):
    print('['+str(datetime.datetime.now())+']', *args, **kwargs)

#-------------------------------------------------------------------------------
# script
#-------------------------------------------------------------------------------
