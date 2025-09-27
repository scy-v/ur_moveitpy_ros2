import time
import rclpy
from moveit.planning import MoveItPy, PlanRequestParameters
from rclpy.logging import get_logger
from tf_transformations import quaternion_from_euler
from geometry_msgs.msg import PoseStamped
from moveit.core.robot_state import RobotState
from moveit.core.kinematic_constraints import construct_joint_constraint
import numpy as np
def plan_and_execute(
        robot,
        ur5e_arm,
        logger,
        vel=None,
        accel=None,
        sleep_time=0.0
):
    """Helper function to plan and execute a motion."""

    params = PlanRequestParameters(robot)    

    if vel is not None:
        params.max_velocity_scaling_factor = vel
    if accel is not None:       
        params.max_acceleration_scaling_factor = accel

    logger.info("Planning trajectory")
    plan_result = ur5e_arm.plan(params)

    #execute the plan
    if plan_result:
        logger.info("Executing plan")
        robot_trajectory = plan_result.trajectory
        robot.execute("ur_manipulator", robot_trajectory)
    else:
        logger.error("Planning failed")
    
    time.sleep(sleep_time)


def main():
    rclpy.init()
    logger = get_logger("ur_demo")
    ur5e = MoveItPy(node_name="ur_demo")
    ur5e_arm = ur5e.get_planning_component("ur_manipulator")

    logger.info("MoveItPy instance created")
    ur5e_arm.set_start_state_to_current_state()
    goal_quat = quaternion_from_euler(-2.98, 0.000, 1.569)
    pose_goal = PoseStamped()
    pose_goal.header.frame_id = "base_link"
    pose_goal.pose.orientation.x = goal_quat[0]
    pose_goal.pose.orientation.y = goal_quat[1]
    pose_goal.pose.orientation.z = goal_quat[2]
    pose_goal.pose.orientation.w = goal_quat[3]
    pose_goal.pose.position.x = -0.649
    pose_goal.pose.position.y = 0.034
    pose_goal.pose.position.z = 0.2
    ur5e_arm.set_goal_state(pose_stamped_msg=pose_goal, pose_link="tcp_link")
    plan_and_execute(ur5e, ur5e_arm, logger, sleep_time=1.0)



    robot_model = ur5e.get_robot_model()
    robot_state = RobotState(robot_model)
    # set plan start state to current state
    ur5e_arm.set_start_state_to_current_state()
    joint_values = [0, -1.57, 0, -1.57, 0, 0] 
    robot_state.set_joint_group_positions("ur_manipulator", joint_values)
    joint_constraint = construct_joint_constraint(
        robot_state=robot_state,
        joint_model_group=ur5e.get_robot_model().get_joint_model_group("ur_manipulator"),
    )

    ur5e_arm.set_goal_state(motion_plan_constraints=[joint_constraint])
    plan_and_execute(ur5e, ur5e_arm, logger, vel=0.05, accel=0.05, sleep_time=1.0)