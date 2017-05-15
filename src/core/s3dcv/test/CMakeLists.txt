cmake_minimum_required(VERSION 3.2)

project(s3dcv_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# automagically create tests
set(TEST_ADDITIONAL_LIBRARIES s3dcv)
include(${CMAKE_SOURCE_DIR}/cmake/gtest-cxx-dev-tools.cmake)
