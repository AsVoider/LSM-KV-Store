cmake_minimum_required(VERSION 3.5)

project(LsmServer)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

# add_subdirectory()

add_executable(main main.cpp)