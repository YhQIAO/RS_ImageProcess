cmake_minimum_required(VERSION 3.17)
project(RS_image_process)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_executable(RS_image_process  Problem6-2.cpp)
target_link_libraries(RS_image_process ${OpenCV_LIBS})