cmake_minimum_required(VERSION 4.2)
project(mm14 C)

set(CMAKE_C_STANDARD 11)

add_executable(mm14
        src/pre_assembly.c
        src/macro_helper.c
        src/first_pass.c
        src/assembler_table.c
        src/opcode_list.c
        src/parser.c
        src/mem_img.c
        src/second_pass.c
        src/output_files.c
        headers/pre_assembly.h
)
