cmake_minimum_required(VERSION 3.2)

project(s3d)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# ffmpeg 3.X
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE REQUIRED)

include_directories(include ${EIGEN_INCLUDE_DIRS} ${FFMPEG_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES} src/disparity/viewer_depth_converter.cpp include/s3d/disparity/viewer_depth_converter.h test/disparity/tests_viewer_depth_converter.cpp)
target_link_libraries(${PROJECT_NAME} gsl decklink_sdk ${FFMPEG_LIBRARIES} ${SWSCALE_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(test)

