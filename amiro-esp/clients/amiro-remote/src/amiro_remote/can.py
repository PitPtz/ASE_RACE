from __future__ import annotations
from amiro_remote.amiro import AMiRoDef
from dataclasses import dataclass
from typing import Tuple, Any, Union, Iterable, Optional
from collections import defaultdict
from time import sleep, time
import socket
import struct
from ctypes import Structure, c_uint32, c_uint8, sizeof


def getdict(struct):
    result = {}
    for field, _ in struct._fields_:
        value = getattr(struct, field)
        if (type(value) not in [int, float, bool]) and not bool(value):
            value = None
        elif hasattr(value, "_length_") and hasattr(value, "_type_"):
            value = list(value)
        elif hasattr(value, "_fields_"):
            value = getdict(value)
        result[field] = value
    return result


def flatten_dict(d, result, prefix=""):
    for k, v in d.items():
        if type(v) == dict:
            flatten_dict(v, result, prefix + k + "_")
        else:
            result[prefix+k] = v


@dataclass
class TopicData:
    """Store topic specific data that is written onto the CAN."""
    topic: Any
    payload: Any
    timestamp_amiro: int
    timestamp_client: int

    def __iter__(self):
        return iter((self.topic, self.payload, self.timestamp_amiro, self.timestamp_client))


class CANMessageIn(Structure):
    _fields_ = [("flags", c_uint32),
                ("counter", c_uint32, 6),
                ("id", c_uint32, 16),
                ("type", c_uint32, 4),
                ("board_id", c_uint32, 3),
                # ("dummy", c_uint32, 3),
                ("length", c_uint8),
                ("payload", c_uint8*8),]

    def key(self) -> Tuple[int, int, int]:
        """Generate a key to identify messages."""
        return (self.board_id, self.type, self.id)

    def __str__(self) -> str:
        return f"flags: {self.flags}, identifier: {self.key()}, counter: {self.counter}" + \
               f", length: {self.length}, payload: {self.payload}"


class SocketFromFile:
    def connect(self, file_path: str):
        self.fd = open(file_path, "rb")

    def settimeout(self, timeout):
        pass

    def recv(self, num_bytes):
        return self.fd.read(num_bytes)

    def close(self):
        self.fd.close()


