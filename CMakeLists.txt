# https://discourse.gnome.org/t/help-with-a-cmakelists-txt-for-gtk-4/7397

cmake_minimum_required(VERSION 3.25.1)

project(mandelbrot-viewer)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES})

