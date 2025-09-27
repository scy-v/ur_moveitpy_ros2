cmake_minimum_required(VERSION 3.8)
project(dh_gripper_ros2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/GripperCtrl.msg"
  "msg/GripperRotCtrl.msg"
  "msg/GripperRotState.msg"
  "msg/GripperState.msg"
  DEPENDENCIES sensor_msgs std_msgs # Add packages that above messages depend on, in this case geometry_msgs for Sphere.msg
)
# uncomment the following section in order to fill in
# further dependencies manually.

# Set dependencies
set(THIS_PACKAGE_INCLUDE_DEPENDS
	ament_cmake
	rclcpp
	rosidl_default_runtime
)

# Create a single library target to use for the driver. 
set(library_name dh_gripper_driver)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

add_library(${library_name} 
	SHARED
	include/${library_name}/src/dh_device.cpp
	include/${library_name}/src/dh_ag95_can.cpp
	include/${library_name}/src/dh_dh3_can.cpp
	include/${library_name}/src/dh_lagacy_gripper.cpp
	include/${library_name}/src/dh_modbus_gripper.cpp
	include/${library_name}/src/dh_rgi.cpp
)
ament_target_dependencies(
	${library_name} 
	${THIS_PACKAGE_INCLUDE_DEPENDS}
)
target_link_libraries(${library_name})
target_include_directories(${library_name}
	PRIVATE
	include/${library_name}/include
)

#######

set(executable_name dh_PGE_gripper_ros2)

add_executable(${executable_name}
	src/dh_PGE_gripper_ros2.cpp
)
ament_target_dependencies(
	${executable_name} 
	${THIS_PACKAGE_INCLUDE_DEPENDS}
)
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} rosidl_typesupport_cpp)
target_link_libraries(${executable_name}  ${library_name} ${cpp_typesupport_target})
target_include_directories(${executable_name}
	PRIVATE
	include/${library_name}/include
)
add_executable(dh_PGE_gripper_ros2_test   
	src/dh_PGE_gripper_ros2_test.cpp
)
ament_target_dependencies(
	dh_PGE_gripper_ros2_test 
	${THIS_PACKAGE_INCLUDE_DEPENDS}
)
rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME} rosidl_typesupport_cpp)
target_link_libraries(dh_PGE_gripper_ros2_test  ${library_name} ${cpp_typesupport_target})
target_include_directories(dh_PGE_gripper_ros2_test
	PRIVATE
	include/${library_name}/include
)
#############
## Install ##
#############

install(
  TARGETS ${executable_name} dh_PGE_gripper_ros2_test ${library_name}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
  )
install(
  DIRECTORY include/
  DESTINATION include
)

ament_export_include_directories(include)
ament_export_libraries(${library_name})
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})
 
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
