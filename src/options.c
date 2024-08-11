#include "options.h"
#include "commands.h"
#include "status.h"
#include <stdio.h>


void log_options(Options options)
{
    printf("Status: %s\n", status_to_string(options.status));
    printf("Command: %s\n", command_to_string(options.command));
    if(options.version.version != NULL)
        printf("Version: %s\n", options.version.version);
    printf("Version Major: %zu\n", options.version.version_major);
    printf("Version Minor: %zu\n", options.version.version_minor);
    printf("Version Patch: %zu\n", options.version.version_patch);
}
