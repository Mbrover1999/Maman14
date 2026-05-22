#include "mem_img.h"

void add_word(MemoryImage *image,
              int value,
              int address,
              char ARE) {

    image->words[image->count].value = value;
    image->words[image->count].address = address;
    image->words[image->count].ARE = ARE;

    image->count++;
}