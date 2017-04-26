cmake_minimum_required(VERSION 3.2)

project(s3d_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# tests
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
set(TEST_LIBRARIES gtest gtest_main gmock ${CMAKE_THREAD_LIBS_INIT} s3d)

# create executable and add test
# test name for: "path/to/tests_a.cpp"
# will be : "path_to_tests_a"
function(add_gtest filepath)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" "" test_name ${filepath})
  string(REPLACE "\/" "_" test_name ${test_name})
  string(REPLACE ".cpp" "" test_name ${test_name})
  add_executable(${test_name} ${filepath})
  target_link_libraries(${test_name} ${TEST_LIBRARIES})
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# find all tests and add them
file(GLOB_RECURSE SRC_FILES *.cpp)
foreach(file_name ${SRC_FILES})
  add_gtest(${file_name})
endforeach()
