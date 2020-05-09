cmake_minimum_required(VERSION 3.15)
project(aviahack)

set(CMAKE_CXX_STANDARD 17)

add_executable(aviahack main.cpp json.cpp HangarManager.cpp)