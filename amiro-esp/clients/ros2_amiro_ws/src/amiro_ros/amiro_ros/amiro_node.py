import rclpy
from rclpy.node import Node
from functools import partial

from amiro_ros.amiro_node_util import *
from amiro_ros_msgs.msg import *
from amiro_remote.amiro import *
from amiro_remote.client import AmiroRemoteClient


class AmiroNode(Node):

    def __init__(self, update_period=0.01):
        super().__init__("amiro")
        self.get_logger().info("Initialize node")

        self.declare_parameter("ip", "129.70.147.80")
        self.declare_parameter("port", 1234)
        
        self.amiro_publisher = {
            topic: self.create_publisher(msg_type, f"amiro/{topic_id2name[topic]}", 10) 
            for topic, msg_type in publisher_spec.items()
        }
        
        self.amiro_services = {
            service: self.create_service(srv_type, f"amiro/{service_id2name[service]}", partial(self.service_callback, service))
            for service, srv_type in service_spec.items()
        }

        self.timer = self.create_timer(update_period, self.topic_callback)

    def connect(self):
        ip = self.get_parameter("ip").get_parameter_value().string_value
        port = self.get_parameter("port").get_parameter_value().integer_value
        self.amiro_remote_client = AmiroRemoteClient(ip, port)
        self.check_connection(was_connected=False)

    def check_connection(self, was_connected=False):
        if not self.amiro_remote_client.is_connected():
            address = (self.amiro_remote_client.can_reader.ip, self.amiro_remote_client.can_reader.port)
            if was_connected:
                self.get_logger().info(f"Lost connection, try reconnecting to {address}...")
            else:
                self.get_logger().info(f"Connecting to {address}...")
            while not self.amiro_remote_client.connect():
                pass
            self.get_logger().info(f"Successfully connected to {address}")

    def topic_callback(self):
        self.check_connection(was_connected=True)

        for topic_data in self.amiro_remote_client.collect_data():
            if topic_data.topic in publisher_spec:
                msg = generate_message(topic_data)
                self.amiro_publisher[topic_data.topic].publish(msg)

    def service_callback(self, service, request, response):
        self.check_connection(was_connected=True)

        self.get_logger().info(f"Got Request {request}")
        handle_service_request(self.amiro_remote_client, service, request)
        return response

    def destroy_node(self) -> bool:
        self.get_logger().info("Shutdown node")
        self.amiro_remote_client.disconnect()
        return super().destroy_node()


def main(args=None):
    rclpy.init(args=args)

    amiro_node = AmiroNode()

    try:
        amiro_node.connect()
        rclpy.spin(amiro_node)
    except KeyboardInterrupt:
        pass
    
    amiro_node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()