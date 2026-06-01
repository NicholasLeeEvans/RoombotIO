from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

web_client_path = os.path.join(get_package_share_directory('roombot_bringup'), 'web-client')


def generate_launch_description():
    urdf_path = os.path.join(
        get_package_share_directory('roombot_description'),
        'urdf', 'roombot.urdf'
    )
    urdf_content = open(urdf_path).read()

    return LaunchDescription([
        DeclareLaunchArgument('roombot_ip', default_value='192.168.10.117'),
        Node(
            package='roombot_bringup',
            executable='roombot_bridge',
            parameters=[{'roombot_ip': LaunchConfiguration('roombot_ip')}]
        ),
        Node(
            package='roombot_bringup',
            executable='breadcrumb_node',
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
        ExecuteProcess(
            cmd=['python3', '-m', 'http.server', '8000'],
            cwd=web_client_path,
            output='screen'
        ),
        # joy_node reads the gamepad hardware and publishes sensor_msgs/Joy on /joy
        Node(
            package='joy',
            executable='joy_node',
        ),
        Node(
            package='roombot_bringup',
            executable='ps4_teleop',
            parameters=[{'roombot_ip': LaunchConfiguration('roombot_ip')}]
        ),
    ])