cmake_minimum_required(VERSION 3.14..3.21 FATAL_ERROR)
# must before the project
set(CMAKE_C_COMPILER clang)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# add_compile_options(-Weverything -Werror -pedantic)
add_compile_options(-Weverything)

project(leptjson_test LANGUAGES C)
add_library(leptjson leptjson.c)
add_executable(leptjson_test test.c)
target_link_libraries(leptjson_test leptjson)

# if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
# endif()

