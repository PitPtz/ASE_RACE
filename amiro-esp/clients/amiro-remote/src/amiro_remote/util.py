import sys
import argparse
import random
from typing import Tuple
from socket import gethostbyname

from amiro_remote.amiro import *


def parse_addresses() -> Tuple[str, int]:
    parser = argparse.ArgumentParser()
    parser.add_argument("--hostname", help="AMiRo Hostname [amiro1, amiro2]")
    parser.add_argument("--ip", help='AMiRo IP')
    parser.add_argument("--port", default=1234, help="tcp port")

    args = parser.parse_args()

    if args.ip and args.hostname:
        print("Use hostname or ip, not both")

    if not args.ip and not args.hostname:
        print("You need to specify an ip or hostname")
        sys.exit(-1)

    if args.hostname:
        args["ip"] = gethostbyname(args.hostname)
        print(f"IP: {args['ip']}")

    return args.ip, args.port


def generate_value(format_char):
    if format_char == 'H':
        return random.randrange(0, 2**16)
    if format_char == 'f':
        return random.uniform(-1, 1)


def generate_dummy_data(topic):
    return [generate_value(format_char) for format_char in topic_layout[topic]]
