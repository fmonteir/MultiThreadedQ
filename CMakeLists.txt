cmake_minimum_required(VERSION 3.29)
project(MultiThreadedQ VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include_directories(include)
add_executable(unit_tests src/test.cpp)