cmake_minimum_required(VERSION 3.17)
project(ICML_IMA)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(ICML_IMA main.cpp generate_data.h read_data.h twingreedy.h fastsgs.h multirandom.h repeatedgreedy.h)