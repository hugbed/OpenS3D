cmake_minimum_required(VERSION 3.2)

project(FFmpegDecoding)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(LINK_LIBS s3d_ffmpeg s3d)
if(UNIX AND NOT APPLE)
  set(LINK_LIBS ${LINK_LIBS} pthread)
endif()

# build target
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/demo_decoding.cpp)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

add_executable(${PROJECT_NAME}_VideoFileClient ${PROJECT_SOURCE_DIR}/src/video_file_client.cpp)
target_link_libraries(${PROJECT_NAME}_VideoFileClient ${LINK_LIBS})
