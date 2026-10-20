#ifndef IO_STR_H
#define IO_STR_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STR_VERSION_MAJOR 0
#define STR_VERSION_MINOR 0
#define STR_VERSION_PATCH 1
#define STR_VERSION "0.0.1"


#ifndef STRAPI
    #define STRAPI extern
#endif // STRAPI

#define STREQ(x, y) (strcmp(x, y) == 0)

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)

/*
 * Allocate a string from a printf-style format. Returns NULL only if the
 * allocation fails. Caller must free().
 */
STRAPI char* str_format(const char* format, ...);

/* Allocate an empty, NUL-terminated heap string. Caller must free(). */
STRAPI char* str_buffer_init(void);

/* Append `text` to the heap string at *buffer, growing it as needed. */
STRAPI void str_append(char** buffer, const char* text);

/* Same as str_append, but also appends a newline. */
STRAPI void str_append_ln(char** buffer, const char* text);

/* Release *buffer and reset it to NULL. */
STRAPI void str_clean(char** buffer);

#ifdef STR_IMPLEMENTATION

STRAPI char* str_buffer_init(void)
{
    char* buffer = (char*)malloc(1);
    if (buffer != NULL) {
        buffer[0] = '\0';
    }

    return buffer;
}

STRAPI void str_append(char** buffer, const char* text)
{
    if (buffer == NULL || *buffer == NULL || text == NULL) {
        fprintf(stderr, "str_append: invalid argument\n");
        exit(EXIT_FAILURE);
    }

    size_t current_size = strlen(*buffer);
    size_t text_len = strlen(text);

    char* grown = (char*)realloc(*buffer, current_size + text_len + 1);
    if (grown == NULL) {
        fprintf(stderr, "str_append: failed to reallocate memory\n");
        exit(EXIT_FAILURE);
    }
    *buffer = grown;

    memcpy(*buffer + current_size, text, text_len + 1);
}

STRAPI void str_append_ln(char** buffer, const char* text)
{
    if (text == NULL) {
        fprintf(stderr, "str_append_ln: invalid argument\n");
        exit(EXIT_FAILURE);
    }

    char* with_newline = str_format("%s\n", text);
    if (with_newline == NULL) {
        fprintf(stderr, "str_append_ln: failed to format text\n");
        exit(EXIT_FAILURE);
    }

    str_append(buffer, with_newline);
    free(with_newline);
}

STRAPI void str_clean(char** buffer)
{
    if (buffer == NULL || *buffer == NULL) {
        return;
    }

    free(*buffer);
    *buffer = NULL;
}

STRAPI char* str_format(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (size < 0) {
        return NULL;
    }

    char* formatted_string = (char*)malloc((size_t)size + 1); // +1 for the null terminator
    if (formatted_string == NULL) {
        return NULL;
    }

    va_start(args, format);
    vsnprintf(formatted_string, (size_t)size + 1, format, args);
    va_end(args);

    return formatted_string;
}

#endif // STR_IMPLEMENTATION

#endif // IO_STR_H
