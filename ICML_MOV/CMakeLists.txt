cmake_minimum_required(VERSION 3.17)
project(ICML_MOV)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


add_executable(ICML_MOV main.cpp utility_functions.h multirandom.h twingreedy.h fastsgs.h repeatedgreedy.h)
