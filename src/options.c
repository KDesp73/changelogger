#include "options.h"
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
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

int always_export_set(Options options)
{
    char* option = clib_format_text("-%c", ABBR_ALWAYS_EXPORT);
    for(size_t i = 0; i < options.argc; ++i){
        if(
            STREQ(options.argv[i], "--always-export") ||
            STREQ(options.argv[i], option)
        ) return true;
    }
    free(option);
    return false;
}

