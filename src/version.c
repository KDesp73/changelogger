#include "entry.h"
#include <stdlib.h>
#include <string.h>

Version make_version(size_t major, size_t minor, size_t patch) 
{
    Version v;
    
    v.version = (char*)malloc(20 * sizeof(char)); // Adjust size as needed
    if (v.version == NULL) {
        perror("Failed to allocate memory for version string");
        exit(EXIT_FAILURE);
    }

    snprintf(v.version, 20, "%zu.%zu.%zu", major, minor, patch);
    
    v.version_major = major;
    v.version_minor = minor;
    v.version_patch = patch;

    return v;
}

Version parse_version(char* version_str) 
{
    Version v;
    char* token;
    char* rest = version_str;

    // Allocate memory for the version string
    v.version = (char*)malloc(strlen(version_str) + 1); // +1 for null terminator
    if (v.version == NULL) {
        perror("Failed to allocate memory for version string");
        exit(EXIT_FAILURE);
    }
    strcpy(v.version, version_str);

    // Parse the version string
    token = strtok_r(rest, ".", &rest);
    v.version_major = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    v.version_minor = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    v.version_patch = (token != NULL) ? atoi(token) : 0;

    return v;
}

void free_version(Version* v)
{
    free(v->version);
}
