/*
 * dh_PGE_gripper_ros2_test.cpp
 *
 *  Created on: May 20, 2024
 *      Author: shangweih
 */

#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <chrono>

#include <dh_gripper_ros2/msg/gripper_ctrl.hpp>
#include <dh_gripper_ros2/msg/gripper_rot_ctrl.hpp>
#include <dh_gripper_ros2/msg/gripper_rot_state.hpp>
#include <dh_gripper_ros2/msg/gripper_state.hpp>

rclcpp::Subscription<dh_gripper_ros2::msg::GripperState>::SharedPtr _sub_grip_state;
rclcpp::Subscription<dh_gripper_ros2::msg::GripperRotState>::SharedPtr _sub_rot_state ;

rclcpp::Publisher<dh_gripper_ros2::msg::GripperCtrl>::SharedPtr _gripper_ctrl_pub;
rclcpp::Publisher<dh_gripper_ros2::msg::GripperRotCtrl>::SharedPtr _rot_ctrl_pub;

dh_gripper_ros2::msg::GripperState _g_state;
dh_gripper_ros2::msg::GripperRotState _r_state;

rclcpp::Parameter _gripper_model;

using namespace std::chrono_literals; // Enables the specification of a time duration

void _update_gripper_state(const dh_gripper_ros2::msg::GripperState::ConstPtr& msg){
}

void _update_gripper_rot_state(const dh_gripper_ros2::msg::GripperRotState::ConstPtr& msg){
}

void AG95_Test(rclcpp::Node::SharedPtr node){
	dh_gripper_ros2::msg::GripperCtrl msg_g_ctrl;
    msg_g_ctrl.initialize = true;
    msg_g_ctrl.position = 100;
    msg_g_ctrl.force = 100;
    msg_g_ctrl.speed = 100;
    _gripper_ctrl_pub->publish(msg_g_ctrl);

    while(!_g_state.is_initialized)
    {
    	rclcpp::spin_some(node);
    }

    int test_state = 0;
   while (rclcpp::ok())
    {
            switch(test_state)
            {
                    case 0:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 100;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 1;
                            break;
                    case 1:
                            if(_g_state.grip_state == 0);
                                    test_state = 2;
                            break;
                    case 2:
                            if(_g_state.grip_state != 0);
                                    test_state = 3;
                            break;
                    case 3:
//                            ros::Duration(0.2).sleep();
                            rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 4;
                    case 4:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 0;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 5;
                            break;
                    case 5:
                            if(_g_state.grip_state == 0);
                                    test_state = 6;
                            break;
                    case 6:
                            if(_g_state.grip_state != 0);
                                    test_state = 7;
                            break;
                    case 7:
//                            ros::Duration(0.2).sleep();
                            rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 0;
            }
            rclcpp::spin_some(node);
    }
}

void DH3_Test(rclcpp::Node::SharedPtr node){
	dh_gripper_ros2::msg::GripperCtrl msg_g_ctrl;
    msg_g_ctrl.initialize = true;
    msg_g_ctrl.position = 1000;
    msg_g_ctrl.force = 100;
    msg_g_ctrl.speed = 100;
    _gripper_ctrl_pub->publish(msg_g_ctrl);

    while(!_g_state.is_initialized)
    {
    	rclcpp::spin_some(node);
    }

    int test_state = 0;
   while (rclcpp::ok())
    {
            switch(test_state)
            {
                    case 0:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 1000;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 1;
                            break;
                    case 1:
                            if(_g_state.grip_state == 0);
                                    test_state = 2;
                            break;
                    case 2:
                            if(_g_state.grip_state != 0);
                                    test_state = 3;
                            break;
                    case 3:
//                            ros::Duration(0.2).sleep();
                    		rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 4;
                    case 4:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 0;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 5;
                            break;
                    case 5:
                            if(_g_state.grip_state == 0);
                                    test_state = 6;
                            break;
                    case 6:
                            if(_g_state.grip_state != 0);
                                    test_state = 7;
                            break;
                    case 7:
//                            ros::Duration(0.2).sleep();
                    		rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 0;
            }
            rclcpp::spin_some(node);
    }
}

void ModbusGripper_test(rclcpp::Node::SharedPtr node){
	dh_gripper_ros2::msg::GripperCtrl msg_g_ctrl;
    msg_g_ctrl.initialize = true;
    msg_g_ctrl.position = 1000;
    msg_g_ctrl.force = 100;
    msg_g_ctrl.speed = 100;
    _gripper_ctrl_pub->publish(msg_g_ctrl);

    while(!_g_state.is_initialized)
    {
//            ros::spinOnce();
    	rclcpp::spin_some(node);
    }

    int test_state = 0;
   while (rclcpp::ok())
    {
            switch(test_state)
            {
                    case 0:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 1000;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 1;
                            break;
                    case 1:
                            if(_g_state.grip_state == 0);
                                    test_state = 2;
                            break;
                    case 2:
                            if(_g_state.grip_state != 0);
                                    test_state = 3;
                            break;
                    case 3:
//                            ros::Duration(0.2).sleep();
                    		rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 4;
                    case 4:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 0;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 5;
                            break;
                    case 5:
                            if(_g_state.grip_state == 0);
                                    test_state = 6;
                            break;
                    case 6:
                            if(_g_state.grip_state != 0);
                                    test_state = 7;
                            break;
                    case 7:
//                            ros::Duration(0.2).sleep();
                    		rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 0;
            }
//            ros::spinOnce();
            rclcpp::spin_some(node);
    }
}

