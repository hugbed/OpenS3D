cmake_minimum_required(VERSION 2.8.7)

project(gsl CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)