from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='rs_ros2',  
            executable="dyn_tf.py",
            name='dynamic_tf_broadcaster',
            output='screen',
            parameters=[{
                'x': -0.890119,
                'y': 0.0414153,
                'z': 0.57547,
                'qx': -0.634522,
                'qy': 0.692684,
                'qz': -0.258589,
                'qw': 0.225172,
                'frame_id': 'base_link',
                'child_frame_id': 'camera_depth_optical_frame',
                'rate': 5.0
            }]
        )
    ])
