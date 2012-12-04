project(bayer-filter)
cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)

add_definitions("-Wall -std=c++11")

add_subdirectory(OpenCLInterface)
include_directories(OpenCLInterface BayerFilter ${OpenCV_INCLUDE_DIRS})

message(${OpenCV_SHARED})

set(bayer_filter_SOURCES
  BayerFilter/main.cpp
  BayerFilter/OpenCLImageFilter.cpp
  BayerFilter/BayerFilterStream.cpp)

add_executable(bayer-filter ${bayer_filter_SOURCES})
target_link_libraries(bayer-filter ${OpenCV_LIBS} opencl-interface)
