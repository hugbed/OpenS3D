cmake_minimum_required(VERSION 3.2)

project(FFmpegDecoding)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# build target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/demo_decoding.cpp)
target_link_libraries(${PROJECT_NAME} s3d_ffmpeg s3d pthread)

add_executable(${PROJECT_NAME}_VideoFileClient ${PROJECT_SOURCE_DIR}/src/video_file_client.cpp)
target_link_libraries(${PROJECT_NAME}_VideoFileClient s3d_ffmpeg s3d_cv pthread) # pthread is linux only