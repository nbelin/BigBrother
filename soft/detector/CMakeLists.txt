cmake_minimum_required(VERSION 2.8)

set(NAME detector)
project(${NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SOURCE_CPP
  "source/main.cpp"
  "source/rectangle.cpp"
  "source/marker.cpp"
  "source/communication.cpp"
  )
file(GLOB_RECURSE SOURCE_HPP "source/*.hpp")
file(GLOB_RECURSE CONFIG_HPP "config/*.c")

set(SOURCE
  ${SOURCE_CPP}
  ${SOURCE_HPP}
  ${CONFIG_HPP}
  )

link_libraries(${OpenCV_LIBS})
add_executable(${NAME} ${SOURCE})
