cmake_minimum_required(VERSION 3.14..3.21 FATAL_ERROR)

# must before the project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(soundex LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(soundex SoundexTest.cc)

target_link_libraries(
  soundex
  gtest_main
  gmock_main
)

include(GoogleTest)
gtest_discover_tests(soundex)
