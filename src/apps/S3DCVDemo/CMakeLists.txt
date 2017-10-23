cmake_minimum_required(VERSION 3.2)

project(S3DCVDemo)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB_RECURSE INCLUDE_FILES include/*.h)

include_directories(include ${GSL_INCLUDE_DIR})

#
# automatically create demo executables in demo directory
# with files : demos/*.cpp
#
# test name for: "demos/demo_a.cpp"
# will be : "s3dcvdemo_demo_a"
#
function(add_demo filepath)
    string(REPLACE "${PROJECT_SOURCE_DIR}/src/demos/" "" test_name ${filepath})
    string(REPLACE "\/" "_" test_name ${test_name})
    string(REPLACE ".cpp" "" test_name ${test_name})

    add_executable(s3dcvdemo_${test_name} ${filepath} ${INCLUDE_FILES})
    target_link_libraries(s3dcvdemo_${test_name} s3dcv ${OpenCV_LIBS} gsl)
endfunction()

message(${PROJECT_SOURCE_DIR})

# find all demos and add them
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/demos/*.cpp)
foreach(file_name ${SRC_FILES})
    add_demo(${file_name})
endforeach()
