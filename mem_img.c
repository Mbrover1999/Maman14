#include <stdlib.h>
#include "mem_img.h"

void add_word(MemoryImage *image,
              int value,
              int address,
              char ARE) {

    image->words[image->count].value = value;
    image->words[image->count].address = address;

    image->count++;
}

MemoryWord *find_by_address(MemoryImage *image, int address) {
    int i = 0;
    for (; i < image->count; i++) {
        if (image->words[i].address == address) {
            return &image->words[i];
        }
    }
    return NULL;
}