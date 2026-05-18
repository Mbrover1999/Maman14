cmake_minimum_required(VERSION 4.2)
project(mm14 C)

set(CMAKE_C_STANDARD 11)

add_executable(mm14 pre_assembly.c
        macro_helper.c
        macro_helper.h)
