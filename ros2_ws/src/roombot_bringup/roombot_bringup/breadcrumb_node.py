import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, DurabilityPolicy, HistoryPolicy

from geometry_msgs.msg import PoseStamped
from nav_msgs.msg import Path
from std_srvs.srv import Empty

from math import dist

from collections import deque

class BreadcrumbNode(Node):
    def __init__(self):
        super().__init__('breadcrumb_node')
        self._pose_subscriber = self.create_subscription(PoseStamped, "/roombot/pose",self._pose_callback, 10)
        self._reset_service = self.create_service(Empty,"/roombot/reset_path",self._reset_callback)
        path_qos = QoSProfile(
                depth=1,
                history=HistoryPolicy.KEEP_LAST,
            durability=DurabilityPolicy.TRANSIENT_LOCAL,
        )
        self._path_publisher = self.create_publisher(Path, '/roombot/path', path_qos)
        self._crumbs = deque(maxlen=1000)

    def _pose_callback(self, pose: PoseStamped):
        self.get_logger().info(f"got pose:{pose.pose.orientation} ")
        # add pose to path
        if len(self._crumbs) == 0 or self._distance(pose, self._crumbs[-1]) > 0.050:
            self._crumbs.append(pose)
        # send to foxglove
        msg = Path()
        msg.header.frame_id = 'odom'
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.poses = list(self._crumbs) + [pose]
        self._path_publisher.publish(msg)

    def _reset_callback(self, request, response):
        self._crumbs.clear()
        empty = Path()
        empty.header.frame_id = 'odom'
        empty.header.stamp = self.get_clock().now().to_msg()
        self._path_publisher.publish(empty)
        return response
    
    def _distance(self, pose_from: PoseStamped, pose_to: PoseStamped): # return distance in m
        pos_from = pose_from.pose.position
        pos_to = pose_to.pose.position
        return dist((pos_from.x, pos_from.y), (pos_to.x, pos_to.y))


def main(args=None):
    rclpy.init(args=args)

    breadcrumb_node = BreadcrumbNode()

    rclpy.spin(breadcrumb_node)

    breadcrumb_node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()