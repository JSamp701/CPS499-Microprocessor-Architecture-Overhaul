cmake_minimum_required(VERSION 3.3)
project(AS_DI)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_library(dummy MODULE source/jsamp701-asapi-dummy-impl.c)

#set(SOURCE_FILES main.cpp)
#add_executable(AS_DI ${SOURCE_FILES})