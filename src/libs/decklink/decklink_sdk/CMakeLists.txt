cmake_minimum_required(VERSION 3.5)
project(decklink_sdk)

include_directories(include)

add_library(${PROJECT_NAME}
	 src/DeckLinkAPIDispatch.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)