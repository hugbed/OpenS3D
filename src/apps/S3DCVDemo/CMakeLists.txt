cmake_minimum_required(VERSION 3.2)

project(S3DCVDemo)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${GSL_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_CameraAlignment ${PROJECT_SOURCE_DIR}/src/camera_alignment_demo.cpp)
target_link_libraries(${PROJECT_NAME}_CameraAlignment s3dcv ${OpenCV_LIBS} gsl) # pthread is linux dependant
