cmake_minimum_required(VERSION 2.6)
project(ocv)
set(ocv_VERSION_MAJOR 1)
set(ocv_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH /usr/local/share/cmake/Modules)

find_package(OpenCV REQUIRED)

message(STATUS "opencv_libs " ${OpenCV_LIBS})
#find_package(Boost COMPONENTS system REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})

add_definitions(-std=c++14)

add_executable(fft fft.cpp)
target_link_libraries(fft ${OpenCV_LIB_COMPONENTS} ${OpenCV_LIBS})

add_executable(hough hough.cpp)
target_link_libraries(hough ${OpenCV_LIB_COMPONENTS} ${OpenCV_LIBS})
