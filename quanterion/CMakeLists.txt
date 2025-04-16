cmake_minimum_required(VERSION 3.26)
project(ct_c24_hw_quaternion_Letooow)

set(CMAKE_CXX_STANDARD 17)

add_executable(ct_c24_hw_quaternion_Letooow
        quat.hpp
#        Quat.cpp
        tests.cpp
#        testttt.cpp
)

add_subdirectory(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(ct_c24_hw_quaternion_Letooow gtest gtest_main)
