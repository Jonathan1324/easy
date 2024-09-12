#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0'; // Null-terminate the string
    }
    fclose(file);

    return content;
}