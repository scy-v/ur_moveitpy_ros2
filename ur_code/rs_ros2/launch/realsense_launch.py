from launch import LaunchDescription
from launch.actions import OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import sys
import os
from ament_index_python.packages import get_package_share_directory

# Add realsense2_camera launch directory to path to import rs_launch
sys.path.append(os.path.join(get_package_share_directory('realsense2_camera'), 'launch'))
import rs_launch

# Local configurable parameters for the camera node
local_parameters = [    
    {'name': 'camera_name', 'default': 'camera', 'description': 'camera unique name'},
    {'name': 'camera_namespace', 'default': 'D435i', 'description': 'namespace for camera'},
    {'name': 'enable_color', 'default': 'true', 'description': 'enable color stream'},
    {'name': 'enable_depth', 'default': 'true', 'description': 'enable depth stream'},
    {'name': 'align_depth.enable', 'default': 'true', 'description': 'enable align depth filter'},
    {'name': 'enable_sync', 'default': 'true', 'description': 'enable sync mode'},
    {'name': 'pointcloud.enable', 'default': 'false', 'description': 'enable pointcloud'},
    {'name': 'pointcloud.ordered_pc', 'default': 'false', 'description': 'publish ordered point cloud'},
    {'name': 'pointcloud.allow_no_texture_points', 'default': 'false', 'description': 'allow points with no texture'},
    {'name': 'rgb_camera.color_profile', 'default': '640,480,30', 'description': 'color stream profile (width,height,fps)'},
    {'name': 'depth_module.depth_profile', 'default': '640,480,30', 'description': 'depth stream profile (width,height,fps)'},
    {'name': 'publish_tf', 'default': 'false', 'description': '[bool] enable/disable publishing static & dynamic TF'},
    {'name': 'decimation_filter.enable', 'default': 'false', 'description': 'enable decimation filter'},
    {'name': 'spatial_filter.enable', 'default': 'false', 'description': 'enable spatial filter'},
    {'name': 'temporal_filter.enable', 'default': 'false', 'description': 'enable temporal filter'},
    {'name': 'disparity_filter.enable', 'default': 'false', 'description': 'enable disparity filter'},
    {'name': 'hole_filling_filter.enable', 'default': 'true', 'description': 'enable hole filling filter'},
]

# Helper to convert local parameters to LaunchConfiguration dict
def set_configurable_parameters(local_params):
    return dict([(param['name'], LaunchConfiguration(param['name'])) for param in local_params])

def generate_launch_description():
    params = rs_launch.configurable_parameters

    # Node for custom point cloud publisher
    pointcloud_node = Node(
        package='rs_ros2',  
        executable='rs_pcl_node', 
        name='pointcloud_publisher',
        output='screen',
        parameters=[{
            'color_topic': '/D435i/camera/color/image_raw',
            'depth_topic': '/D435i/camera/aligned_depth_to_color/image_raw',
            'camera_info_topic': '/D435i/camera/aligned_depth_to_color/camera_info',
            'pointcloud_topic': '/D435i/camera/color/aligned_points',
            'publish_rate_': 30.0,
            'timer_rate_': 0.5
        }]
    )

    # Launch both the realsense camera and custom pointcloud node
    return LaunchDescription(
        rs_launch.declare_configurable_parameters(local_parameters) +
        rs_launch.declare_configurable_parameters(params) + 
        [
            OpaqueFunction(function=rs_launch.launch_setup,
                           kwargs={'params': set_configurable_parameters(params)}),
            pointcloud_node
        ]
    )
