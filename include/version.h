#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
typedef struct {
    char* full;
    size_t major;
    size_t minor;
    size_t patch;
} Version;

void make_version(Version* v);
void parse_version(Version* version);
void free_version(Version* v);
int is_valid_version(const char *version);
int is_version_set(Version v);

#endif // VERSION_H
