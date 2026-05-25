# OpenGL

## Linux Prerequisites

Before compiling the project on Linux, you need to install the required graphics drivers, window management development headers, and build tools. 

Run the following command in your terminal to install all dependencies at once:

```bash
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    libwayland-dev \
    wayland-protocols \
    extra-cmake-modules \
    libx11-xcb-dev \
    libxkbcommon-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev