cmake_minimum_required(VERSION 3.5)
project(decklink_sdk)

find_package(DecklinkAPI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

add_library(${PROJECT_NAME}
        src/DeckLinkAPIDispatch.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_link_libraries(${PROJECT_NAME} ${DECKLINKAPI_LIBRARY})