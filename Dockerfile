FROM ros:humble

# Install build tools and all display/graphics libraries required by Webots
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep \
    build-essential \
    libgl1-mesa-glx \
    libgl1-mesa-dri \
    libglu1-mesa \
    libegl1-mesa \
    libgles2-mesa \
    libxrender1 \
    libxi6 \
    libxtst6 \
    libxext6 \
    libx11-6 \
    libxkbcommon0 \
    libxkbcommon-x11-0 \
    libxcb1 \
    libxcb-xinerama0 \
    libxcb-cursor0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxcb-xfixes0 \
    libxcb-util1 \
    libdbus-1-3 \
    libfontconfig1 \
    libasound2 \
    libpulse0 \
    libglib2.0-0 \
    libopenal1 \
    libsndio7.0 \
    xvfb \
    ros-humble-rviz2 \
    && rm -rf /var/lib/apt/lists/*

# Set workspace as working directory
WORKDIR /ros2_ws

# Copy package manifests so rosdep can read dependencies
COPY ros2_ws/src ./src

# Install all ROS2 package dependencies declared in package.xml files
RUN apt-get update && \
    rosdep update && \
    rosdep install --from-paths src --ignore-src -r -y && \
    rm -rf /var/lib/apt/lists/*

# Source ROS2 and workspace setup on every bash session
RUN echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc && \
    echo "[ -f /ros2_ws/install/setup.bash ] && source /ros2_ws/install/setup.bash" >> ~/.bashrc

CMD ["/bin/bash"]
