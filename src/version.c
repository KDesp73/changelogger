#include "version.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void make_version(Version* v) 
{
    v->version = (char*)malloc(20 * sizeof(char)); // Adjust size as needed
    if (v->version == NULL) {
        perror("Failed to allocate memory for version string");
        exit(EXIT_FAILURE);
    }

    snprintf(v->version, 20, "%zu.%zu.%zu", v->version_major, v->version_minor, v->version_patch);
}

void parse_version(Version* version) 
{
    char* token;
    char* rest = version->version;

    token = strtok_r(rest, ".", &rest);
    version->version_major = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    version->version_minor = (token != NULL) ? atoi(token) : 0;

    token = strtok_r(rest, ".", &rest);
    version->version_patch = (token != NULL) ? atoi(token) : 0;
}

void free_version(Version* v)
{
    free(v->version);
}

int is_version_set(Version v)
{
    return (
        v.version != NULL ||
        v.version_major != 0 ||
        v.version_minor != 0 ||
        v.version_patch != 0
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
