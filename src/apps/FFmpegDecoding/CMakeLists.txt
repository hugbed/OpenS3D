cmake_minimum_required(VERSION 3.2)

project(FFmpegDecoding)

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL REQUIRED) # missing AVDEVICE

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if(FFMPEG_FOUND)
  include_directories(${FFMPEG_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "FFMPEG not found")
endif()

# get all *.cpp files from src to build target
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*${SRC_EXT_PATTERN})
source_group("Source Files" FILES ${SRC_FILES})


# build target
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})
