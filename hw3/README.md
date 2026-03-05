# ENPM690 HW 3
This repositiory is a containerized ros2 workspace with package "hw3_690" that simulates a custom differential drive robot in Webots. The robot is able to be manually piloted by running a custom teleoperation node, or can complete an autonomous tracking routine using its onboard camera by running a custom follower node. The workspace is containerized using Docker Compose, a tool for defining and running multi-container applications.

## File Structure
```
├── docker-compose.yml                      # Manages services and volumes for Docker container
├── Dockerfile                              # Defines Docker container / installations
└── ros2_ws                                 # ROS2 workspace
    └── src                                 
        └── hw3_690                         # ROS2 package 
            ├── include                     # Header files for ros2 nodes
            │   ├── MyFollower.hpp          
            │   ├── MyRobotDriver.hpp
            │   └── MyTeleop.hpp
            ├── launch                 
            │   └── robot_launch.launch.py  # Launch file launches custom robot in custom world
            ├── resource
            │   └── custom_bot.urdf         # URDF for custom robot 
            ├── src                         
            │   ├── MyFollower.cpp          # Source file for autonomous follow node
            │   ├── MyRobotDriver.cpp       # Source file for custom robot driver plugin
            │   └── MyTeleop.cpp            # Source file for custom teleoperation node
            └── world
            │   ├── CustomBot.proto         # Defines custom robot with Webots PROTO system
            │   ├── custom_world.wbt        # Defines custom world with Webots PROTO system
            │   └── meshes                  # Custom meshes designed in CAD for robot 
            │       ├── base_link.dae
            │       └── wheel_link.dae
            ├── my_robot_driver.xml         # XML for custom robot driver plugin
            ├── CMakeLists.txt              
            └──  package.xml
```
## Dependencies
- [Docker Engine](https://docs.docker.com/engine/install/)
- [Docker Compose](https://docs.docker.com/compose/install/)


## Build and Run Instructions
Install dependencies, then navigate to ENPM690_Zinobile and run:

```bash
xhost +local:docker                    # Allow Docker to access your display
sudo docker-compose build              # Build docker image
sudo docker-compose up -d              # Run container
sudo docker-compose exec hw3_690 bash  # Enter container
```
Once inside the container, build and source the package:
```bash
colcon build
source install/setup.bash
```
NOTE: the first time you build, you may get an error regarding an unused parameter. Build and source again if this occurs. 

### Launch Simulation
```bash
ros2 launch hw3_690 robot_launch.launch.py
```
A Webots window should appear showing the robot in its environment, with a small camera overlay displaying the robot's POV.

### Manually Pilot Robot
Open a new terminal and run:
```bash
sudo docker-compose exec hw3_690 bash
source install/setup.bash
ros2 run hw3_690 teleop_node
```
Keeping the terminal window selected, use the WASD keys and space bar to pilot the robot. The ros2 Info Log will display with the current action. Press Q once finished. 

### Autonomous Tracking
The autonomous tracking function directs the robot to search for and approach the largest yellow object in range, in this case a rubber duck. The tracking algorithm has three steps:
 - Turn clockwise until rubber duck detected
 - Move forward while turning to keep rubber duck in center of frame
 - Stop when the area of yellow pixels is a certain percentage of the total screen area
Once the robot stops, the duck can be moved around manually in Webots to make the robot track and follow it.

The screen area percentage is an adjustable parameter, to influence how close the robot gets before stopping. To run with the default value of 20% and default robot linear and angular speeds, run:
```bash
ros2 run hw3_690 follower_node
```
To run with different values for stop fraction, linear speed, or angular speed, run:
```bash
ros2 run hw3_690 follower_node --ros-args -p stop_fraction:=0.1 -p linear_speed:=0.25 -p angular_speed:=0.5 # Change values as needed
```
The robot publishes images of the conturs and centroid of the tracked area on screen. To see these images, open a new terminal and run:
```bash
sudo docker-compose exec hw3_690 bash
source install/setup.bash
ros2 run rviz2 rviz2
```
Then click add -> by topic -> tracked image -> image. 

