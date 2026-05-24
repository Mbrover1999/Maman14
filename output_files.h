#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include "assembler_table.h"
#include "mem_img.h"

void write_object_file(const char *filename,
                       AssemblerTable *table,
                       MemoryImage *code_image,
                       MemoryImage *data_image);
void write_entries_file(const char *filename, AssemblerTable *table);
void write_externals_file(const char *filename, MemoryImage *code_image);

#endif