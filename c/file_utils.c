#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file(const char *filename) {
    FILE *file;
    char *buffer;
    long file_size;

    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Couldn't open file!\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char*)malloc((file_size + 1) * sizeof(char));
    if (buffer == NULL) {
        printf("ERROR!\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, sizeof(char), file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

    return buffer;
}