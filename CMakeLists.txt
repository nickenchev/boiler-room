cmake_minimum_required(VERSION 3.5)
project(boiler-room CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

find_package(SDL2 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_subdirectory(lib/boiler)
add_subdirectory(lib/game)

add_executable(boiler-room-sdl
	src/main_game.cpp)

target_include_directories(boiler-room-sdl PRIVATE game boiler)
target_link_libraries(boiler-room-sdl game boiler SDL2::SDL2main SDL2::SDL2)

qt_add_executable(boiler-room
	src/main.cpp
	src/ui/mainwindow.ui
	src/ui/mainwindow.h
	src/ui/mainwindow.cpp
	src/ui/openglwindow.h
	src/ui/openglwindow.cpp
)

set_target_properties(boiler-room PROPERTIES
	WIN32_EXECUTABLE,
	MACOSX_BUNDLE
)

target_include_directories(boiler-room PRIVATE game boiler)
target_link_libraries(boiler-room PRIVATE game boiler Qt6::Widgets)
