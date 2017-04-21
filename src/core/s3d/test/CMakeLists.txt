cmake_minimum_required(VERSION 3.5)

project(s3d_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# tests
find_package(GTest REQUIRED)
include_directories(GTEST_INCLUDE_DIRS)
set(env{CTEST_OUTPUT_ON_FAILURE} true)
set(env{GTEST_COLOR} true)

set(TEST_LIBRARIES ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(tests_video_capture tests_video_capture_device.cpp)
target_link_libraries(tests_video_capture ${TEST_LIBRARIES})

add_test(NAME tests_video_capture COMMAND tests_video_capture)
