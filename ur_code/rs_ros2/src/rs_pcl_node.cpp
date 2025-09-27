#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <librealsense2/rs.hpp>

class PointCloudPublisher : public rclcpp::Node
{
public:
    PointCloudPublisher()
        : Node("pointcloud_publisher")
    {
        // Declare ROS2 parameters
        this->declare_parameter<std::string>("color_topic", "/D435i/camera/color/image_raw");
        this->declare_parameter<std::string>("depth_topic", "/D435i/camera/aligned_depth_to_color/image_raw");
        this->declare_parameter<std::string>("camera_info_topic", "/D435i/camera/aligned_depth_to_color/camera_info");
        this->declare_parameter<std::string>("pointcloud_topic", "/D435i/camera/color/aligned_points");
        this->declare_parameter<double>("timer_rate_", 1.0);
        this->declare_parameter<double>("publish_rate_", 25.0);

        // Get parameter values
        color_topic_ = this->get_parameter("color_topic").as_string();
        depth_topic_ = this->get_parameter("depth_topic").as_string();
        info_topic_ = this->get_parameter("camera_info_topic").as_string();
        pc_topic_ = this->get_parameter("pointcloud_topic").as_string();
        timer_rate_ = this->get_parameter("timer_rate_").as_double();
        publish_rate_ = this->get_parameter("publish_rate_").as_double();

        RCLCPP_INFO(this->get_logger(), 
            "color_topic=%s, depth_topic=%s, camera_info_topic=%s, pointcloud_topic=%s, timer_rate_=%.2f, publish_rate=%.2f",
            color_topic_.c_str(), depth_topic_.c_str(), info_topic_.c_str(), pc_topic_.c_str(), timer_rate_, publish_rate_);

        // Subscribe to color, depth, and camera info topics
        color_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            color_topic_, 10, std::bind(&PointCloudPublisher::colorCallback, this, std::placeholders::_1));
        depth_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
            depth_topic_, 10, std::bind(&PointCloudPublisher::depthCallback, this, std::placeholders::_1));
        info_sub_ = this->create_subscription<sensor_msgs::msg::CameraInfo>(
            info_topic_, 1, std::bind(&PointCloudPublisher::infoCallback, this, std::placeholders::_1));
        
        // Publisher for point cloud
        pc_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(pc_topic_, 10);

        last_time_ = std::chrono::steady_clock::now();

        // Timer to print FPS for debugging
        fps_timer_ = this->create_wall_timer(
            std::chrono::duration<double>(1.0 / timer_rate_),
            std::bind(&PointCloudPublisher::printFPS, this));

        // Timer to control point cloud publishing rate
        publish_timer_ = this->create_wall_timer(
            std::chrono::duration<double>(1.0 / publish_rate_),
            std::bind(&PointCloudPublisher::publishPointCloud, this));
    }

