project(LensGrader)
set(CMAKE_CXX_STANDARD 14)

include(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#find_library(SDL2_LIBRARY NAME SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB ${PROJECT_NAME}_SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

file(GLOB ${PROJECT_NAME}_HEADERS *.h *.hpp)
target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})