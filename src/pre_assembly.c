#include <stdio.h>
#include <stdlib.h>

#include "../headers/assembler_table.h"
#include "../headers/macro_helper.h"
#include "../headers/mem_img.h"
#include "../headers/first_pass.h"
#include "../headers/output_files.h"
#include "../headers/second_pass.h"

int assemble_file(const char *input_path)
{
    FILE *file = NULL;
    FILE *out = NULL;
    /*Stating our data structures*/
    MacroList macros;
    AssemblerTable table;
    MemoryImage code_image;
    MemoryImage data_image;
    int error_flag;

    printf("Starting assemble on: '%s'\n", input_path);
    file = fopen(input_path, "r");
    out = fopen("results/ps.as", "w");

    if (file == NULL || out == NULL) {
        printf("No file found exiting program!");
        return 1;
    }
    printf("Starting translation......\n");

    printf("Expanding macros......\n");

    /*Initiate our data structures*/
    init_macro_list(&macros);
    init_assembler_table(&table);

    error_flag = 0;

    /*Again not a requirement but felt wrong to not check*/
    if (!expand_macros(file, out, &macros)) {
        error_flag = 1;
    }

    fclose(out);

    out = fopen("results/ps.as", "r");

    if (out == NULL) {
        printf("Cannot open ps.as for reading\n");
        return 1;
    }

    printf("Starting first pass......\n");

    /*As stated in the printf, we start our passes here*/
    first_pass(out, &table,&macros,
    &code_image, &data_image, &error_flag);
    rewind(out);
    printf("Starting second pass......\n");
    second_pass(out, &table, &macros, &code_image, &error_flag);
    /*We don't create files if an error was found*/
    if (!error_flag) {
        write_object_file("results/ps.ob",
                          &table,
                          &code_image,
                          &data_image);
        write_entries_file("results/ps.ent", &table);
        write_externals_file("results/ps.ext", &code_image);
        printf("Done!\n");

    }else {
        printf("Fatal error, file were not created!\n");
    }
    free_macro_list(&macros);
    fclose(file);
    fclose(out);
    return 0;
}

int main(int argc, char *argv[]) {
    int i;

    if (argc < 2) {

        printf("Usage: %s <file1.as> <file2.as> ...\n", argv[0]);
        return 1;
    }

    i = 1;

    for (; i < argc; i++) {

        assemble_file(argv[i]);
    }

    return 0;
}

