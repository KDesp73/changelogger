#include "version.h"
#include <ctype.h>
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

int is_valid_version(const char *version) 
{
    // Split the version string by the dot (.)
    char *version_copy = strdup(version); // Create a modifiable copy of the version string
    if (version_copy == NULL) {
        perror("Failed to allocate memory");
        return 0; // Memory allocation failed
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
