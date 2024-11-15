#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
int is_number(const char *str);
void append_line(const char* file, const char* line, size_t index);
int is_blank(const char *str);
char* char_repeat(const char* c, size_t count);
void clear_input_buffer();

int is_true(const char* str);
int yes_or_no(const char* message);

#define freec(x) free((char*) x)

#endif // UTILS_H
