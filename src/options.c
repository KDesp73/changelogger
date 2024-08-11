#include "options.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "status.h"
#include <stdio.h>


void log_options(Options options)
{
    printf("Status: %s\n", status_to_string(options.status));
    if(options.version.full != NULL)
        printf("Version: %s\n", options.version.full);
    printf("Version Major: %zu\n", options.version.major);
    printf("Version Minor: %zu\n", options.version.minor);
    printf("Version Patch: %zu\n", options.version.patch);
}

int version_full_set(Options options)
{
    char* option = clib_format_text("-%c", ABBR_VERSION_FULL);
    for(size_t i = 0; i < options.argc; ++i){
        if(
            STREQ(options.argv[i], "--version-full") ||
            STREQ(options.argv[i], option)
        ) return true;
    }
    free(option);
    return false;
}

int version_major_set(Options options)
{
    char* option = clib_format_text("-%c", ABBR_VERSION_MAJOR);
    for(size_t i = 0; i < options.argc; ++i){
        if(
            STREQ(options.argv[i], "--version-major") ||
            STREQ(options.argv[i], option)
        ) return true;
    }
    free(option);
    return false;
}

int version_minor_set(Options options)
{
    char* option = clib_format_text("-%c", ABBR_VERSION_MINOR);
    for(size_t i = 0; i < options.argc; ++i){
        if(
            STREQ(options.argv[i], "--version-minor") ||
            STREQ(options.argv[i], option)
        ) return true;
    }
    free(option);
    return false;
}

int version_patch_set(Options options)
{
    char* option = clib_format_text("-%c", ABBR_VERSION_PATCH);
    for(size_t i = 0; i < options.argc; ++i){
        if(
            STREQ(options.argv[i], "--version-patch") ||
            STREQ(options.argv[i], option)
        ) return true;
    }
    free(option);
    return false;
}
