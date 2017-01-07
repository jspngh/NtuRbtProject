cmake_minimum_required(VERSION 2.8)
project( vision )
find_package( OpenCV REQUIRED )
add_executable( vision
  VisionManager.cpp
  KinectManager.cpp)
set_property(TARGET vision PROPERTY CXX_STANDARD 11)

include_directories( /usr/include/libusb-1.0)
target_link_libraries( vision ${OpenCV_LIBS} freenect pthread)
set ( CMAKE_CXX_FLAGS "-Wall -Wextra" )
