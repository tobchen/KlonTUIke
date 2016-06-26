cmake_minimum_required (VERSION 2.8.8)

project (KlonTUIke)
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

set(CMAKE_C_FLAGS "-g -Wall -Wextra -ansi -pedantic")

add_library(common OBJECT src/logical.c src/lv_inter.c)

add_executable(ktui src/main.c src/visual.c $<TARGET_OBJECTS:common>)
target_link_libraries(ktui ncurses)

add_executable(ktui_uni EXCLUDE_FROM_ALL src/main.c src/visual.c $<TARGET_OBJECTS:common>)
target_link_libraries(ktui_uni ncursesw)
target_compile_definitions(ktui_uni PUBLIC "KLONTUIKE_UNICODE")