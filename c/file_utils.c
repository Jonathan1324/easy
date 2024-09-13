#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int count = 0;

    const char* str = content;
    
    // Loop through the string until the null terminator is reached
    while (*str != '\0') {
        if (*str == '\n') {
            count++;
        }
        str++;
    }

    content[strlen(content) - count] = '\0';

    return content;
}