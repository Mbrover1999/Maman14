#include <stdio.h>
#include <stdlib.h>

#include "../headers/parser.h"
#include "../headers/first_pass.h"
#include "../headers/mem_img.h"
#include "../headers/assembler_table.h"
#include "../headers/opcode_list.h"

/*Return addressing mode, by default 0*/
int get_addressing_type(int mode) {
    if (mode == ADDR_IMMEDIATE) return 0;
    if (mode == ADDR_DIRECT) return 1;
    if (mode == ADDR_RELATIVE) return 2;
    if (mode == ADDR_REGISTER) return 3;
    return 0;
}

/*Return addressing mode, by default 0*/
int encode_first_pass(const ParsedLine *parsed) {
    const CommandInfo *cmd = find_command(parsed->command);

    int word = 0;
    int src_mode = 0;
    int dst_mode = 0;

    if (parsed->operand_count == 2) {
        src_mode = get_addressing_mode(parsed->operands[0]);
        dst_mode = get_addressing_mode(parsed->operands[1]);
    } else if (parsed->operand_count == 1) {
        dst_mode = get_addressing_mode(parsed->operands[0]);
    }

    /*Push the opcode to bits 8 - 11*/
    word |= (cmd->opcode << 8);
    if (cmd->funct != -1) {
        /*Push the funct to bits 4 - 7*/
        word |= (cmd->funct << 4);
    }else {
        /*Push the 0000 to bits 4 - 7*/
        word |= 0 << 4;
    }
    /*Push the src_mode (get addressing type beforehand) to bits 2 - 3*/
    word |= (get_addressing_type(src_mode) << 2);
    /*Push the dst_mode (get addressing type beforehand) to bits 0 - 1*/
    word |= get_addressing_type(dst_mode);

    return word;
}

/* Update the data symbols at the end of the first pass*/
void update_data_symbols(AssemblerTable *table, int ICF) {
    int i = 0;
    for (; i < table->count; i++) {
        if (table->symbols[i].attributes == ATTR_DATA) {
            table->symbols[i].value += ICF;
        }
    }
}

/*Main function in first pass, start the pass and checks, if we face an error,
  raise the error_flag, keep going even after an error*/
void first_pass(FILE *in, AssemblerTable *table,MacroList *macros,
    MemoryImage *code_image, MemoryImage *data_image, int *error_flag) {
    char line[256]; /*Buffer*/
    ParsedLine parsed; /*This will hold the parsed line after syntax checks*/
    int line_num = 0; /*Current line counter*/
    int DC = 0;
    int IC = 100;
    int L = 0;
        code_image->count = 0; /*This will hold our code commands information*/
    data_image->count = 0; /*This will hold our data commands information*/

    /*Start passing through the file*/
    while (fgets(line, sizeof(line), in) != NULL) {
        line_num++;

        /*If for any reason the parser failed to pass*/
        if (!parse_line(line, &parsed, macros)) {
            printf("Parser error in line num: %d\n", line_num);
            *error_flag = 1; /*Update error flag*/
            continue;
        }

        /*Ignore error lines or comment lines*/
        if (parsed.type == LINE_COMMENT || parsed.type == LINE_ERROR) {
            continue;
        }

        if (parsed.type == LINE_DIRECTIVE) {
            DirectiveType directive = is_valid_directive(parsed.command);
            if (directive == DIR_DATA || directive == DIR_STRING) {
                if (parsed.has_label) {
                    if (!add_symbol(table,parsed.label,DC,ATTR_DATA)) {
                        printf("Duplicated symbol in line: %d", line_num);
                        *error_flag = 1;
                   };
                }
                if (directive == DIR_STRING) {

                    char *str = parsed.operands[0];

                    int i = 1;
                    /*Start adding the string value char by char to the data image*/
                    for (;str[i] != '"'; i++) {

                        add_word(data_image, str[i],DC,'A');

                        DC++;
                    }
                    /*Add null terminator*/
                    add_word(data_image,0,DC,'A');

                    DC++;

                }else {

                    int i = 0;
                    /*Start adding the operands value to the data image*/
                    for (; i < parsed.operand_count; i++) {

                        /*Convert string of operands to long int, then cast to int*/
                        /*strtol will notify in case of a failure*/
                        int num = (int) strtol(parsed.operands[i], NULL, 10);
                        add_word(data_image, num, DC, 'A');
                        DC++;
                    }

                }
                continue;
            }
            if (directive == DIR_EXTERN) {

                if (!add_symbol(table,parsed.operands[0],0,
                    ATTR_EXTERN)) {
                    printf("Duplicated symbol in line: %d", line_num);
                    *error_flag = 1;
                }

                continue;
            }
            if (directive == DIR_ENTRY) {
                continue;
            }


        }

        if (parsed.type == LINE_INSTRUCTION) {
            int i = 0;
            int first_word;
            int temp_IC; /*Hold a temp instruction count to help us in our for loop*/

            /*Check if label is valid and if exists*/
            if (parsed.has_label) {

                if (!add_symbol(table,parsed.label,IC,
                   ATTR_CODE)) {
                    printf("Duplicated symbol in line: %d", line_num);
                    *error_flag = 1;
                   }
            }

            first_word = encode_first_pass(&parsed);

            /*Add the label*/
            add_word(code_image, first_word, IC, 'A');

            temp_IC = IC;
            temp_IC++;

            for (; i < parsed.operand_count; i++) {
                /*Insert operands with the stock value '?'*/
                add_word(code_image, 0, temp_IC, '?');
                temp_IC++;
            }

            L = 1+ parsed.operand_count;
            IC = IC + L;
        }
    }
    table->ICF = IC;
    table->DCF = DC;
    /*Add to the data symbols in our assembler table the updated ICF amount*/
    update_data_symbols(table, table->ICF);


}

