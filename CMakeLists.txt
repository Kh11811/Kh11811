cmake_minimum_required(VERSION 3.10)
project(UserService)

set(CMAKE_CXX_STANDARD 17)

add_executable(UserService main.cpp)

# Add Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(UserServiceTest test.cpp)
target_link_libraries(UserServiceTest gtest_main)
