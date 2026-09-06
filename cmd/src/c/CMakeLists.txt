cmake_minimum_required(VERSION 3.10)
project(cmd)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(cmd cmd.c)

install(TARGETS cmd RUNTIME DESTINATION bin)
