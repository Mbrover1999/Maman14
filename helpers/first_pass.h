#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "../assembler_table.h"
#include "mem_img.h"

void first_pass(FILE *in, AssemblerTable *table,MacroList *macros,
                MemoryImage *code_image, MemoryImage *data_image, int *error_flag);


#endif
