cmake_minimum_required(VERSION 2.8)
project( connect4 )
find_package( OpenCV REQUIRED )
add_executable( connect4
  main.cpp
  board.cpp
  algorithm/minimax.cpp
  vision/VisionManager.cpp vision/KinectManager.cpp
  hci/HCI.cpp hci/LTexture.cpp
  ai/AI.cpp
  robot/robot.cpp
  robot/server.cpp)
set_property(TARGET connect4 PROPERTY CXX_STANDARD 11)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
include_directories( /usr/include/libusb-1.0)
target_link_libraries( connect4 ${OpenCV_LIBS} freenect pthread ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2MIXER_LIBRARIES})
set ( CMAKE_CXX_FLAGS "-Wall -Wextra" )
