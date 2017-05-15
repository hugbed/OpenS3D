cmake_minimum_required(VERSION 3.2)

project(S3DDemo)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${GSL_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_VideoFileClient ${PROJECT_SOURCE_DIR}/src/video_file_client.cpp)
target_link_libraries(${PROJECT_NAME}_VideoFileClient s3d pthread) # pthread is linux dependant

add_executable(${PROJECT_NAME}_VideoConversionPBM ${PROJECT_SOURCE_DIR}/src/video_conversion_pbm.cpp)
target_link_libraries(${PROJECT_NAME}_VideoConversionPBM s3d pthread) # pthread is linux dependant

add_executable(${PROJECT_NAME}_CameraAlignment ${PROJECT_SOURCE_DIR}/src/camera_alignment_demo.cpp)
target_link_libraries(${PROJECT_NAME}_CameraAlignment s3d ${OpenCV_LIBS} gsl) # pthread is linux dependant

