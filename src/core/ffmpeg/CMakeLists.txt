cmake_minimum_required(VERSION 3.2)

project(s3d_ffmpeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# ffmpeg 3.X
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE REQUIRED)

include_directories(include ${FFMPEG_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} gsl ${FFMPEG_LIBRARIES} s3d)
target_include_directories(${PROJECT_NAME} PUBLIC include ${FFMPEG_INCLUDE_DIRS})

add_subdirectory(test)
