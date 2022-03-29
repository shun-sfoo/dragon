cmake_minimum_required(VERSION 3.14..3.21 FATAL_ERROR)
# must before the project
set(CMAKE_C_COMPILER gcc)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(ins LANGUAGES C)
add_executable(ins ins.c)
