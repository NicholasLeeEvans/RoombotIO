import requests
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy

# PS4 DualShock axis indices (standard joy package mapping)
AXIS_LEFT_X = 0   # left stick horizontal: left=+1, right=-1
AXIS_LEFT_Y = 1   # left stick vertical:   up=+1,   down=-1
AXIS_R2 = 5       # R2 trigger:            released=+1, fully pressed=-1

DEADZONE = 0.15          # ignore stick values below this magnitude
MAX_DISTANCE_MM = 50.0   # mm per command pulse (scaled by stick magnitude)
MAX_ANGLE_DEG = 20.0     # degrees per command pulse (scaled by stick magnitude)
COMMAND_INTERVAL_S = 0.5 # seconds between commands while stick is held


class PS4TeleopNode(Node):
    def __init__(self):
        super().__init__('ps4_teleop')
        self.declare_parameter('roombot_ip', '192.168.10.117')
        self._ip = self.get_parameter('roombot_ip').get_parameter_value().string_value

        self._joy_sub = self.create_subscription(Joy, '/joy', self._joy_callback, 10)
        self._timer = self.create_timer(COMMAND_INTERVAL_S, self._send_command)

        self._linear = 0.0   # -1.0 (back) to +1.0 (forward)
        self._angular = 0.0  # -1.0 (right) to +1.0 (left)
        self._enabled = False

    def _joy_callback(self, msg: Joy):
        # R2 trigger: requires meaningful press (below +1 means partially depressed)
        r2 = msg.axes[AXIS_R2] if len(msg.axes) > AXIS_R2 else 1.0
        self._enabled = r2 < 0.0

        ly = msg.axes[AXIS_LEFT_Y] if len(msg.axes) > AXIS_LEFT_Y else 0.0
        lx = msg.axes[AXIS_LEFT_X] if len(msg.axes) > AXIS_LEFT_X else 0.0

        self._linear = ly if abs(ly) > DEADZONE else 0.0
        self._angular = lx if abs(lx) > DEADZONE else 0.0

    def _send_command(self):
        if not self._enabled:
            return
        if abs(self._linear) < 0.01 and abs(self._angular) < 0.01:
            return

        try:
            if abs(self._angular) >= abs(self._linear):
                angle = self._angular * MAX_ANGLE_DEG
                url = f'http://{self._ip}/action?type=turn&angle={angle:.1f}&radius=0'
            else:
                distance = self._linear * MAX_DISTANCE_MM
                url = f'http://{self._ip}/action?type=straight&distance={distance:.1f}'

            requests.get(url, timeout=0.4)
            self.get_logger().debug(f'sent: {url}')
        except Exception as exc:
            self.get_logger().warn(f'command failed: {exc}')


def main(args=None):
    rclpy.init(args=args)
    node = PS4TeleopNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    rclpy.shutdown()
