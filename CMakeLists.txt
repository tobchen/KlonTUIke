cmake_minimum_required(VERSION 2.8)

include(CheckIncludeFiles)

project(KlonTUIke)

check_include_files(ncursesw/ncurses.h KTUI_UNICODE)

set(CMAKE_C_FLAGS "-g -Wall -Wextra -ansi -pedantic")

add_executable(ktui src/main.c src/visual.c src/lv_inter.c src/logical.c)
if (KTUI_UNICODE)
    target_link_libraries(ktui ncursesw)
    target_compile_definitions(ktui PUBLIC "KTUI_UNICODE")
else()
    target_link_libraries(ktui ncurses)
endif()
