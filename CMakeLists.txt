cmake_minimum_required(VERSION 3.0)
project(integrate_serial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

add_executable(integrate_serial main.cpp integrals.h)
