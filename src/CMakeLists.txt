project(LensGrader)

file(GLOB ${PROJECT_NAME}_SOURCES *.cpp)
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})

file(GLOB ${PROJECT_NAME}_HEADERS *.h *.hpp)
target_sources(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_HEADERS})
