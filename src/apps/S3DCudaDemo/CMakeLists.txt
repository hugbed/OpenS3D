cmake_minimum_required(VERSION 3.5)

project(S3DCudaDemo)

add_executable(${PROJECT_NAME}_Simple src/simple_cuda.cpp)
target_link_libraries(${PROJECT_NAME}_Simple s3dcuda)
