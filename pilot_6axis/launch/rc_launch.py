from launch import LaunchDescription
from launch_ros.actions import Node
import os

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='pilot_6axis',
            executable='rc',
            name='rc',
            parameters=[
                {"pilot_six_axis_joy_topic" : "/sdrac/joy" }
            ]
        )
    ])
