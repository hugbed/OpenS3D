#
# automatically create tests in current directory
# with files : tests_*.cpp
# link with gtest, gmock and TEST_ADDITIONAL_LIBRARIES
#
# Usage:
# include(${CMAKE_SOURCE_DIR}/cmake/gtest-cxx-dev-tools.cmake)
#

# tests
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
set(TEST_LIBRARIES gtest gtest_main gmock ${CMAKE_THREAD_LIBS_INIT} ${TEST_ADDITIONAL_LIBRARIES})

# create executable and add test
# test name for: "path/to/tests_a.cpp"
# will be : "path_to_tests_a"
function(add_gtest filepath)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" "" test_name ${filepath})
  string(REPLACE "\/" "_" test_name ${test_name})
string(REPLACE ".cpp" "" test_name ${test_name})
add_executable(test_${test_name} ${filepath})
target_link_libraries(test_${test_name} ${TEST_LIBRARIES})
add_test(NAME ${test_name} COMMAND test_${test_name})
endfunction()

# find all tests and add them
file(GLOB_RECURSE SRC_FILES tests_*.cpp)
foreach(file_name ${SRC_FILES})
add_gtest(${file_name})
endforeach()
