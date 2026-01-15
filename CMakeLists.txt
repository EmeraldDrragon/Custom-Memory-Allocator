cmake_minimum_required(VERSION 3.19)
project(Custom_Malloc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(custom_malloc
    source/main.cpp
    source/customAlloc.cpp
    source/customAlloc.h
    source/utils.cpp)

target_link_libraries(custom_malloc PRIVATE Threads::Threads)