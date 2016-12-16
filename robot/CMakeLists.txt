cmake_minimum_required(VERSION 2.8)
add_executable(main main.cpp server.cpp robot.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 11)
