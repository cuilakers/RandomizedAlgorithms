cmake_minimum_required(VERSION 3.17)
project(ICML_REV)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


add_executable(ICML_REV main.cpp generate_random_data.h utility.h twingreedy.h fastsgs.h multirandom.h adaptive.h repeatedgreedy.h)