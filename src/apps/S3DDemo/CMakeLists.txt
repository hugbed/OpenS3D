cmake_minimum_required(VERSION 3.2)

project(S3DDemo)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${GSL_INCLUDE_DIR})

set(LINK_LIBS s3d)
if(UNIX AND NOT APPLE)
  set(LINK_LIBS ${LINK_LIBS} pthread)
endif()

add_executable(s3ddemo_video_conversion_pbm ${PROJECT_SOURCE_DIR}/src/video_conversion_pbm.cpp)
target_link_libraries(s3ddemo_video_conversion_pbm ${LINK_LIBS})


