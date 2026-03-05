#include "MyTeleop.hpp"

#include <termios.h>
#include <unistd.h>

MyTeleop::MyTeleop() : Node("my_teleop") {
  cmd_vel_publisher_ =
      create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
}

char MyTeleop::getKey() {
  struct termios old_tio, new_tio;
  tcgetattr(STDIN_FILENO, &old_tio);
  new_tio = old_tio;
  new_tio.c_lflag &= ~(ICANON | ECHO);  // raw mode, no echo
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

  char ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);  // restore
  return ch;
}

void MyTeleop::publishVelocity(double linear, double angular) {
  geometry_msgs::msg::Twist msg;
  msg.linear.x = linear;
  msg.angular.z = angular;
  cmd_vel_publisher_->publish(msg);
}

void MyTeleop::run() {
  RCLCPP_INFO(get_logger(),
    "\nTeleop controls:\n"
    "  W - forward\n"
    "  S - backward\n"
    "  A - turn left\n"
    "  D - turn right\n"
    "  Space - stop\n"
    "  Q - quit\n");

  while (rclcpp::ok()) {
    char key = getKey();

    switch (key) {
      case 'w': case 'W':
        publishVelocity(LINEAR_SPEED, 0.0);
        RCLCPP_INFO(get_logger(), "Forward");
        break;
      case 's': case 'S':
        publishVelocity(-LINEAR_SPEED, 0.0);
        RCLCPP_INFO(get_logger(), "Backward");
        break;
      case 'a': case 'A':
        publishVelocity(0.0, ANGULAR_SPEED);
        RCLCPP_INFO(get_logger(), "Turn left");
        break;
      case 'd': case 'D':
        publishVelocity(0.0, -ANGULAR_SPEED);
        RCLCPP_INFO(get_logger(), "Turn right");
        break;
      case ' ':
        publishVelocity(0.0, 0.0);
        RCLCPP_INFO(get_logger(), "Stop");
        break;
      case 'q': case 'Q':
        publishVelocity(0.0, 0.0);
        RCLCPP_INFO(get_logger(), "Quit");
        return;
      default:
        break;
    }

    rclcpp::spin_some(shared_from_this());
  }
}

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyTeleop>();
  node->run();
  rclcpp::shutdown();
  return 0;
}
