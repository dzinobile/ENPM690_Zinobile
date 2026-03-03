#include "MyFollower.hpp"
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <memory>

using std::placeholders::_1;

MyFollower::MyFollower() : Node("my_follower") {
    camera_subscriber_ =
        this->create_subscription<sensor_msgs::msg::Image>(
            "CustomBot/camera1/image_color",
            1,
            std::bind(&MyFollower::followObject, this, _1));

    camera_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("tracked_image", 3);
    cmd_vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
}

void MyFollower::followObject(const sensor_msgs::msg::Image::SharedPtr msg) {
    // Convert ROS image to OpenCV Mat (BGR)
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
    } catch (const cv_bridge::Exception & e) {
        RCLCPP_ERROR(get_logger(), "cv_bridge exception: %s", e.what());
        return;
    }
    cv::Mat frame = cv_ptr->image;

    // Convert to HSV for colour-based thresholding
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Yellow in HSV: H 20-35, S 100-255, V 100-255
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(20, 100, 100), cv::Scalar(35, 255, 255), mask);

    // Find external contours in the mask
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    geometry_msgs::msg::Twist cmd;

    if (!contours.empty()) {
        // Pick the largest contour (most likely the target object)
        auto largest = std::max_element(
            contours.begin(), contours.end(),
            [](const auto & a, const auto & b) {
                return cv::contourArea(a) < cv::contourArea(b);
            });

        double area = cv::contourArea(*largest);
        cv::Moments m = cv::moments(*largest);
        int cx = static_cast<int>(m.m10 / m.m00);
        int cy = static_cast<int>(m.m01 / m.m00);

        // Visualise: draw contour and centroid on the output frame
        cv::drawContours(frame,
            std::vector<std::vector<cv::Point>>{*largest}, -1,
            cv::Scalar(0, 255, 0), 2);
        cv::circle(frame, cv::Point(cx, cy), 8, cv::Scalar(0, 0, 255), -1);

        double image_area = static_cast<double>(frame.cols * frame.rows);

        if (area >= STOP_AREA_FRACTION * image_area) {
            // Object is close enough — stop
            cmd.linear.x = 0.0;
            cmd.angular.z = 0.0;
            RCLCPP_INFO(get_logger(), "Object reached target size (area=%.0f). Stopping.", area);
        } else {
            // Drive forward and steer to keep object centred horizontally
            double normalised_error = (cx - frame.cols / 2.0) / (frame.cols / 2.0);
            cmd.linear.x = LINEAR_SPEED;
            cmd.angular.z = -ANGULAR_SPEED * normalised_error;
        }
    } else {
        // No yellow detected — spin in place to search
        cmd.linear.x = 0.0;
        cmd.angular.z = ANGULAR_SPEED;
    }

    cmd_vel_publisher_->publish(cmd);

    // Publish annotated image for debugging / visualisation
    auto out_msg = cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg();
    camera_publisher_->publish(*out_msg);
}

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MyFollower>());
    rclcpp::shutdown();
    return 0;
}
