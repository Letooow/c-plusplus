cmake_minimum_required(VERSION 3.26)
project(ct_cpp24_backlog_lw_containers_Letooow)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(ct_cpp24_backlog_lw_containers_Letooow
        bucket_storage.hpp
        my_list.hpp
        helpers.hpp
        tests.cpp
        helpers.h)

add_subdirectory(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(ct_cpp24_backlog_lw_containers_Letooow gtest gtest_main)