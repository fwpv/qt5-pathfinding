cmake_minimum_required(VERSION 3.5)

project(qt5_pathfinding LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(qt5_pathfinding
    field.cpp
    field.h
    fieldscene.cpp
    fieldscene.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(qt5_pathfinding PRIVATE Qt5::Widgets)
