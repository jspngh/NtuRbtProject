cmake_minimum_required(VERSION 2.8)
project( connect4 )
add_executable( connect4 main.cpp board.cpp)
set_property(TARGET connect4 PROPERTY CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-Wall -Wextra" )