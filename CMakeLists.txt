cmake_minimum_required(VERSION 3.6)
project(OpenS3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

find_package(OpenCV REQUIRED)

set(SOURCE_FILES main.cpp hb_utils.h s3d/DisparityAlgorithm.h s3d/DisparityAlgorithmBM.h s3d/DisparityAlgorithmORB.h)
add_executable(OpenS3D ${SOURCE_FILES})
target_link_libraries(OpenS3D ${OpenCV_LIBS})