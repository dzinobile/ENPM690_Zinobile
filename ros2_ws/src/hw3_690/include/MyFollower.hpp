#pragma once
//topic CustomBot/camera1/image_color, type sensor_msgs/msg/Image
#include "sensor_msgs/msg/image.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"


class MyFollower : public rclcpp::Node {
public:
  MyFollower();

private:
  void followObject(const sensor_msgs::msg::Image::SharedPtr msg);

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr camera_subscriber_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr camera_publisher_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;

  static constexpr double LINEAR_SPEED  = 0.5;
  static constexpr double ANGULAR_SPEED = 1.0;
  // Stop when object area exceeds this fraction of the total image area
  static constexpr double STOP_AREA_FRACTION = 0.20;
};
