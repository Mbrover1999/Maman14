#include <stdlib.h>
#include <string.h>

char *str_dup(const char *s) {
    char *copy = malloc(strlen(s) + 1);
    if (copy != NULL) {
        strcpy(copy, s);
    }
    return copy;
}

void trim(char *str) {

    char *start = str;
    char *end = NULL;

    /*skip spaces at start*/
    while (*start == ' ' || *start == '\t' || *start == '\n') {
        start++;
    }

    memmove(str, start, strlen(start) + 1);

    if (*str == '\0') {
        return;
    }


    /*find end*/
    end = str + strlen(str) - 1;

    /*remove spaces at end*/
    while (end >= str &&
          (*end == ' ' || *end == '\t' || *end == '\n')) {

        *end = '\0';
        end--;
          }
}
