#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro_helper.h"

int main(void) {
    FILE *file = fopen("../example.txt", "r");
    FILE *out = fopen("../answer.txt", "w");

    if (file == NULL || out == NULL) {
        printf("No file found exiting program!");
        return 1;
    }
    MacroList macros;
    init_macro_list(&macros);
    expand_macros(file, out, &macros);
    free_macro_list(&macros);
    fclose(file);
    fclose(out);
    return 0;
}

