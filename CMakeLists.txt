# cmake_minimum_required(VERSION <specify CMake version here>)
cmake_minimum_required(VERSION 3.0)
project(Socket_Programming)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_REQUIRED_FLAGS -D_GNU_SOURCE gnuXX)
#set(CMAKE_REQUIRED_FLAGS -std=gnuXX)
set(CMAKE_REQUIRED_FLAGS -D_POSIX_C_SOURCE=200112L)
set(CMAKE_CXX_FLAGS -pthread)
add_definitions(-D_GNU_SOURCE)


add_executable(Socket_Programming Server.cpp Server.h Client.cpp Client.h client_main.cpp)