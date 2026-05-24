#include <stdio.h>
#include "helpers/output_files.h"

void write_object_file(const char *filename,
                       AssemblerTable *table,
                       MemoryImage *code_image,
                       MemoryImage *data_image) {
    FILE *out;
    int i;

    out = fopen(filename, "w");

    if (out == NULL) {
        printf("Cannot create object file\n");
        return;
    }

    fprintf(out, "%d %d\n", table->ICF - 100, table->DCF);

    for (i = 0; i < code_image->count; i++) {
        fprintf(out, "%04d %03X %c\n",
                code_image->words[i].address,
                code_image->words[i].value & 0xFFF,
                code_image->words[i].ARE);
    }

    for (i = 0; i < data_image->count; i++) {
        fprintf(out, "%04d %03X %c\n",
                table->ICF + data_image->words[i].address,
                data_image->words[i].value & 0xFFF,
                data_image->words[i].ARE);
    }

    fclose(out);
}
void write_entries_file(const char *filename, AssemblerTable *table) {
    FILE *out;
    int i;
    int has_entry = 0;

    out = fopen(filename, "w");
    if (out == NULL) {
        return;
    }

    for (i = 0; i < table->count; i++) {
        if (table->symbols[i].attributes & ATTR_ENTRY) {
            fprintf(out, "%s %04d\n",
                    table->symbols[i].name,
                    table->symbols[i].value);
            has_entry = 1;
        }
    }

    fclose(out);

    if (!has_entry) {
        remove(filename);
    }
}

void write_externals_file(const char *filename, MemoryImage *code_image) {
    FILE *out;
    int i;
    int has_extern = 0;

    out = fopen(filename, "w");
    if (out == NULL) {
        return;
    }

    for (i = 0; i < code_image->count; i++) {
        if (code_image->words[i].ARE == 'E') {
            fprintf(out, "%s %04d\n",
                    code_image->words[i].symbol_name,
                    code_image->words[i].address);
            has_extern = 1;
        }
    }

    fclose(out);

    if (!has_extern) {
        remove(filename);
    }
}