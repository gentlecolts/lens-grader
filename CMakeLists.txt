if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "DO NOT BUILD in-tree.")
endif()

project(LensGrader)
set(CMAKE_CXX_STANDARD 14)

cmake_minimum_required (VERSION 3.0)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PUBLIC include)
add_subdirectory(tests)