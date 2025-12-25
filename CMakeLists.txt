cmake_minimum_required(VERSION 3.16)

project(MonProjetQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/msvc2022_64") 

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

add_executable(MonApp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    DrawArea.cpp
    DrawArea.h
    Context.cpp
    Context.h
    FlappyBird.cpp
    FlappyBird.h
)

target_link_libraries(MonApp PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)