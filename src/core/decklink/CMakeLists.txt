cmake_minimum_required(VERSION 3.2)

project(s3d_decklink)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB_RECURSE SRC_FILES
        include/*.h
        src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} decklink_sdk s3d)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(test)
