#
# automatically create tests in current directory
# with files : tests_*.cpp
# link with gtest, gmock and TEST_ADDITIONAL_LIBRARIES
#
# Usage:
# include(${CMAKE_SOURCE_DIR}/cmake/gtest-cxx-dev-tools.cmake)
#

# tests
set(TEST_LIBRARIES Qt5::Test ${TEST_ADDITIONAL_LIBRARIES})
set(TEST_SOURCES ${TEST_ADDITIONAL_SOURCES})

# create executable and add test
# test name for: "path/to/tests_a.cpp"
# will be : "path_to_tests_a"
function(add_qttest filepath)
  string(REPLACE "${PROJECT_SOURCE_DIR}/" "" test_name ${filepath})
  string(REPLACE "\/" "_" test_name ${test_name})
string(REPLACE ".cpp" "" test_name ${test_name})
add_executable(${test_name} ${filepath} ${TEST_SOURCES})
target_link_libraries(${test_name} ${TEST_LIBRARIES})
add_test(${test_name} ${test_name})
endfunction()

# find all tests and add them
file(GLOB_RECURSE SRC_FILES tests_*.cpp)
foreach(file_name ${SRC_FILES})
add_qttest(${file_name})
endforeach()