class CANReaderWriter:
    """Connect with the ESP module of an AMiRo to read and write from CAN remotely."""

    def __init__(self,
                 address: Union[tuple[str, int], str],
                 amiro_def: AMiRoDef,
                 filter_topics: Optional[Iterable[Any]] = None) -> None:
        """Initialize with ESP address and the topics to listen for."""
        self.amiro_def = amiro_def
        self.address = address
        if filter_topics is None or len(filter_topics) == 0:
            self.topics = list(amiro_def.Topic)
        else:
            self.topics = filter_topics
        self.all_topics = list(amiro_def.Topic)
        self.buffer = defaultdict(bytes)
        self.buffer_metadata = dict()
        self.data = defaultdict(list)
        self.sock = None

    def __enter__(self) -> "CANReaderWriter":
        """Support context management for ESP connection."""

        self.connect()
        return self

    def __exit__(self, type, value, traceback) -> None:
        """Support context management for ESP connection."""
        self.disconnect()

    def connect(self) -> None:
        """Connect to the ESP."""

        if type(self.address) is str:
            self.sock = SocketFromFile()
        else:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(5)
        self.sock.connect(self.address)
        linger_enabled = 1
        linger_time = 10 #This is in seconds.
        linger_struct = struct.pack('ii', linger_enabled, linger_time)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, linger_struct)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 20*10)
        #print(self.sock.getsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF))

    def disconnect(self) -> None:
        """Disconnect from the ESP."""

        if self.sock is not None:
            self.sock.shutdown(socket.SHUT_RDWR)
            self.sock.close()
            self.sock = None

    def print_loop(self, topics=None) -> None:
        """Continuously receive and p
        rint CAN messages."""

        with self:
            while True:
                data = self.get()
                if data is not None:
                    if topics is None or data.topic in topics:
                        print(data)

    def get(self, non_blocking=False) -> TopicData:
        """Return the next topic data that could be reconstructed from messages received from the ESP."""
        # non_blocking -> return None if recv would block

        while True:
            if non_blocking:
                old_mode = self.sock.getblocking()
                self.sock.setblocking(False)
            try:
                data = self.sock.recv(self.amiro_def.CAN_MESSAGE_SIZE, socket.MSG_WAITALL)
            except BlockingIOError:
                if non_blocking:
                    return None
            if non_blocking:
                self.sock.setblocking(old_mode)
            assert len(data)==self.amiro_def.CAN_MESSAGE_SIZE
            msg = CANMessageIn.from_buffer_copy(data)
            if self._check_message_in(msg):  # check if message can be handled
                key = msg.key()
                if msg.counter == 0:  # meta frame
                    timestamp = struct.unpack("Q", bytes(msg.payload[:self.amiro_def.CAN_PAYLOAD_SIZE-1]) + bytes(1))[0]
                    meta_raw = struct.unpack("B", bytes(msg.payload[-1:]))[0]
                    fire_and_forget = meta_raw & 1
                    frame_count = meta_raw >> 1
                    self.buffer_metadata[key] = (timestamp, fire_and_forget, frame_count)
                else:  # data frame
                    self.buffer[key] += bytes(msg.payload[:msg.length])  # add bytes to buffer

                # original data can be reconstructed now, counter counts downwards!
                if msg.counter == 1 and key in self.buffer_metadata:
                    data_bytes = self.buffer.pop(key)
                    timestamp, _, frame_count = self.buffer_metadata.pop(key)
                    target_size = sizeof(self.amiro_def.MessageDefs[0][key[2]]["class"])
                    size = len(data_bytes)
                    if (size < target_size):
                        print(f"Warning got a message with id {key[2]} that is shorter than expected! "
                              f"Expected: {target_size} got {len(data_bytes)} "
                              f"Name: {self.amiro_def.MessageDefs[0][key[2]]['name']}")
                    elif size >= target_size:
                        if size > target_size:
                            print(f"Warning got a message with id {key[2]} that is bigger than expected!"
                                  f"Expected: {target_size} got {len(data_bytes)} "
                                  f"Name: {self.amiro_def.MessageDefs[0][key[2]]['name']}")
                        d = self.amiro_def.MessageDefs[0][key[2]]["class"].from_buffer_copy(data_bytes[:target_size])
                        return TopicData(key[2], d, timestamp/1e6, time())

    def _check_message_in(self, msg: CANMessageIn) -> bool:
        """Check wether a CAN message refers to a known topic and can be handled."""
        if msg.id not in self.all_topics:
            print(f"Warning: There is no msg definition for a msg with id {msg.id}")
        return msg.type == self.amiro_def.MessageType.PubSub and msg.id in self.topics

    def can_identifier(self, type: MessageType, id: Union[Topic, Service], counter: int) -> int:
        """Construct the 32-bit identifier of a CAN message."""

        can_identifier = 0
        can_identifier |= counter
        can_identifier |= id << 6
        can_identifier |= type << 22
        can_identifier |= self.amiro_def.BoardID.Unknown << 26
        return can_identifier

    def can_message_bytes(self, payload: bytes, type: MessageType, id: Union[Topic, Service], counter: int) -> bytes:
        """Construct the bytes of a CAN message (corresponding tawi_message_t layout)."""

        can_message = bytes()
        # esp flags
        can_message += struct.pack("I", 1)
        # identifier of can message
        can_message += struct.pack("I", self.can_identifier(type, id, counter))
        # data length
        can_message += struct.pack("B", len(payload))
        # data
        can_message += payload
        # padding to 20 bytes total
        can_message += bytearray(self.amiro_def.CAN_MESSAGE_SIZE - len(can_message))

        return can_message

    def _send_bytes(self, payload_bytes: bytes, type: MessageType, id: Union[Topic, Service]) -> None:
        """Send bytes to the ESP to be written onto the CAN, if necessary chunk them into multiple messages."""

        chunks = [payload_bytes[i:i + self.amiro_def.CAN_PAYLOAD_SIZE]
                  for i in range(0, len(payload_bytes), self.amiro_def.CAN_PAYLOAD_SIZE)]
        timestamp, frame_count, fire_and_forget = 0, len(chunks), 0
        meta_payload_bytes = struct.pack("Q", timestamp)[:7]

        meta_byte = frame_count << 1  # frame_count
        meta_byte |= fire_and_forget  # fire and forget
        meta_payload_bytes += struct.pack("B", meta_byte)
        self.sock.sendall(self.can_message_bytes(meta_payload_bytes, type, id, 0))
        for i, data in enumerate(chunks):
            counter = len(chunks) - i
            self.sock.sendall(self.can_message_bytes(data, type, id, counter))

    def _send_service(self, service: Service, payload: Any) -> None:
        """Pack the service's values into bytes and send them to the ESP to write them onto the CAN."""
        assert len(bytes(payload)) == sizeof(payload)
        self._send_bytes(bytes(payload), self.amiro_def.MessageType.Service, service)
