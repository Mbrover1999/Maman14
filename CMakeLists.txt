cmake_minimum_required(VERSION 4.2)
project(mm14 C)

set(CMAKE_C_STANDARD 11)

add_executable(mm14
        pre_assembly.c
        macro_helper.c
        first_pass.c
        assembler_table.c
        opcode_list.c
        parser.c
        mem_img.c
        second_pass.c
)
