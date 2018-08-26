cmake_minimum_required(VERSION 2.8)
project( analyze )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/share/cmake/Modules/")

find_package(OpenCV REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost COMPONENTS thread system REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${PORTAUDIO_INCLUDE_DIRS} )
include_directories( ${EIGEN_INCLUDE_DIRS} )

add_definitions(-O3)

add_executable( cppRecord cppRecord.cpp )
target_link_libraries(cppRecord ${PORTAUDIO_LIBRARIES} ${OpenCV_LIBS} ${EIGEN_LIBS} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY})
