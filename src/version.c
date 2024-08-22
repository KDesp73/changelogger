#include "version.h"
#include "extern/clib.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int compare_versions(Version a, Version b) 
{
    if(STREQ(a.full, "0.0.0") && STREQ(a.full, b.full)) return 0;
    if(STREQ(a.full, "0.0.0")) return -1;
    if(STREQ(b.full, "0.0.0")) return 1;

    if (a.major != b.major) {
        return b.major - a.major;
    }

    if (a.minor != b.minor) {
        return b.minor - a.minor;
    }

    if (a.patch != b.patch) {
        return b.patch - a.patch;
    }

    return strcmp(a.full, b.full);
}

void make_version(Version* v) 
{
    v->full = (char*)malloc(20 * sizeof(char)); // Adjust size as needed
    if (v->full == NULL) {
        perror("Failed to allocate memory for version string");
        exit(EXIT_FAILURE);
    }

    snprintf(v->full, 20, "%zu.%zu.%zu", v->major, v->minor, v->patch);
}

void parse_version(Version* version) 
{
    char* token;
    char* rest = version->full;

    token = strtok_r(rest, ".", &rest);
    version->major = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    version->minor = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    version->patch = (token != NULL) ? atoi(token) : 0;

    make_version(version);
}

void free_version(Version* v)
{
    free(v->full);
}

int is_version_set(Version v)
{
    return (
        v.full != NULL ||
        v.major != 0 ||
        v.minor != 0 ||
        v.patch != 0
    );
}

int is_valid_version(const char *version) 
{
    if(version == NULL) return 0;

    char *version_copy = strdup(version); 
    if (version_copy == NULL) {
        perror("Failed to allocate memory");
        return 0; 
    }

    char *token = strtok(version_copy, ".");
    int component_count = 0;

    while (token != NULL) {
        // Check if the token is a valid number
        for (int i = 0; token[i] != '\0'; i++) {
            if (!isdigit(token[i])) {
                free(version_copy);
                return 0; // Invalid character found
            }
        }

        // Convert the token to an integer
        int value = atoi(token);
        if (value < 0) {
            free(version_copy);
            return 0; // Negative value is invalid
        }

        component_count++;
        token = strtok(NULL, ".");
    }

    free(version_copy);

    // A valid version should have exactly three components
    return component_count == 3;
}
