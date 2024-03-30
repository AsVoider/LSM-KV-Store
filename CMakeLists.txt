cmake_minimum_required(VERSION 3.5)

project(LsmServer)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_subdirectory(src)

add_executable(main main.cpp src/mem_pool.cpp)