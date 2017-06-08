cmake_minimum_required(VERSION 2.6)
project(RockPaperScissors)
set(RockPaperScissors_VERSION_MAJOR 1)
set(RockPaperScissors_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH /usr/local/share/cmake/Modules)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Freetype REQUIRED)
find_package(FTGL REQUIRED)

#find_package(Boost COMPONENTS system REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})

include_directories(${FREETYPE_INCLUDE_DIRS})

add_definitions(-std=c++14)

add_executable(RockPaperScissors src/main.cpp)
target_link_libraries(RockPaperScissors ${FTGL_LIBRARY} ${FREETYPE_LIBRARY} ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})
