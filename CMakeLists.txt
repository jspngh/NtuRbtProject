cmake_minimum_required(VERSION 2.8)
project( connect4 )
find_package( OpenCV REQUIRED )
add_executable( connect4
  main.cpp
  board.cpp
  algorithm/minimax.cpp
  vision/VisionManager.cpp vision/KinectManager.cpp
  ai/AI.cpp
  robot/robot.cpp
  robot/server.cpp)
set_property(TARGET connect4 PROPERTY CXX_STANDARD 11)
include_directories( /usr/include/libusb-1.0)
target_link_libraries( connect4 ${OpenCV_LIBS} freenect pthread)
set ( CMAKE_CXX_FLAGS "-Wall -Wextra" )
