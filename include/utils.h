#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
int is_number(const char *str);
void append_line(const char* file, const char* line, size_t index);
int is_blank(const char *str);
char* char_repeat(char c, size_t count);
void clear_input_buffer();

#endif // UTILS_H
