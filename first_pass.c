#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "first_pass.h"
#include "mem_img.h"
#include "assembler_table.h"
#include "opcode_list.h"

int encode_first_word(const ParsedLine *parsed) {
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

    word |= (cmd->opcode << 11);
    word |= (src_mode << 7);
    word |= (dst_mode << 3);
    word |= 4; /* ARE = A */

    return word;
}

void update_data_symbols(AssemblerTable *table, int ICF) {
    int i = 0;
    for (; i < table->count; i++) {
        if (table->symbols[i].attributes == ATTR_DATA) {
            table->symbols[i].value += ICF;
        }
    }
}

void first_pass(FILE *in, AssemblerTable *table,MacroList *macros,
    MemoryImage *code_image, MemoryImage *data_image) {
    char line[256];
    ParsedLine parsed;
    int line_num = 0;
    int DC = 0;
    int IC = 100;
    int error_flag = 0;
    int L = 0;
    code_image->count = 0;
    data_image->count = 0;

    while (fgets(line, sizeof(line), in) != NULL) {
        line_num++;
        if (!parse_line(line, &parsed, macros)) {
            printf("Parser error in line num: %d\n", line_num);
            error_flag = 1;
            continue;
        }
        if (parsed.type == LINE_COMMENT || parsed.type == LINE_ERROR) {
            continue;
        }

        if (parsed.type == LINE_DIRECTIVE) {
            DirectiveType directive = is_valid_directive(parsed.command);
            if (directive == DIR_DATA || directive == DIR_STRING) {
                if (parsed.has_label) {
                    if (!add_symbol(table,parsed.label,DC,
                   ATTR_DATA)) {
                        printf("Duplicated symbol in line: %d", line_num);
                        error_flag = 1;
                   };
                }
                if (directive == DIR_STRING) {

                    char *str = parsed.operands[0];

                    int i = 1;

                    for (;str[i] != '"'; i++) {

                        add_word(data_image,
                                 str[i],
                                 DC,
                                 'A');

                        DC++;
                    }

                    add_word(data_image,
                             '\0',
                             DC,
                             'A');

                    DC++;

                }else {

                    int i = 0;
                    for (; i < parsed.operand_count; i++) {

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
                    error_flag = 1;
                };

                continue;
            }
            if (directive == DIR_ENTRY) {
                continue;
            }


        }

        if (parsed.type == LINE_INSTRUCTION) {
            int i = 0;
            int first_word;
            int temp_IC;

            if (parsed.has_label) {

                if (!add_symbol(table,parsed.label,IC,
                   ATTR_CODE)) {
                    printf("Duplicated symbol in line: %d", line_num);
                    error_flag = 1;
                   }
            }

            first_word = encode_first_word(&parsed);

            add_word(code_image, first_word, IC, 'A');

            temp_IC = IC;
            temp_IC++;

            for (; i < parsed.operand_count; i++) {
                add_word(code_image, 0, temp_IC, '?');
                temp_IC++;
            }

            L = 1+ parsed.operand_count;
            IC = IC + L;
        }
    }
    table->ICF = IC;
    table->DCF = DC;

    update_data_symbols(table, table->ICF);


}

