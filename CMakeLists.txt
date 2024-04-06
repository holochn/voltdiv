cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME "voltdiv")
project(${PROJECT_NAME} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/main.cpp)