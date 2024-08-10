#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
typedef struct {
    char* version;
    size_t version_major;
    size_t version_minor;
    size_t version_patch;
} Version;

Version make_version(size_t major, size_t minor, size_t patch);
Version parse_version(char* version);
void free_version(Version* v);
int is_valid_version(const char *version);

#endif // VERSION_H
