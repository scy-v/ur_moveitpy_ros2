from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    # Declare arguments
    declared_arguments = [
        DeclareLaunchArgument("GripperID", default_value="1", description="ID of the gripper."),
        DeclareLaunchArgument("GripperModel", default_value="PGE", description="Model of the gripper."),
        DeclareLaunchArgument("Connectport", default_value="/dev/robot/dh_gripper", description="Serial port of the gripper."),
        DeclareLaunchArgument("Baudrate", default_value="115200", description="Baudrate for serial communication."),
        DeclareLaunchArgument("test_run", default_value="false", description="Whether to run the test node."),
    ]

    # Initialize LaunchConfigurations
    gripper_id = LaunchConfiguration("GripperID")
    gripper_model = LaunchConfiguration("GripperModel")
    connect_port = LaunchConfiguration("Connectport")
    baudrate = LaunchConfiguration("Baudrate")
    test_run = LaunchConfiguration("test_run")

    # Nodes
    gripper_node = Node(
        package="dh_gripper_ros2",
        executable="dh_PGE_gripper_ros2",
        name="dh_PGE_gripper_ros2",
        output="screen",
        parameters=[
            {"Gripper_ID": gripper_id},
            {"Gripper_Model": gripper_model},
            {"Connect_port": connect_port},
            {"BaudRate": baudrate},
        ],
        respawn=True,
    )

    test_node = Node(
        package="dh_gripper_ros2",
        executable="dh_PGE_gripper_ros2_test",
        name="dh_PGE_gripper_ros2_test",
        output="screen",
        parameters=[
            {"Gripper_Model": gripper_model},
        ],
        condition=IfCondition(test_run),
    )

    return LaunchDescription(declared_arguments + [gripper_node, test_node])
