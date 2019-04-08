project(LensGrader-SDLTest)

include(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 sdl2)
#find_library(SDL2_LIBRARY NAME SDL2)

if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIRS})

	add_executable(${PROJECT_NAME} main.cpp)

	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} LensGrader)
else()
	message(WARNING "${PROJECT_NAME}: Couldnt find SDL2, skipping")
endif()