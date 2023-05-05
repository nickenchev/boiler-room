cmake_minimum_required(VERSION 3.2)
project(boiler-room CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

find_package(SDL2 REQUIRED)

add_subdirectory(lib/boiler)
add_subdirectory(lib/game)

add_executable(boiler-room
	src/main_game.cpp)

target_include_directories(boiler-room PRIVATE game boiler)
target_link_libraries(boiler-room game boiler SDL2::SDL2main SDL2::SDL2)
