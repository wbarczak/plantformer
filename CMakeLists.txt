cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Plantformer)

add_executable(plantformer)
target_sources(plantformer
PRIVATE
    src/main.cpp
    src/screens/game.cpp
)

add_subdirectory(vendor/raylib)

target_link_libraries(plantformer
PRIVATE
    raylib
)

target_compile_features(plantformer PRIVATE cxx_std_20)
target_compile_options(plantformer PRIVATE -Wall -Wpedantic -Werror)

add_compile_definitions(plantformer PRIVATE ASSETS="${PROJECT_SOURCE_DIR}/assets/")
#add_compile_definitions(plantformer PRIVATE ASSETS="/assets/")
add_compile_definitions(plantformer PRIVATE LEVELS="${PROJECT_SOURCE_DIR}/levels/")
#add_compile_definitions(plantformer PRIVATE LEVELS="/levels/")

#set(CMAKE_BUILD_TYPE "debug")