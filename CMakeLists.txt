cmake_minimum_required(VERSION 3.10)

project(thread_pool_test CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "C++ Compiler not found")
endif()

if (NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU))
    message(FATAL_ERROR "Not supported C++ Compiler: " ${CMAKE_CXX_COMPILER_ID})
endif()

find_package(Threads REQUIRED)

include_directories(include)
aux_source_directory(test src)
add_executable(thread_pool_test ${src})
target_link_libraries(thread_pool_test PRIVATE gtest pthread)