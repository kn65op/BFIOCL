project(bayer-filter)
cmake_minimum_required(VERSION 2.8)


add_subdirectory(OpenCLInterface)
include_directories(OpenCLInterface)


add_executable(bayer-filter BayerFilter/main.cpp)
target_link_libraries(bayer-filter opencl-interface)
