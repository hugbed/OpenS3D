cmake_minimum_required(VERSION 3.5)

project(s3d)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(include)

file(GLOB_RECURSE SRC_FILES
    include/*.h
    src/*.cpp)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_add_clang_tidy(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/include/" "${SRC_FILES}")

# ${OpenCV_INCLUDE_DIRS};/usr/include/c++/5.4.0"