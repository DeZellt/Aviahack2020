cmake_minimum_required(VERSION 3.15)
project(aviahack)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

add_executable(aviahack main.cpp json.cpp HangarManager.cpp)