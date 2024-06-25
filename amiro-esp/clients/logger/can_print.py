#!/usr/bin/env python3
import socket
import sys
import time
import argparse
import struct
import traceback

parser = argparse.ArgumentParser(description='Recv CAN Messages over TCP')
parser.add_argument('--hostname', help='AMiRo Hostname [amiro1, amiro2]')
parser.add_argument('--ip', help='AMiRo IP')
parser.add_argument('--port', default=1234, help='tcp port')

args = parser.parse_args()

if args.ip and args.hostname:
    print("Use hostname or ip, not both")

if not args.ip and not args.hostname:
    print("You need to specify an ip or hostname")
    sys.exit(-1)

if args.hostname:
    args['ip'] = socket.gethostbyname(args.hostname)
    print('IP: %s' % args['ip'])

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (args.ip, args.port)
print('connecting to {} port {}'.format(*server_address))
try:
    sock.connect(server_address)
except OSError:
    print("Could not connect to %s:%s" % server_address)
    print("You need to be in the CITEC wifi or cable network (not eduroam)")
    traceback.print_exc()
    sys.exit(-1)
try:
    while True:
        data = sock.recv(20, socket.MSG_WAITALL) #CAN struct size is 20 ,see twai_message_t (esp-idf)
        msg = struct.unpack('IIBBBBBBBBBxxx',data) #Flags, Identifier, Length, Data[8], 3 bytes pad to 32bit?
        print(msg)
        #todo send

finally:
    print('closing socket')
    sock.close()
