#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char* paste_strings(char* str1, char* str2) {
    char* new_string = malloc(strlen(str1) + strlen(str2) + 1);
    if (new_string == NULL) {
        printf("Fatal error: malloc failed");
        exit(1);
        return NULL;
    }
    sprintf(new_string, "%s%s", str1, str2);
    return new_string;
}

char* const_to_malloc(char* const_string) {
    char* new_string = malloc(strlen(const_string) + 1);
    if (new_string == NULL) {
        printf("Fatal error: malloc failed");
        exit(EXIT_FAILURE);
        return NULL;
    }
    sprintf(new_string, "%s", const_string);
    return new_string;
}

char* read_file_into_string(char* file_path) {
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Error: %s not found!\n", file_path);
        return NULL;
    }
    char* contents = malloc(FILE_BUFFER + 1);
    if (contents == NULL) {
        printf("Fatal error: malloc failed");
        exit(EXIT_FAILURE);
        return NULL;
    }
    int read_bytes = fread(contents, 1, FILE_BUFFER, fp);
    contents[read_bytes] = '\0';
    fclose(fp);
    return contents;
}
