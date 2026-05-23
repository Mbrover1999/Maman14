#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include <stdio.h>

#include "assembler_table.h"
#include "macro_helper.h"
#include "mem_img.h"


void second_pass(FILE *in,AssemblerTable *table,MacroList *macros,
    MemoryImage *code_image,int *error_flag);

#endif