void RGI_test(rclcpp::Node::SharedPtr node){
	dh_gripper_ros2::msg::GripperCtrl msg_g_ctrl;
    msg_g_ctrl.initialize = true;
    msg_g_ctrl.position = 1000;
    msg_g_ctrl.force = 100;
    msg_g_ctrl.speed = 100;
    _gripper_ctrl_pub->publish(msg_g_ctrl);

    while(!_g_state.is_initialized)
    {
//            ros::spinOnce();
    	rclcpp::spin_some(node);
    }

    int test_state = 0;
   while (rclcpp::ok())
    {
            switch(test_state)
            {
                    case 0:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 1000;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 1;
                            break;
                    case 1:
                            if(_g_state.grip_state == 0);
                                    test_state = 2;
                            break;
                    case 2:
                            if(_g_state.grip_state != 0);
                                    test_state = 3;
                            break;
                    case 3:
//                            ros::Duration(0.2).sleep();
                            rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 4;
                    case 4:
                            msg_g_ctrl.initialize = false;
                            msg_g_ctrl.position = 0;
                            msg_g_ctrl.force = 100;
                            msg_g_ctrl.speed = 100;
                            _gripper_ctrl_pub->publish(msg_g_ctrl);
                            test_state = 5;
                            break;
                    case 5:
                            if(_g_state.grip_state == 0);
                                    test_state = 6;
                            break;
                    case 6:
                            if(_g_state.grip_state != 0);
                                    test_state = 7;
                            break;
                    case 7:
//                            ros::Duration(0.2).sleep();
                    		rclcpp::sleep_for(std::chrono::milliseconds(200));
                            test_state = 0;
            }
//            ros::spinOnce();
            rclcpp::spin_some(node);
    }
}


int main(int argc, char** argv)
{
	rclcpp::init(argc, argv);
	auto n = rclcpp::Node::make_shared("dh_gripper_tester");
//    std::string _gripper_model;
//    n.param<std::string>("Gripper_Model", _gripper_model,"AG95_MB");
    n->get_parameter_or("Gripper_Model",_gripper_model, rclcpp::Parameter("Gripper_Model","AG95_MB"));
//
////    ROS_INFO("Gripper_model : %s", _gripper_model.c_str());
    RCLCPP_INFO(n->get_logger(),"Gripper_model : %s", _gripper_model.value_to_string().c_str());
    _sub_grip_state = n->create_subscription<dh_gripper_ros2::msg::GripperState>("/gripper/states", 50, _update_gripper_state);
    _gripper_ctrl_pub = n->create_publisher<dh_gripper_ros2::msg::GripperCtrl>("/gripper/ctrl", 50);

    if(_gripper_model.as_string().find("AG95_CAN")!=_gripper_model.value_to_string().npos)
    {
        AG95_Test(n);
    }
    else if(_gripper_model.as_string().find("DH3")!=_gripper_model.value_to_string().npos)
    {
        _sub_rot_state = n->create_subscription<dh_gripper_ros2::msg::GripperRotState>("/gripper/rot_states", 50, _update_gripper_rot_state);
        _rot_ctrl_pub = n->create_publisher<dh_gripper_ros2::msg::GripperRotCtrl>("/gripper/rot_ctrl", 50);
        DH3_Test(n);
    }
    else if(_gripper_model.value_to_string().find("AG95_MB")!=_gripper_model.value_to_string().npos
                ||_gripper_model.value_to_string().find("PGE")!=_gripper_model.value_to_string().npos
                ||_gripper_model.value_to_string().find("PGC")!=_gripper_model.value_to_string().npos
                ||_gripper_model.value_to_string().find("CGC")!=_gripper_model.value_to_string().npos)
    {
        ModbusGripper_test(n);
    }
    else if(_gripper_model.value_to_string().find("RGI")!=_gripper_model.value_to_string().npos)
    {
        _sub_rot_state = n->create_subscription<dh_gripper_ros2::msg::GripperRotState>("/gripper/rot_states", 50, _update_gripper_rot_state);
        _rot_ctrl_pub = n->create_publisher<dh_gripper_ros2::msg::GripperRotCtrl>("/gripper/rot_ctrl", 50);
        RGI_test(n);
    }
    else
    {
            return -1;
    }
    return 0;
}