private:
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr color_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr depth_sub_;
    rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr info_sub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pc_pub_;

    size_t color_count_ = 0;
    size_t depth_count_ = 0;
    size_t pc_count_ = 0;

    std::chrono::steady_clock::time_point last_time_;
    rclcpp::TimerBase::SharedPtr fps_timer_;
    rclcpp::TimerBase::SharedPtr publish_timer_;

    std::string color_topic_;
    std::string depth_topic_;
    std::string info_topic_;
    std::string pc_topic_;
    double timer_rate_;
    double publish_rate_;

    cv::Mat color_img_;
    cv::Mat depth_img_;
    rs2_intrinsics intrinsics_;
    bool intrinsics_received_ = false;

    // Callback to receive camera intrinsics
    void infoCallback(const sensor_msgs::msg::CameraInfo::SharedPtr msg)
    {
        if (intrinsics_received_) return;

        intrinsics_.width = msg->width;
        intrinsics_.height = msg->height;
        intrinsics_.ppx = msg->k[2];
        intrinsics_.ppy = msg->k[5];
        intrinsics_.fx = msg->k[0];
        intrinsics_.fy = msg->k[4];
        intrinsics_.model = RS2_DISTORTION_NONE; 
        for (int i = 0; i < 5; i++) intrinsics_.coeffs[i] = msg->d[i];

        intrinsics_received_ = true;
        RCLCPP_INFO(this->get_logger(), "Camera intrinsics received.");
    }

    // Callback to receive color image
    void colorCallback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        try {
            color_img_ = cv_bridge::toCvCopy(msg, "bgr8")->image;
            color_count_++;
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        }
    }

    // Callback to receive depth image
    void depthCallback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
        try {
            depth_img_ = cv_bridge::toCvCopy(msg, msg->encoding)->image;
            depth_count_++;
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        }
    }

    // Publish point cloud at the specified publish_rate_
    void publishPointCloud()
    {   
        if (!intrinsics_received_ || color_img_.empty() || depth_img_.empty()) {
            // Print which data is still missing
            RCLCPP_INFO(this->get_logger(),
                "Waiting for data: intrinsics_received=%s, color_img_empty=%s, depth_img_empty=%s",
                intrinsics_received_ ? "true" : "false",
                color_img_.empty() ? "true" : "false",
                depth_img_.empty() ? "true" : "false"
            );
            return;
        }

        int height = depth_img_.rows;
        int width = depth_img_.cols;

        // Create PointCloud2 message
        auto pc_msg = sensor_msgs::msg::PointCloud2();
        pc_msg.header.stamp = this->get_clock()->now();
        pc_msg.header.frame_id = "camera_depth_optical_frame";
        pc_msg.height = 1;
        pc_msg.width = width * height;
        pc_msg.is_dense = false;
        pc_msg.is_bigendian = false;
        pc_msg.point_step = 16; // x,y,z,rgb float/uint32
        pc_msg.row_step = pc_msg.point_step * pc_msg.width;

        // Define fields for PointCloud2
        pc_msg.fields.resize(4);
        pc_msg.fields[0].name = "x"; pc_msg.fields[0].offset = 0; pc_msg.fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32; pc_msg.fields[0].count = 1;
        pc_msg.fields[1].name = "y"; pc_msg.fields[1].offset = 4; pc_msg.fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32; pc_msg.fields[1].count = 1;
        pc_msg.fields[2].name = "z"; pc_msg.fields[2].offset = 8; pc_msg.fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32; pc_msg.fields[2].count = 1;
        pc_msg.fields[3].name = "rgb"; pc_msg.fields[3].offset = 12; pc_msg.fields[3].datatype = sensor_msgs::msg::PointField::UINT32; pc_msg.fields[3].count = 1;

        pc_msg.data.resize(pc_msg.row_step);

        // Fill point cloud data
        sensor_msgs::PointCloud2Iterator<float> iter_x(pc_msg, "x");
        sensor_msgs::PointCloud2Iterator<float> iter_y(pc_msg, "y");
        sensor_msgs::PointCloud2Iterator<float> iter_z(pc_msg, "z");
        sensor_msgs::PointCloud2Iterator<uint32_t> iter_rgb(pc_msg, "rgb");

        for (int v = 0; v < height; v++)
        {
            for (int u = 0; u < width; u++, ++iter_x, ++iter_y, ++iter_z, ++iter_rgb)
            {
                float d = static_cast<float>(depth_img_.at<uint16_t>(v, u)) / 1000.0f; // convert mm -> m
                if (d <= 0.0f) {
                    *iter_x = *iter_y = *iter_z = std::numeric_limits<float>::quiet_NaN();
                    *iter_rgb = 0;
                } else {
                    *iter_x = (u - intrinsics_.ppx) * d / intrinsics_.fx;
                    *iter_y = (v - intrinsics_.ppy) * d / intrinsics_.fy;
                    *iter_z = d;

                    cv::Vec3b color = color_img_.at<cv::Vec3b>(v, u);
                    *iter_rgb = (static_cast<uint32_t>(color[2]) << 16 |
                                static_cast<uint32_t>(color[1]) << 8 |
                                static_cast<uint32_t>(color[0]));
                }
            }
        }

        // Publish point cloud
        pc_pub_->publish(pc_msg);
        pc_count_++;
    }

    // Print FPS for color, depth, and point cloud
    void printFPS()
    {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - last_time_).count();

        std::cout << "Color FPS: " << color_count_ / elapsed
                  << ", Depth FPS: " << depth_count_ / elapsed
                  << ", PointCloud FPS: " << pc_count_ / elapsed << std::endl;

        color_count_ = 0;
        depth_count_ = 0;
        pc_count_ = 0;
        last_time_ = now;
    }
};

int main(int argc, char **argv)
{   
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PointCloudPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
