from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder(
            robot_name="ur5e", package_name="ur_manipulation"
        )
        .robot_description(file_path="config/ur5e.urdf.xacro")
        .robot_description_semantic(file_path="config/ur5e.srdf.xacro")
        .moveit_cpp(
            file_path=get_package_share_directory("ur_manipulation")
            + "/config/moveit_py.yaml"
        )
        .to_moveit_configs()
    )
    ur_demo_params = moveit_config.to_dict()
    
    return LaunchDescription([
        Node(
            package="ur_manipulation",
            executable="ur_demo",
            name="ur_demo",
            output="screen",
            emulate_tty=True,
            parameters=[ur_demo_params],
        )
    ])
