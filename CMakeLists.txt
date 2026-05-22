cmake_minimum_required(VERSION 4.2)
project(mm14 C)

set(CMAKE_C_STANDARD 11)

add_executable(mm14 pre_assembly.c
        macro_helper.c
        macro_helper.h
        first_pass.c
        assembler_table.c
        assembler_table.h
        opcode_list.c
        parser.c
        first_pass.h)
