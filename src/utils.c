#define STR_IMPLEMENTATION
#define FS_IMPLEMENTATION
#include "extern/str.h"
#include "extern/fs.h"
#include "extern/logging.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int is_executable_file(const char* path)
{
    struct stat st;

    if (stat(path, &st) != 0) return 0;
    if (!S_ISREG(st.st_mode)) return 0;

    return access(path, X_OK) == 0;
}

/*
 * Look `name` up in $PATH, like the `which` builtin. Returns a malloc'd
 * path on success (caller frees), or NULL when the file cannot be found
 * or executed. Names containing '/' are checked directly instead.
 */
char* find_executable(const char* name)
{
    if (name == NULL || name[0] == '\0') return NULL;

    if (strchr(name, '/') != NULL) {
        return is_executable_file(name) ? str_format("%s", name) : NULL;
    }

    const char* path = getenv("PATH");
    if (path == NULL) return NULL;

    const char* entry = path;
    for (;;) {
        const char* colon = strchr(entry, ':');
        size_t len = (colon != NULL) ? (size_t)(colon - entry) : strlen(entry);

        /* An empty PATH entry means the current directory */
        const char* dir = (len == 0) ? "." : entry;
        size_t dir_len = (len == 0) ? 1 : len;

        char* candidate = str_format("%.*s/%s", (int)dir_len, dir, name);
        if (candidate != NULL) {
            if (is_executable_file(candidate)) return candidate;
            free(candidate);
        }

        if (colon == NULL) break;
        entry = colon + 1;
    }

    return NULL;
}

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

// NOTE: candidate for include/extern/str.h
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

int input_finished(void)
{
    return feof(stdin) || ferror(stdin);
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
            if (input_finished()) {
                /* A closed stdin must not spin here forever */
                putchar('\n');
                WARN("No input available, assuming 'n'");
                return 0;
            }
            ERRO("Error reading input.");
            clear_input_buffer();
            continue;
        }

        if (choice[0] == 'y' || choice[0] == 'n') {
            break;
        } else {
            ERRO("'%s' is not a valid option. Please enter 'y' or 'n'.", choice);
            clear_input_buffer();
        }
    }

    return choice[0] == 'y';
}

char* truncate_for_display(const char* str, size_t max_width)
{
    if (str == NULL) {
        return strdup("");
    }

    size_t len = strlen(str);
    if (len <= max_width) {
        return strdup(str);
    }

    size_t cut = max_width > 3 ? max_width - 3 : 0;
    char* out = malloc(max_width + 1);
    if (out == NULL) {
        return NULL;
    }

    memcpy(out, str, cut);
    out[cut] = '\0';
    if (max_width > 3) {
        strcat(out, "...");
    }

    return out;
}

char* sql_quote(const char* input)
{
    if (input == NULL) {
        char* empty = strdup("''");
        return empty;
    }

    size_t len = strlen(input);
    size_t extra = 0;
    for (size_t i = 0; i < len; ++i) {
        if (input[i] == '\'') {
            extra++;
        }
    }

    char* out = malloc(len + extra + 3);
    if (out == NULL) {
        return NULL;
    }

    size_t j = 0;
    out[j++] = '\'';
    for (size_t i = 0; i < len; ++i) {
        if (input[i] == '\'') {
            out[j++] = '\'';
        }
        out[j++] = input[i];
    }
    out[j++] = '\'';
    out[j] = '\0';

    return out;
}

/*
 * Build a WHERE condition that targets a single entry.
 * Both values are passed through sql_quote() so messages containing
 * quotes (e.g. "don't") work and nothing can escape the literal.
 * Returns NULL on allocation failure; caller must free().
 */
char* entry_condition(const char* message, const char* date)
{
    char* message_q = sql_quote(message);
    char* date_q = sql_quote(date);

    if (message_q == NULL || date_q == NULL) {
        free(message_q);
        free(date_q);
        return NULL;
    }

    static const char prefix[] = "message = ";
    static const char middle[] = " AND date = ";

    size_t len = sizeof(prefix) - 1
               + strlen(message_q)
               + sizeof(middle) - 1
               + strlen(date_q)
               + 1;

    char* condition = (char*)malloc(len);
    if (condition != NULL) {
        snprintf(condition, len, "%s%s%s%s", prefix, message_q, middle, date_q);
    }

    free(message_q);
    free(date_q);
    return condition;
}

/*
 * Wrap `input` in single quotes so the shell treats it as a single
 * literal argument, escaping any embedded single quotes. Returns a
 * freshly allocated string; caller must free().
 */
char* shell_quote(const char* input)
{
    if (input == NULL) {
        return strdup("''");
    }

    size_t len = strlen(input);
    size_t extra = 0;
    for (size_t i = 0; i < len; ++i) {
        if (input[i] == '\'') {
            extra += 3; // ' becomes '\''
        }
    }

    char* out = (char*)malloc(len + extra + 3);
    if (out == NULL) {
        return NULL;
    }

    size_t j = 0;
    out[j++] = '\'';
    for (size_t i = 0; i < len; ++i) {
        if (input[i] == '\'') {
            out[j++] = '\'';
            out[j++] = '\\';
            out[j++] = '\'';
            out[j++] = '\'';
        } else {
            out[j++] = input[i];
        }
    }
    out[j++] = '\'';
    out[j] = '\0';

    return out;
}

/*
 * Run `command` through the shell, capture its stdout and return its
 * exit status (or -1 if it could not be run). The status is the real
 * one: it comes from pclose(), not from a second shell running
 * `echo $?`.
 *
 * If `output` is non-NULL the captured stdout is stored there (caller
 * frees); otherwise it is discarded. The command's stderr is left
 * attached to ours, so the user still sees it live.
 */
int execute_command_capture(const char* command, char** output)
{
    if (output != NULL) {
        *output = NULL;
    }

    if (command == NULL) {
        return -1;
    }

    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        return -1;
    }

    char buffer[256];
    char* result = NULL;
    size_t result_size = 0;

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t buffer_len = strlen(buffer);
        char* grown = (char*)realloc(result, result_size + buffer_len + 1);
        if (grown == NULL) {
            free(result);
            pclose(pipe);
            return -1;
        }
        result = grown;
        memcpy(result + result_size, buffer, buffer_len + 1);
        result_size += buffer_len;
    }

    int status = pclose(pipe);

    if (output != NULL) {
        *output = result;
    } else {
        free(result);
    }

    if (status == -1) {
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return -1; // Killed by a signal
}
