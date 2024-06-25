import sys
import argparse
import rospy
import math
import numpy as np
from sensor_msgs.msg import LaserScan
from amiro_remote.client import AmiroRemoteClient, Topic, topic_id2name, TopicData, LightRingColor
from std_msgs.msg import Float32MultiArray, String
from geometry_msgs.msg import Twist

angle_increment = 2*math.pi/8.
laserscan_default_config = {
    'angle_min': - angle_increment/2.,
    'angle_max': + angle_increment/2.,
    'angle_increment': - angle_increment,
    'time_increment': 0,
    'scan_time': 1./61.5, # empirical measured
    'range_min': 0.001,
    'range_max': 0.06,
}


class SensorActor(AmiroRemoteClient):
    def __init__(self, ip, port, topics):
        super().__init__(ip, port)
        self.publisher = {
            # TODO: add prefix by argument
            Topic.RingProximity: rospy.Publisher(topics.get(Topic.RingProximity, topic_id2name[
                Topic.RingProximity]), Float32MultiArray, queue_size=1),
        }
        self.convert_functions = {
            Topic.RingProximity: self.ring_proximity_to_multi_array,
        }

    def _loop(self) -> None:
        """Continuously update topic data received from the AMiRo and potentially send data to services."""

        # while not self.stop_signal:
        while not rospy.is_shutdown():
            topic_data = self.can_reader.get()

            # update data
            self.latest[topic_data.topic] = topic_data
            pub = self.publisher.get(topic_data.topic, None)
            if pub is not None:
                pub.publish(self.convert_functions[topic_data.topic](topic_data))

            # check if queue contains services to send data to
            while not self.signal_queue.empty():
                is_service, items = self.signal_queue.get()
                if is_service:
                    self.can_reader._send_service(*items)
                    rospy.logdebug(f"Write can data: {items}")
                else:
                    pass
        self.can_reader.disconnect()

    @staticmethod
    def ring_proximity_to_laserscan(data: TopicData):
        offset = 2600
        result = Float32MultiArray()
        values = np.asarray(data.values)
        values -= offset
        values = values / (2**16) # TODO: finetune
        values = np.log(values)
        values *= -0.8 # TODO: finetune
        result.data = values.astype(np.float32).tolist()
        return data.values
    

    @staticmethod
    def ring_proximity_to_multi_array(data: TopicData):
        offset = 2600
        result = Float32MultiArray()
        values = np.asarray(data.values)
        values -= offset
        values = values / (2**16) # TODO: finetune
        values = np.log(values)
        values *= -0.8 # TODO: finetune
        result.data = values.astype(np.float32).tolist()
        return result


def main(args):
    rospy.init_node("AMiRo_Sensor_Actor")
    with SensorActor(args.ip, args.port, {
        Topic.RingProximity: args.ring_proximity_topic
    }) as sensor_actor:
        def set_lights(data: String):
            rospy.logdebug(f"{data}")
            color = [color for color in LightRingColor if color.name==data.data]
            if color:
                sensor_actor.set_light(color[0])


        def set_cmd_vel(data: Twist):
            rospy.logdebug(f"Linear: {data.linear.x}, Angular: {data.angular.z}")
            sensor_actor.set_motor(data.linear.x, data.angular.z)
            
        rospy.Subscriber(args.set_lights_topic, String, set_lights)
        rospy.Subscriber(args.cmd_vel_topic, Twist, set_cmd_vel)
        rospy.spin()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Recv CAN Messages over TCP')
    parser.add_argument('--hostname', help='AMiRo Hostname [amiro1, amiro2]', default='AMIRO')
    parser.add_argument('--ip', help='AMiRo IP', default='192.168.1.35')
    parser.add_argument('--port', default=1234, help='tcp port')
    parser.add_argument('--cmd_vel_topic', help='ROS cmd_vel topic, expecting geometry_msgs/Twist', default='amiro/cmd_vel')
    parser.add_argument('--set_lights_topic', help='ROS led topic, expecting std_msgs/String', default='amiro/set_lights')
    parser.add_argument('--ring_proximity_topic', help='ROS ring proximity topic, publishing std_msgs/Float32MultiArray', default='amiro/ring_proximity')

    main(parser.parse_args())
