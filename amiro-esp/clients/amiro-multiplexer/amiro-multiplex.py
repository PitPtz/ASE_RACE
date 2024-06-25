#!/usr/bin/env python3
import asyncio
import argparse
import socket
import sys
import traceback
from concurrent.futures import TimeoutError

parser = argparse.ArgumentParser(description='')
parser.add_argument('ip', help='AMiRo IP')
parser.add_argument('--port', default=1234, help='tcp port')
parser.add_argument('--allow-client-writes', action="store_true")
parser.add_argument('--server-ip', default="0.0.0.0", help='server ip')
parser.add_argument('--server-port', default=1234, help='server port')
parser.add_argument('--timeout', default=None, type=float)

args = parser.parse_args()


#                            |------> Client 1
#                            |
# AMiRo <----> TCPServer <---|------> Client 2
#                            |
#                            |------> Client 3

async def test():
    test = await self.sock_reader.read(20)
    return test


class TCPServer:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.clients = []

    async def handle_client(self, reader, writer):
        self.clients.append(writer)
        try:
            while True:
                data = await reader.read(20)
                if not data:
                    break
                if args.allow_client_writes:
                    self.sock_writer.write(data)
        except ConnectionResetError:
            print("Client disconnect")
        writer.close()
        self.clients.remove(writer)

    async def broadcast(self, message):
        for client in self.clients:
            client.write(message)
            await client.drain()

    async def start(self):
        server = await asyncio.start_server(
            self.handle_client, self.host, self.port
        )
        print(f"Server started on {self.host}:{self.port}")

        async with server:
            await server.serve_forever()

    async def connect_esp(self):
        self.sock_reader, self.sock_writer = await asyncio.open_connection(args.ip, args.port) #socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (args.ip, args.port)
        print('connected to {} port {}'.format(*server_address))

    async def socket_client_loop(self):
        await self.connect_esp()
        #try:
        #    sock.connect(server_address)
        #except OSError:
        #    print("Could not connect to %s:%s" % server_address)
        #    print("You need to be in the CITEC wifi or cable network (not eduroam)")
        #    traceback.print_exc()
        #    sys.exit(-1)
        try:
            while True:
                try:
                    data = await asyncio.wait_for(self.sock_reader.read(20),timeout=args.timeout)
                except (TimeoutError, asyncio.TimeoutError):
                    print("ESP connection timeout. Reconnecting..")
                    await self.connect_esp()
                    continue
                except (BrokenPipeError):
                    print("ESP connection broken. Reconnecting..")
                    await self.connect_esp()
                    continue
                if len(data)==0: # disconnect
                    print("Empty")
                #data = await self.sock_reader.read(20) #CAN struct size is 20
                await self.broadcast(data)
        except Exception as e:
            print("Unexpected error:", sys.exc_info()[0])
            traceback.print_exc()
            print(f"Error closing reader: {e}")
        finally:
            print('closing socket')
            #sock.close()
            try:
                self.sock_writer.close()
                print('Writer closed')
            except Exception as e:
                print(f"Error closing writer: {e}")
                traceback.print_exc()
            try:
                if not self.sock_reader.at_eof():
                    self.sock_reader.feed_eof()
                    print('Reader closed')
            except Exception as e:
                print(f"Error closing reader: {e}")
                traceback.print_exc()

server = TCPServer(args.server_ip, args.server_port)

async def start_servers():
    await asyncio.gather(
        server.start(),
        server.socket_client_loop()
    )

asyncio.run(start_servers())
