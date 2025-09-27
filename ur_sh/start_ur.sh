#!/bin/bash

# Launch the UR robot driver
terminator -e "bash -i -c 'ros2 launch ur_robot_driver ur5e.launch.py robot_ip:=192.168.131.11; exec bash'" &

sleep 3

# Launch MoveIt
terminator -e "bash -i -c 'ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e launch_rviz:=true; exec bash'" &

# Launch the gripper
terminator -e "bash -i -c 'ros2 launch dh_gripper_ros2 dh_gripper.launch.py; exec bash'" &

# Launch RealSense camera and publish images/depth and aligned point cloud
terminator -e "bash -i -c 'ros2 launch rs_ros2 realsense_launch.py; exec bash'" &

# Launch camera coordinate transformation
terminator -e "bash -i -c 'ros2 launch rs_ros2 eye2hand.launch.py; exec bash'" &
