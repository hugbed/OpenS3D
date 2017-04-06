cmake_minimum_required(VERSION 3.5)
project(decklink_interface)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(
	include
)

file(GLOB_RECURSE SRC_FILES
		src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} s3d decklink_sdk)