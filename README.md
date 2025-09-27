# MoveIt2 with ROS2 Humble (Moveit_py Python API)

🔹 **This repository demonstrates how to use MoveIt2 with ROS2 Humble via the `moveit_py` interface to control robot.**  
🔹 **If you have all the hardware and want to use the full functionality, you should complete the optional steps.**

## Table of Contents
- [1. Install MoveIt2 with Moveit\_py ⚡](#1-install-moveit2-with-moveit_py-)
- [2. Example: MoveIt\_py for UR Robots ⚡](#2-example-moveit_py-for-ur-robots-)
- [3. UR Robot Driver \& MoveIt2 Launch (Optional) ⚡](#3-ur-robot-driver--moveit2-launch-optional-)
- [4. Intel Realsense ROS2 Configuration (Optional) ⚡](#4-intel-realsense-ros2-configuration-optional-)
- [5. DH Gripper ROS2 Configuration (Optional) ⚡](#5-dh-gripper-ros2-configuration-optional-)
- [6. One-Command Launch (Optional) ⚡](#6-one-command-launch-optional-)
---

# 1. Install MoveIt2 with Moveit_py ⚡ 

🔹 **Follow these steps to set up MoveIt2 in your ROS2 Humble:**

```bash
# Create workspace and clone MoveIt2
mkdir -p ~/moveit2_ws/src && cd ~/moveit2_ws/src
git clone https://github.com/moveit/moveit2.git 
cd moveit2

# Checkout pull request for moveit_py in ROS2 Humble
git fetch origin pull/3487/head:pr-3487
git checkout pr-3487
```

🛠️ **Build the workspace:**

```bash
cd ~/moveit2_ws
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release

# Add MoveIt2 to bashrc
echo "source ~/moveit2_ws/install/setup.sh" >> ~/.bashrc
source ~/.bashrc
```

💡 **Tip:** Now you can use the `moveit_py` Python API to control your robots via `move_group`.

---

# 2. Example: MoveIt_py for UR Robots ⚡ 

Follow these steps to try an example with a UR robot:

```bash
# Create workspace and clone example
mkdir -p ~/ur_ws/src && cd ~/ur_ws/src
git clone https://github.com/scy-v/ur_moveitpy_ros2.git

# Build workspace
cd ~/ur_ws
colcon build --symlink-install

# Add ur_ws to bashrc
echo "source ~/ur_ws/install/setup.sh" >> ~/.bashrc
```

✅ **Launch the example** (if you have configured the UR Robot):

```bash
ros2 launch ur_manipulation ur_demo.launch.py
```

💡 **Tip:** You can refer to the Python scripts `ur_demo.py` or `movepy_template.py` inside `ur_manipulation` for MoveIt_py usage.

---

# 3. UR Robot Driver & MoveIt2 Launch (Optional) ⚡ 

If you want to control the real UR robot, follow these steps:

```bash
cd ~/ur_ws/src
git clone -b humble https://github.com/UniversalRobots/Universal_Robots_ROS2_Driver.git

# Optional dependencies
sudo apt update
sudo apt install python3-vcstool

# Import repositories
vcs import < Universal_Robots_ROS2_Driver/Universal_Robots_ROS2_Driver.humble.repos

# Build workspace
cd ~/ur_ws
colcon build --symlink-install
```

✅ **Launch the real UR robot in Rviz using MoveIt2:**

```bash
ros2 launch ur_robot_driver <ur_type>.launch.py robot_ip:=<robot_ip> launch_rviz:=false
ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=<ur_type> launch_rviz:=true
```

---

# 4. Intel Realsense ROS2 Configuration (Optional) ⚡ 

If you want to use the Realsense camera (e.g., D435i) with ROS2:

1. Install [Realsense SDK](https://github.com/IntelRealSense/librealsense) first.
2. Clone the ROS2 package:

```bash
cd ~/src
git clone -b ros2-master https://github.com/IntelRealSense/realsense-ros.git
colcon build --symlink-install
```

✅ **Launch the Realsense camera from rs_ros2 package:**

```bash
ros2 launch rs_ros2 realsense_launch.py
```

✅ **Eye-to-hand calibration** (compute pose between camera and robot):

```bash
ros2 launch rs_ros2 eye2hand.launch.py
```

---

# 5. DH Gripper ROS2 Configuration (Optional) ⚡

To use the DH Gripper with ROS2:

```bash
# Launch DH Gripper from dh_gripper_ros2 package
ros2 launch dh_gripper_ros2 dh_gripper.launch.py
```

---

# 6. One-Command Launch (Optional) ⚡

Once all optional setups are complete, you can start everything with a single command:

```bash
cd <ur_sh_folder>
bash start_ur.sh
```

💡 **Tip:** Make sure all required hardware and configurations are ready before running this command.
