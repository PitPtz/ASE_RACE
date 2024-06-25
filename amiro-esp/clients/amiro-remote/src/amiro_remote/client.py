# from email.policy import default
from amiro_remote.amiro import Service, Topic, LightRingColor
from amiro_remote.can import CANReaderWriter, TopicData
from typing import Dict, List, Any, Union
from collections import defaultdict, deque
from threading import Thread, Lock
from queue import Queue


class AmiroRemoteClient:
    """Control services and obtain topic data of an AMiRo remotely."""

    def __init__(self, addr, topics=None, max_buffer_size=int(1e5), dummy: bool = False) -> None:
        """Initialize with ESP address and the topics to listen for."""

        # responsible for reading/writing to CAN remotely
        self.can_reader = CANReaderWriter(addr, topics=topics, dummy=dummy)

        # use background thread to receive CAN messages without blocking main thread
        self.thread = None
        self.stop_signal = False
        self.service_queue = Queue()

        # holds the latest topic data
        self.latest: Dict[Topic, TopicData] = defaultdict(None)
        # holds all received data in a buffer, emptied when retreived from foreground thread
        self.max_buffer_size = max_buffer_size
        self.buffer: deque[TopicData] = deque(maxlen=max_buffer_size)
        self.buffer_lock = Lock()

    def __enter__(self) -> "AmiroRemoteClient":
        """Support context management for AMiRo connection."""

        self.connect()
        return self

    def __exit__(self, type, value, traceback) -> None:
        """Support context management for AMiRo connection."""

        if type is KeyboardInterrupt:
            self.set_motor(0, 0)
            self.set_light(LightRingColor.Off)

        self.disconnect()

    def connect(self) -> bool:
        """Connect with the AMiRo and start receiving topic data in background thread."""

        self.latest = defaultdict(None)
        self.buffer = deque(maxlen=self.max_buffer_size)
        self.service_queue = Queue()
        self.can_reader.connect()
        self.stop_signal = False
        self.thread = Thread(target=self._loop)
        self.thread.setDaemon(True)
        self.thread.start()

    def disconnect(self) -> None:
        """Disconnect from the AMiRo and stop background thread."""

        self.stop_signal = True
        if self.thread is not None:
            self.thread.join()
        self.thread = None

    def is_connected(self) -> bool:
        """Returns wether the client is connected to an AMiRo."""

        return self.thread is not None and self.thread.is_alive()

    def _loop(self) -> None:
        """Continuously update topic data received from the AMiRo and potentially send data to services."""

        try:
            while not self.stop_signal:
                # receive and update topic data
                # TODO: handle timeout from can_reader
                topic_data = self.can_reader.get()

                # update data
                self.latest[topic_data.topic] = topic_data
                with self.buffer_lock:
                    self.buffer.append(topic_data)

                # check if queue contains services to send data to
                while not self.service_queue.empty():
                    service, values = self.service_queue.get()
                    self.can_reader._send_service(service, values)
        except:  # noqa E722
            pass
        self.can_reader.disconnect()

    def buffer_size(self, topic: Topic = None) -> int:
        """Return the amount of buffered topic data, optionally provide a specific topic."""

        if topic is None:
            return len(self.buffer)
        else:
            return sum(1 for td in self.buffer if td.topic == topic)

    def get_latest(self, topic: Topic) -> Union[TopicData, None]:
        return self.latest[topic]

    def collect_data(self, topic: Topic = None) -> List[TopicData]:
        """Return buffered topic data and remove it, optionally provide a specific topic."""

        if topic is None:
            # return collected and clear buffer
            collected = self.buffer
            self.buffer = deque(maxlen=self.max_buffer_size)
            return list(collected)
        else:
            # empty buffer temporarily and work with current data
            all_buffered = self.buffer
            self.buffer = deque()

            # split buffer into collected data for this topic and the rest of the remaining data
            remaining, collected = deque(maxlen=self.max_buffer_size), deque()
            for topic_data in all_buffered:
                target_list = (collected if topic_data.topic == topic else remaining)
                target_list.append(topic_data)

            # add remaining data back to the buffer
            with self.buffer_lock:
                self.buffer = remaining + self.buffer
            return list(collected)

    def _put_service(self, service: Service, values: List[Any]) -> None:
        """Enqueue service data to send from the background thread."""

        self.service_queue.put((service, values))

    def set_motor(self, translation: float, rotation: float) -> None:
        """Set the AMiRo's motor control."""

        self._put_service(Service.DMCTarget, [translation, 0, 0, 0, 0, rotation])

    def set_light(self, color: LightRingColor) -> None:
        """Set the color of the AMiRo's light ring."""

        self._put_service(Service.Light, [color for _ in range(24)])

    def set_light_list(self, colors: List[LightRingColor]) -> None:
        """Set the color of all light ring LEDs individually"""

        self._put_service(Service.Light, colors)

    def set_odometry(self, position: List[int], orientation: List[int]) -> None:
        """Reset the odometry values to the given values"""

        assert len(position) == 3 and len(orientation) == 4
        self._put_service(Service.ResetOdometry, position + orientation)
