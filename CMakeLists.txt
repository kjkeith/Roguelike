cmake_minimum_required(VERSION 3.13)
project(keith_kirkland.assignment-1.08)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-lncurses")

set(CURSES_USE_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(keith_kirkland.assignment-1.08 dungeon.cpp dungeon.h priority.cpp priority.h Monster.cpp Monster.h ParsedMonster.cpp ParsedMonster.h Dice.h
        MonsterParser.cpp MonsterParser.h ParsedItem.cpp ParsedItem.h ItemParser.cpp ItemParser.h Item.cpp Item.h)
target_link_libraries(keith_kirkland.assignment-1.08 ${CURSES_LIBRARIES})