#pragma once

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

  double linear_speed_ = 0.5;
  double angular_speed_ = 1.0;
  double stop_fraction_;
};
