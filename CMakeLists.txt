cmake_minimum_required(VERSION 3.13)
project(Harp)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
set(CMAKE_C_FLAGS_DEBUG "-ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_INSTALL_RPATH "/usr/local/bin")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/usr/local/lib")

include_directories(.)
include_directories(include/)

link_libraries(X11 dl pthread m)

add_executable(Harp.exe
	tests.c
	lexer.c
        main.c)
