#ifndef IO_FS_H
#define IO_FS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define FS_VERSION_MAJOR 0
#define FS_VERSION_MINOR 0
#define FS_VERSION_PATCH 1
#define FS_VERSION "0.0.1"


#ifndef FSAPI
    #define FSAPI extern
#endif // FSAPI

/* Returns 1 if `filename` can be opened for reading, 0 otherwise. */
FSAPI int fs_file_exists(const char* filename);

/* Returns 1 if `path` exists and is a directory, 0 otherwise. */
FSAPI int fs_directory_exists(const char* path);

/* Returns 1 on success (or if it already exists), 0 on failure. */
FSAPI int fs_create_directory(const char* path);

/* Write `data` to `filename` using `mode` ("w", "w+", "wb", "a", "ab", "a+"). */
FSAPI void fs_write_file(const char* filename, const char* data, const char* mode);

/* Read a whole file into a heap string. Returns NULL on failure. */
FSAPI char* fs_read_file(const char* filename, const char* mode);

#ifdef FS_IMPLEMENTATION

FSAPI int fs_file_exists(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}

FSAPI int fs_directory_exists(const char* path)
{
    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        return 0;
    }

    return S_ISDIR(statbuf.st_mode);
}

FSAPI int fs_create_directory(const char* path)
{
#ifdef _WIN32
    int rc = _mkdir(path);
#else
    int rc = mkdir(path, 0755); // 0755 is the permission mode
#endif

    if (rc == 0) {
        return 1;
    }

    if (errno == EEXIST && fs_directory_exists(path)) {
        return 1;
    }

    perror("Error creating directory");
    return 0;
}

FSAPI void fs_write_file(const char* filename, const char* data, const char* mode)
{
    if (
        strcmp(mode, "w") &&
        strcmp(mode, "w+") &&
        strcmp(mode, "wb") &&
        strcmp(mode, "a") &&
        strcmp(mode, "ab") &&
        strcmp(mode, "a+")
    ) {
        fprintf(stderr, "[PANIC] Writing file using invalid mode: %s\n", mode);
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }
    if (fputs(data, file) == EOF) {
        perror("Error writing to file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

FSAPI char* fs_read_file(const char* filename, const char* mode)
{
    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file for reading");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        perror("Error reading file");
        fclose(file);
        return NULL;
    }

    char* buffer = (char*)malloc((size_t)file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, (size_t)file_size, file);
    if (bytes_read != (size_t)file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

#endif // FS_IMPLEMENTATION

#endif // IO_FS_H
