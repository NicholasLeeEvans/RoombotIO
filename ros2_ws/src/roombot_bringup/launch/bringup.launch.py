from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    urdf_path = os.path.join(
        get_package_share_directory('roombot_description'),
        'urdf', 'roombot.urdf'
    )
    urdf_content = open(urdf_path).read()

    return LaunchDescription([
        Node(
            package='roombot_bringup',
            executable='circle_odom',
        ),
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': urdf_content}],
        ),
        Node(
            package='foxglove_bridge',
            executable='foxglove_bridge',
        ),
    ])