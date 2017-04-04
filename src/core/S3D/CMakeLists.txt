cmake_minimum_required(VERSION 3.5)

project(S3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

include_directories(include)

add_library(${PROJECT_NAME} ${SRC_FILES} src/disparity/DisparityAlgorithmBM.cpp src/disparity/DisparityAlgorithmORB.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
