#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/assembler_table.h"
#include "../headers/macro_helper.h"
#include "../headers/mem_img.h"
#include "../headers/first_pass.h"
#include "../headers/output_files.h"
#include "../headers/second_pass.h"


void print_binary_word(FILE *out, int value) {
    int i;

    for (i = 11; i >= 0; i--) {
        fputc((value & (1 << i)) ? '1' : '0', out);
    }
}

void print_debug_binary(MemoryImage *code_image,
                        MemoryImage *data_image,
                        AssemblerTable *table) {
    int i;

    printf("CODE:\n");

    for (i = 0; i < code_image->count; i++) {
        printf("%04d ", code_image->words[i].address);
        print_binary_word(stdout, code_image->words[i].value);
        printf("\n");
    }

    printf("DATA:\n");

    for (i = 0; i < data_image->count; i++) {
        printf("%04d ", table->ICF + data_image->words[i].address);
        print_binary_word(stdout, data_image->words[i].value);
        printf("\n");
    }
}



int main(void) {
    FILE *file = fopen("../input/example.txt", "r");
    FILE *out = fopen("../results/answer.txt", "w");

    if (file == NULL || out == NULL) {
        printf("No file found exiting program!");
        return 1;
    }
    MacroList macros;
    AssemblerTable table;
    MemoryImage code_image;
    MemoryImage data_image;

    init_macro_list(&macros);
    init_assembler_table(&table);

    expand_macros(file, out, &macros);
    int error_flag = 0;
    fclose(out);

    out = fopen("../results/answer.txt", "r");
    if (out == NULL) {
        printf("Cannot open answer.txt for reading\n");
        return 1;
    }

    first_pass(out, &table,&macros,
    &code_image, &data_image, &error_flag);
    print_assembler_table(&table);
    rewind(out);
    second_pass(out, &table, &macros, &code_image, &error_flag);

    if (!error_flag) {
        write_object_file("../results/lol.ob",
                          &table,
                          &code_image,
                          &data_image);
        write_entries_file("../results/example.ent", &table);
        write_externals_file("../results/example.ext", &code_image);

    }


    print_debug_binary(&code_image, &data_image, &table);
    free_macro_list(&macros);
    fclose(file);
    fclose(out);
    return 0;
}

