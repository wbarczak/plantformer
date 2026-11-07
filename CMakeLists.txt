cmake_minimum_required(VERSION 3.23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(platformer)

add_executable(game)
target_sources(game
PRIVATE
    src/main.cpp
)

add_subdirectory(vendor/raylib)

target_link_libraries(game
PRIVATE
    raylib
)

target_compile_features(game PRIVATE cxx_std_20)

add_compile_definitions(game PRIVATE ASSETS="${PROJECT_SOURCE_DIR}/assets/")
#add_compile_definitions(game PRIVATE ASSETS="/assets/")
add_compile_definitions(game PRIVATE LEVELS="${PROJECT_SOURCE_DIR}/levels/")
#add_compile_definitions(game PRIVATE LEVELS="/levels/")