cmake_minimum_required(VERSION 3.26)
project(aoc-2023 C)

set(CMAKE_C_STANDARD 90)

add_executable(day1 day1.c input.c strtools.c timer.c)
add_executable(day2 day2.c input.c strtools.c timer.c)
add_executable(day3 day3.c input.c strtools.c timer.c)
#add_executable(parser parser.c)