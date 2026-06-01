import json
import threading
import time
from math import radians

import requests
import rclpy
from geometry_msgs.msg import TransformStamped
from rclpy.node import Node
from tf2_ros import TransformBroadcaster
from tf_transformations import quaternion_from_euler

from geometry_msgs.msg import PoseStamped


class RoombotBridge(Node):
    def __init__(self):
        super().__init__('roombot_bridge')
        self.declare_parameter('roombot_ip', '192.168.10.117')
        ip = self.get_parameter('roombot_ip').get_parameter_value().string_value
        self._url = f'http://{ip}/events'
        self._broadcaster = TransformBroadcaster(self)
        threading.Thread(target=self._sse_loop, daemon=True).start()

        self._pose_publisher = self.create_publisher(PoseStamped, "/roombot/pose", 10)

    def _sse_loop(self):
        while rclpy.ok():
            try:
                #self.get_logger().info(f'Connecting to {self._url}')
                resp = requests.get(self._url, stream=True, timeout=(10, None),
                                    headers={'Accept': 'text/event-stream'})
                event_type = None
                for raw_line in resp.iter_lines(decode_unicode=True):
                    line = raw_line.strip()
                    if not rclpy.ok():
                        return
                    if line.startswith('event:'):
                        event_type = line[6:].strip()
                    elif line.startswith('data:') and event_type == 'telemetry':
                        payload = json.loads(line[5:].strip())
                        self._publish_tf(payload)
                        event_type = None
                    elif line == '':
                        event_type = None
            except Exception as exc:
                self.get_logger().warn(f'SSE connection lost: {exc} — retrying in 2s')
                time.sleep(2.0)

    def _publish_tf(self, data):
        t = TransformStamped()
        t.header.stamp = self.get_clock().now().to_msg()
        t.header.frame_id = 'odom'
        t.child_frame_id = 'base_footprint'
        t.transform.translation.x = data['x'] / 1000.0   # mm → m
        t.transform.translation.y = data['y'] / 1000.0
        t.transform.translation.z = 0.0
        q = quaternion_from_euler(0.0, 0.0, radians(data['angle']))
        t.transform.rotation.x = q[0]
        t.transform.rotation.y = q[1]
        t.transform.rotation.z = q[2]
        t.transform.rotation.w = q[3]
        self._broadcaster.sendTransform(t)

        pose = PoseStamped()
        pose.header.frame_id = 'odom'
        pose.header.stamp = self.get_clock().now().to_msg()
        pose.pose.position.x = t.transform.translation.x
        pose.pose.position.y = t.transform.translation.y
        pose.pose.position.z = t.transform.translation.z
        pose.pose.orientation = t.transform.rotation
        self._pose_publisher.publish(pose)


def main():
    rclpy.init()
    node = RoombotBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    rclpy.shutdown()
