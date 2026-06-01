import rclpy
from rclpy.node import Node
from tf_transformations import quaternion_from_euler
from tf2_ros import TransformBroadcaster

from geometry_msgs.msg import TransformStamped
import math

class TransformPublisher(Node):

    def __init__(self):
        super().__init__('transform_broadcaster')
        self.broadcaster = TransformBroadcaster(self)
        timer_period = 0.5
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.angle = 0.0
        self.radius = 1.0

    def timer_callback(self):
        t = TransformStamped()
        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = 'odom'
        t.child_frame_id = 'base_footprint'
        t.transform.translation.x = self.radius * math.cos(self.angle)
        t.transform.translation.y = self.radius * math.sin(self.angle)
        t.transform.translation.z = 0.0
        yaw = self.angle + math.pi/2
        quat = quaternion_from_euler(
            0,
            0,
            yaw
        )

        t.transform.rotation.x = quat[0]
        t.transform.rotation.y = quat[1]
        t.transform.rotation.z = quat[2]
        t.transform.rotation.w = quat[3]

        self.angle += 0.05

        self.broadcaster.sendTransform(t)
def main():
    rclpy.init()
    node = TransformPublisher()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    rclpy.shutdown()