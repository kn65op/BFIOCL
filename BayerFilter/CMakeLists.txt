project(bayer-filter)
cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)

add_subdirectory(OpenCLInterface)
include_directories(OpenCLInterface BayerFilter ${OpenCV_INCLUDE_DIRS})

message(${OpenCV_SHARED})

add_executable(bayer-filter BayerFilter/main.cpp BayerFilter/OpenCLImageFilter.cpp)
target_link_libraries(bayer-filter ${OpenCV_LIBS} opencl-interface)
