cmake_minimum_required(VERSION 3.3)
project(AS_DI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(AS_DI ${SOURCE_FILES})