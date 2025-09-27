/*
 * dh_PGE_gripper_ros2.cpp
 *
 *  Created on: May 14, 2024
 *      Author: shangweih
 */

#include <iostream>
#include <unistd.h>
#include <dh_gripper_factory.h>

#include <rclcpp/rclcpp.hpp>
#include <dh_gripper_ros2/msg/gripper_ctrl.hpp>
#include <dh_gripper_ros2/msg/gripper_rot_ctrl.hpp>
#include <dh_gripper_ros2/msg/gripper_rot_state.hpp>
#include <dh_gripper_ros2/msg/gripper_state.hpp>
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

//std::string _gripper_ID;
rclcpp::Parameter _gripper_ID;
rclcpp::Parameter _gripper_model;
rclcpp::Parameter _gripper_connect_port;
rclcpp::Parameter _gripper_Baudrate;

DH_Gripper *_gripper;

using namespace std::chrono_literals; // Enables the specification of a time duration

void update_gripper_control(const dh_gripper_ros2::msg::GripperCtrl::ConstPtr& msg)
{

    if(msg->initialize)
    {
      _gripper->Initialization();
    }
    else
    {
//    	RCLCPP_INFO("speed:[%f],force: [%f], position: [%f]", msg->speed,msg->force, msg->position);
        _gripper->SetTargetSpeed((int)msg->speed);
        _gripper->SetTargetForce((int)msg->force);
        _gripper->SetTargetPosition((int)msg->position);
    }
}

void update_rotation_control(const dh_gripper_ros2::msg::GripperRotCtrl::ConstPtr& msg)
{
//    RCLCPP_INFO("r_speed:[%f],r_force: [%f], r_angle: [%f]", msg->speed,msg->force, msg->angle);
    if(_gripper_model.as_string().find("RGI")!= _gripper_model.as_string().npos)
    {
        dynamic_cast<DH_RGI *>(_gripper)->SetTargetRotationTorque((int)msg->force);
        dynamic_cast<DH_RGI *>(_gripper)->SetTargetRotationSpeed((int)msg->speed);
        dynamic_cast<DH_RGI *>(_gripper)->SetTargetRotation((int)msg->angle);

    }
    else if(_gripper_model.as_string().find("DH3_CAN")!= _gripper_model.as_string().npos)
    {
        dynamic_cast<DH_DH3_CAN *>(_gripper)->SetTargetRotation((int)msg->angle);
    }

}

void update_gripper_state(dh_gripper_ros2::msg::GripperState& msg)
{
//    static long seq = 0;
//    msg.header.stamp = rclcpp::Clock();//::now();
//    msg.header.seq =seq;
    int tmp_state[5] = {0};
    _gripper->GetRunStates(tmp_state);
    if(tmp_state[0] == 1)
        msg.is_initialized = true;
    else
        msg.is_initialized = false;

    msg.grip_state      = tmp_state[1];
    msg.position        = tmp_state[2];
    msg.target_position = tmp_state[3];
    msg.target_force    = tmp_state[4];
//    seq++;
}
void update_gripper_joint_state(sensor_msgs::msg::JointState& msg)
{
//    static long seq = 0;
    msg.header.frame_id = "";
//    msg.header.stamp = ros::Time::now();
//    msg.header.seq = seq;

    msg.name.resize(1);
    msg.position.resize(1);


    int tmp_pos = 0;

    _gripper->GetCurrentPosition(tmp_pos);

    msg.position[0] = (1000-tmp_pos)/1000.0 * 0.637;
    msg.name[0] = "gripper_finger1_joint";

//    seq++;
}

void update_rotation_state(dh_gripper_ros2::msg::GripperRotState& msg)
{
//    static long seq = 0;
//    msg.header.stamp = ros::Time::now();
//    msg.header.seq =seq;
    int tmp_state[9] = {0};
    _gripper->GetRunStates(tmp_state);
    msg.rot_state       = tmp_state[5];
    msg.angle           = tmp_state[6];
    msg.target_angle    = tmp_state[7];
    msg.target_force    = tmp_state[8];
//    seq++;
}

