# CMakeList.txt : CMake project for szachyCMake, include source and define
# project specific logic here.
#

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and specify it as a C project
project(szachyCMake C)

# Define the source files
set(SOURCES
    "source/main.c"
    "source/application.c"
    "source/board.c"
    "source/chessPieces.c"
    "source/input.c"
    "source/interface.c"
    "source/window.c"
    "header/application.h"
    "header/board.h"
    "header/chessPieces.h"
    "header/input.h"
    "header/interface.h"
    "header/window.h"
    "header/game.h"
    "source/game.c"     
    "bot.c"
    "header/magicBitboard.h"
    "source/magicBitboard.c"
    "magic_numbers.h"
  "header/linkedList.h")

# Conditionally exclude files for Linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(REMOVE_ITEM SOURCES "source/window.c" "header/window.h")
    list(APPEND SOURCES "source/linuxTerminal.c" "header/linuxTerminal.h")
    set(CMAKE_C_COMPILER "gcc")
endif()

# Set the C compiler to GCC

# Add source to this project's executable.
add_executable(szachyCMake ${SOURCES})

# Set the C standard to C99
set_property(TARGET szachyCMake PROPERTY C_STANDARD 99)

# TODO: Add tests and install targets if needed.
