#pragma once

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"

class MyTeleop : public rclcpp::Node {
public:
  MyTeleop();
  void run();

private:
  void publishVelocity(double linear, double angular);
  char getKey();

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_publisher_;

  static constexpr double LINEAR_SPEED  = 0.5;
  static constexpr double ANGULAR_SPEED = 1.0;
};
