cmake_minimum_required(VERSION 3.16)
project(epmc_v2_cpp LANGUAGES CXX)

include_directories(include)

add_executable( motor_control examples/motor_control.cpp )
target_link_libraries( motor_control serial )

add_executable( read_imu examples/read_imu.cpp )
target_link_libraries( read_imu serial )

