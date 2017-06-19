cmake_minimum_required(VERSION 2.6)
project(fractal)
set(fractal_VERSION_MAJOR 1)
set(fractal_VERSION_MINOR 0)
set(CMAKE_MODULE_PATH /usr/local/share/cmake/Modules)

find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)
find_package(Freetype REQUIRED)
find_package(FTGL REQUIRED)

#find_package(Boost COMPONENTS system REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})

include_directories(${FREETYPE_INCLUDE_DIRS} ${GLFW_INCLUDE_DIR})

add_definitions(--std=c++11)

add_executable(fractal main.cpp PickStack.cpp MyFont.cpp BaseGL.cpp)
target_link_libraries(fractal ${FTGL_LIBRARY} ${FREETYPE_LIBRARY} ${GLFW_LIBRARY} ${OPENGL_LIBRARIES})
