#include "extern/clib.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_number(const char *str) 
{
    if (*str == '\0') {
        return 0;
    }

    if (*str == '-') {
        str++;
    }

    int has_digits = 0;
    int has_decimal = 0;

    while (*str) {
        if (isdigit(*str)) {
            has_digits = 1;
        } else if (*str == '.') {
            if (has_decimal) {
                return 0;
            }
            has_decimal = 1;
        } else {
            return 0;
        }
        str++;
    }

    return has_digits; 
}

void append_line(const char* file, const char* line, size_t index)
{
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Failed to open file for reading");
        return;
    }

    // Read existing lines into a dynamic array
    char **lines = NULL;
    size_t line_count = 0;
    size_t capacity = 10;

    lines = malloc(capacity * sizeof(char*));
    if (lines == NULL) {
        perror("Failed to allocate memory");
        fclose(fp);
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = 0;

        // Resize if necessary
        if (line_count >= capacity) {
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char*));
            if (lines == NULL) {
                perror("Failed to reallocate memory");
                fclose(fp);
                return;
            }
        }

        // Allocate memory for the line and copy it
        lines[line_count] = strdup(buffer);
        if (lines[line_count] == NULL) {
            perror("Failed to duplicate string");
            fclose(fp);
            return;
        }
        line_count++;
    }
    fclose(fp);

    // Insert the new line at the specified index
    if (index > line_count) {
        index = line_count; // Append to the end if index is out of bounds
    }

    // Resize the array to accommodate the new line
    lines = realloc(lines, (line_count + 1) * sizeof(char*));
    if (lines == NULL) {
        perror("Failed to reallocate memory");
        return;
    }

    // Shift lines to make space for the new line
    for (size_t i = line_count; i > index; i--) {
        lines[i] = lines[i - 1];
    }

    // Insert the new line
    lines[index] = strdup(line);
    if (lines[index] == NULL) {
        perror("Failed to duplicate string");
        return;
    }
    line_count++;

    // Write the updated lines back to the file
    fp = fopen(file, "w");
    if (fp == NULL) {
        perror("Failed to open file for writing");
        return;
    }

    for (size_t i = 0; i < line_count; i++) {
        fprintf(fp, "%s\n", lines[i]);
        free(lines[i]); // Free each line after writing
    }

    free(lines); // Free the array of lines
    fclose(fp);
}

// TODO: add to clib.h
int is_blank(const char *str) 
{
    if (str == NULL) {
        return 1; // NULL is considered blank
    }

    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0; // Found a non-whitespace character
        }
        str++; 
    }

    return 1; 
}
char* char_repeat(const char* c, size_t count)
{
    size_t char_len = strlen(c); // Length of the UTF-8 character
    size_t total_len = char_len * count;

    // Allocate memory for the repeated characters and null terminator
    char* result = (char*)malloc(total_len + 1);
    if (result == NULL) {
        return NULL;
    }

    // Fill the result with the repeated character
    for (size_t i = 0; i < count; i++) {
        memcpy(result + i * char_len, c, char_len);
    }

    // Null-terminate the string
    result[total_len] = '\0';

    return result;
}

void clear_input_buffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Discard characters until newline or EOF
}

int is_true(const char* str)
{
    return (
        STREQ(str, "1") ||
        STREQ(str, "true") ||
        STREQ(str, "TRUE") ||
        STREQ(str, "True") ||
        STREQ(str, "yes") ||
        STREQ(str, "y") ||
        STREQ(str, "YES") ||
        STREQ(str, "Yes")
    );
}


int yes_or_no(const char* message)
{
    char choice[2];
    while (1) {
        printf("%s [y/n]: ", message);
        if (scanf("%1s", choice) != 1) {
            ERRO("Error reading input.\n");
            clear_input_buffer();
            continue;
        }

        if (choice[0] == 'y' || choice[0] == 'n') {
            break;
        } else {
            ERRO("'%s' is not a valid option. Please enter 'y' or 'n'.\n", choice);
            clear_input_buffer();
        }
    }

    return choice[0] == 'y';
}
