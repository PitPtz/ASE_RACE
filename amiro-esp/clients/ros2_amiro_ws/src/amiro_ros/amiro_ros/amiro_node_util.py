from typing import Dict, Type

from amiro_remote.amiro import *
from amiro_ros_msgs.msg import *
from amiro_ros_msgs.srv import *


publisher_spec: Dict[Topic, Type] = {
    Topic.FloorProximity:   FloorProximity,
    Topic.FloorAmbient:     FloorAmbient,
    Topic.RingAmbient:      RingAmbient,
    Topic.RingProximity:    RingProximity,
    #Topic.Touch:            Touch,
    Topic.Accelerometer:    Accelerometer,
    Topic.Gyroscope:        Gyroscope,
    Topic.Compass:          Compass,
    Topic.Magnetometer:     Magnetometer,
    Topic.Odometry:         Odometry,
}


service_spec: Dict[Service, Type] = {
    Service.Light:          Light,
    Service.DMCTarget:      DMCTarget,
    Service.ResetOdometry:  ResetOdometry,
}


def generate_message(topic_data):

    msg = publisher_spec[topic_data.topic]()
    
    if topic_data.topic in [Topic.FloorProximity, Topic.FloorAmbient, 
                            Topic.RingProximity, Topic.RingAmbient]:
        msg.values = topic_data.values

    elif topic_data.topic in [Topic.Accelerometer, Topic.Gyroscope, 
                            Topic.Compass, Topic.Magnetometer]:
        msg.values.x = topic_data.values[0]
        msg.values.y = topic_data.values[1]
        msg.values.z = topic_data.values[2]
    
    elif topic_data.topic == Topic.Odometry:
        msg.pose.position.x = topic_data.values[0]
        msg.pose.position.y = topic_data.values[1]
        msg.pose.position.z = topic_data.values[2]
        msg.pose.orientation.x = topic_data.values[3]
        msg.pose.orientation.y = topic_data.values[4]
        msg.pose.orientation.z = topic_data.values[5]
        msg.pose.orientation.w = topic_data.values[6]

    return msg


def handle_service_request(amiro_remote_client, service, request):

    if service == Service.Light:
        if request.color_id in [item.value for item in LightRingColor]:
            lrc = LightRingColor(request.color_id)
            amiro_remote_client.set_light(lrc)

    elif service == Service.DMCTarget:
        amiro_remote_client.set_motor(request.velocity.linear.x, request.velocity.angular.z)

    elif service == Service.ResetOdometry:
        pos = request.reset_pose.position
        ori = request.reset_pose.orientation
        amiro_remote_client.set_odometry(
                [pos.x, pos.y, pos.z], 
                [ori.x, ori.y, ori.z, ori.w]
        )
