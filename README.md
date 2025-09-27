# Moveit2 for ROS2 Humble (moveit_py interface)
```bash
# in home directory
mkdir -p moveit2_ws/src && cd moveit2_ws/src
git clone https://github.com/moveit/moveit2.git 
cd moveit2
git fetch origin pull/3487/head:pr-3487 #from pr-3487
git checkout pr-3487
cd <movei2_ws>
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
echo "source ~/moveit2_ws/install/setup.sh" >> ~/.bashrc
source ~/.bashrc
```
- Now, you can use the moveit_py interface (Python API) to control the robot from move_group.

# Example for the moveit_py
```bash
mkdir -p ur_ws/src && cd ur_ws/src
git clone https://github.com/scy-v/ur_ros2_moveitpy.git
cd ..
colcon build --symlink-install
``` 
# UR Robot Driver and Moveit2 Configuration (Optional)
```bash
cd ur_ws/src
git clone -b humble https://github.com/UniversalRobots/Universal_Robots_ROS2_Driver.git
sudo apt update (optional)
sudo apt install python3-vcstool (optional)
vcs import < Universal_Robots_ROS2_Driver/Universal_Robots_ROS2_Driver.humble.repos
cd ..
colcon build --symlink-install
# Now, you can drive the real ur robot by Moveit in Rviz:
ros2 launch ur_robot_driver <ur_type>.launch.py robot_ip:= <robot_ip> launch_rviz:=false
ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:= <ur_type> launch_rviz:=true
```

# Realsense Ros2 Configuration (Optional) 
- You should download [realsense-SDK](https://github.com/IntelRealSense/librealsense) first
```bash
cd <src_folder>
git clone -b ros2-master https://github.com/IntelRealSense/realsense-ros.git
colcon build --symlink-install
```

