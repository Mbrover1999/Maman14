#include <stdlib.h>
#include "../headers/mem_img.h"

/*Add word to the memory image, data_image and code_image share this type*/
void add_word(MemoryImage *image,
              int value,
              int address,
              char ARE) {

    image->words[image->count].value = value;
    image->words[image->count].address = address;
    image->words[image->count].ARE = ARE;
    image->words[image->count].symbol_name[0] = '\0';


    image->count++;
}
/*Find and return a word by its address*/
MemoryWord *find_by_address(MemoryImage *image, int address) {
    int i = 0;
    for (; i < image->count; i++) {
        if (image->words[i].address == address) {
            return &image->words[i];
        }
    }
    return NULL;
}