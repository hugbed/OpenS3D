cmake_minimum_required(VERSION 3.2)

project(S3DCudaDemo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_executable(${PROJECT_NAME}_Simple src/simple_cuda.cpp)
target_link_libraries(${PROJECT_NAME}_Simple s3dcuda)

add_executable(${PROJECT_NAME}_Disparity ${PROJECT_SOURCE_DIR}/src/disparity_demo.cpp)
target_link_libraries(${PROJECT_NAME}_Disparity ${OpenCV_LIBS} gsl s3dcuda)