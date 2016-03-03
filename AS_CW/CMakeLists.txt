cmake_minimum_required(VERSION 3.3)
project(AS_CW)
#/home/user/jsamp701/Desktop
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/user/jsamp701/Desktop)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /home/user/jsamp701/Desktop)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/user/jsamp701/Desktop)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY out)
add_library(cwrapper MODULE source/as_lib_wrapper.c)

# code taken from
# http://stackoverflow.com/questions/4928341/having-cmake-put-generated-binaries-in-a-specific-directory-structure-with-asset
#
# SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HelloCMake_SOURCE_DIR}/assets)

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY out)

#set(SOURCE_FILES main.cpp)
#add_executable(AS_CW ${SOURCE_FILES})