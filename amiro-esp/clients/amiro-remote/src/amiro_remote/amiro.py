from enum import IntEnum


class AMiRoDef:
    def __init__(self):
        # ---- Size in bytes for various data on the AMiRo ----
        self.CAN_MESSAGE_SIZE = 20
        self.CAN_PAYLOAD_SIZE = 8
        self.REMOTE_SIGNAL_PAYLOAD_SIZE = 14
        # ---- Enums mirroring the AMiRo's definitions in C-enums. ----
        self.BoardID = IntEnum("BoardID", {"WheelDrive": 0,
                                           "PowerManagement": 1,
                                           "LightRing": 2,
                                           "Unknown": 3})  # Board IDs
        self.MessageType = IntEnum("MessageType",
                                   {"PubSub": 0, "Service": 1})  # CAN msg types

    def load_message_defs(self, caps_by_type):
        self.MessageDefs = caps_by_type
        self.Topic = IntEnum("Topic",
                             {k['name']: k['id'] for k in caps_by_type[0]})
        self.Service = IntEnum("Service",
                               {k['name']: k['id'] for k in caps_by_type[1]})
