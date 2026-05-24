#ifndef MEM_IMG_H
#define MEM_IMG_H

#define MAX_IMAGE_SIZE 4096
#include "parser.h"

typedef struct {
    int value;
    int address;
    char ARE;
    char symbol_name[MAX_LABEL_LEN];
} MemoryWord;

typedef struct {
    MemoryWord words[MAX_IMAGE_SIZE];
    int count;
} MemoryImage;

void add_word(MemoryImage *image,int value,int address,char ARE);
MemoryWord *find_by_address(MemoryImage *image, int address);

#endif