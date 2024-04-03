cmake_minimum_required(VERSION 3.5)

project(LsmServer)

set(CMAKE_CXX_STANDARD 23)

include_directories(include utils)

aux_source_directory(src SRC_LIST)

add_subdirectory(src)

add_executable(main main.cpp ${SRC_LIST})