cmake_minimum_required(VERSION 2.6)
project(ocv)
set(ocv_VERSION_MAJOR 1)
set(ocv_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH /usr/local/share/cmake/Modules)

find_package(OpenCV REQUIRED)

#find_package(Boost COMPONENTS system REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})

add_definitions(-std=c++14)

add_executable(ocv src/main.cpp)
target_link_libraries(ocv ${OpenCV_LIBS})
