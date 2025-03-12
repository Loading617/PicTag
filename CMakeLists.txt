cmake_minimum_required(VERSION 3.10)
project(PicTag)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(EXIV2 REQUIRED exiv2)

add_executable(picture-tag-editor src/main.cpp src/gui.cpp src/metadata.cpp)

target_include_directories(picture-tag-editor PRIVATE include ${GTK_INCLUDE_DIRS} ${EXIV2_INCLUDE_DIRS})
target_link_libraries(picture-tag-editor ${GTK_LIBRARIES} ${EXIV2_LIBRARIES})
