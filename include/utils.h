#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
int is_number(const char *str);
void append_line(const char* file, const char* line, size_t index);
int is_blank(const char *str);
char* char_repeat(const char* c, size_t count);
void clear_input_buffer();
/* True when stdin reached EOF or failed, so prompts can stop instead of looping */
int input_finished(void);

int is_true(const char* str);
int yes_or_no(const char* message);
char* truncate_for_display(const char* str, size_t max_width);
char* sql_quote(const char* input);
char* entry_condition(const char* message, const char* date);
char* shell_quote(const char* input);
int execute_command_capture(const char* command, char** output);
char* find_executable(const char* name);

#define freec(x) free((char*) x)

#endif // UTILS_H
