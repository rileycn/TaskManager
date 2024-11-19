#ifndef MISC_H
#define MISC_H

#define FILE_BUFFER (1024)

char* paste_strings(char* str1, char* str2);
char* const_to_malloc(char* const_string);
char* read_file_into_string(char* file_path);

#endif