int main(int argc, char** argv)
{
	rclcpp::init(argc, argv);
	auto n = rclcpp::Node::make_shared("dh_gripper_driver");
//    ros::NodeHandle n("~");

    n->declare_parameter<int>("Gripper_ID", 1);
    n->declare_parameter<std::string>("Gripper_Model", "AG95_MB");
    n->declare_parameter<std::string>("Connect_port", "/dev/ttyUSB0");
    n->declare_parameter<int>("BaudRate", 115200);

    n->get_parameter("Gripper_ID", _gripper_ID);
    n->get_parameter("Gripper_Model", _gripper_model);
    n->get_parameter("Connect_port", _gripper_connect_port);
    n->get_parameter("BaudRate", _gripper_Baudrate);

    RCLCPP_INFO(n->get_logger(),"Gripper_ID : %s", _gripper_ID.value_to_string().c_str());
    RCLCPP_INFO(n->get_logger(),"Gripper_model : %s", _gripper_model.value_to_string().c_str());
    RCLCPP_INFO(n->get_logger(),"Connect_port: %s", _gripper_connect_port.value_to_string().c_str());
    RCLCPP_INFO(n->get_logger(),"BaudRate : %s (In TCP/IP Mode , BaudRate is unuse)", _gripper_Baudrate.value_to_string().c_str());

    DH_Gripper_Factory* _gripper_Factory = new DH_Gripper_Factory();
    _gripper_Factory->Set_Parameter(atoi(_gripper_ID.value_to_string().c_str()), _gripper_connect_port.as_string(), atoi(_gripper_Baudrate.value_to_string().c_str()));


    _gripper = _gripper_Factory->CreateGripper(_gripper_model.as_string());
    if(_gripper == NULL)
    {
    	RCLCPP_ERROR(n->get_logger(),"No this Model :%s", _gripper_model.value_to_string().c_str());
        return -1;
    }

    if(_gripper->open()<0)
    {
    	RCLCPP_ERROR(n->get_logger(),"Unable to open commport to %s", _gripper_connect_port.value_to_string().c_str());
        return -1;
    }

    //initialize the gripper
    int initstate = 0;
    _gripper->GetInitState(initstate);
    if(initstate != DH_Gripper::S_INIT_FINISHED)
    {
        _gripper->Initialization();
        std::cout<< " Send grip init " << std::endl;

        //wait for gripper initialization
        initstate = 0;
        std::cout<< " wait grip initialized " << std::endl;
        while(initstate != DH_Gripper::S_INIT_FINISHED )
            _gripper->GetInitState(initstate);
        std::cout<< "GetInitState "<< initstate << std::endl;
    }

//    ros::Subscriber _sub_grip ;
    rclcpp::Subscription<dh_gripper_ros2::msg::GripperCtrl>::SharedPtr _sub_grip;
//    auto _sub_grip = n->create_subscription<dh_gripper_ros2::msg::GripperCtrl>("/gripper/ctrl", 50, update_gripper_control);

    rclcpp::Publisher<dh_gripper_ros2::msg::GripperState>::SharedPtr _gripper_state_pub;
//    _gripper_state_pub = n->create_publisher<dh_gripper_ros2::msg::GripperState>("/gripper/states", 50);
////    ros::Publisher _gripper_state_pub;

    rclcpp::Subscription<dh_gripper_ros2::msg::GripperRotCtrl>::SharedPtr _sub_rot;
    rclcpp::Publisher<dh_gripper_ros2::msg::GripperRotState>::SharedPtr _rot_state_pub;
////    ros::Subscriber _sub_rot ;
////    ros::Publisher _rot_state_pub;
//
    _sub_grip = n->create_subscription<dh_gripper_ros2::msg::GripperCtrl>("/gripper/ctrl", 50, update_gripper_control);
    _gripper_state_pub = n->create_publisher<dh_gripper_ros2::msg::GripperState>("/gripper/states", 50);
//
    if(_gripper->GetGripperAxiNumber()==2)
    {
    	_sub_rot = n->create_subscription<dh_gripper_ros2::msg::GripperRotCtrl>("/gripper/rot_ctrl", 50, update_rotation_control);
    	_rot_state_pub = n->create_publisher<dh_gripper_ros2::msg::GripperRotState>("/gripper/rot_states", 50);
    }
//    //TODO
    auto _gripper_joint_state_pub = n->create_publisher<sensor_msgs::msg::JointState>("/gripper/joint_states", 50);
    rclcpp::Rate loop_rate(10);
////    ros::Rate loop_rate(50);
     while (rclcpp::ok())
     {
         dh_gripper_ros2::msg::GripperState msg_g_state;
         update_gripper_state(msg_g_state);
        _gripper_state_pub->publish(msg_g_state);

        sensor_msgs::msg::JointState msg_g_joint_state;
        update_gripper_joint_state(msg_g_joint_state);
        _gripper_joint_state_pub->publish(msg_g_joint_state);

        if(_gripper->GetGripperAxiNumber()==2)
        {
            dh_gripper_ros2::msg::GripperRotState msg_r_state;
            update_rotation_state(msg_r_state);
            _rot_state_pub->publish(msg_r_state);

        }

//        ros::spinOnce();
        rclcpp::spin_some(n);
        loop_rate.sleep();
    }


    _gripper->close();


    return 0;

}


