#include "help.h"
#include "clib.h"
#include <stdio.h>

#define PTN(format, ...) \
    do { \
        fprintf(stdout, format, ##__VA_ARGS__); \
        fprintf(stdout, "\n"); \
    } while(0)

void help()
{
    PTN("Usage: changelogger [-h | -v]"); // TODO: update
    PTN("");

    PTN("%sCommands: %s", BOLD, RESET);
    PTN("init                                   Initializes the changelog");
    PTN("list                                   Lists all entries");
    PTN("add <msg> [<options>]                  Adds an entry");
    PTN("set <options>                          Sets a variable");     
    PTN("");
    PTN("%sOptions: %s", BOLD, RESET);
    PTN("-h --help                              Prints this message");
    PTN("-V                                     Prints the project version");

    PTN("");
    PTN("Made by KDesp73");
}

void add_help()
{
    PTN("%sAdditional Options: %s", BOLD, RESET);
    PTN("-h --help                  Prints this message");
    PTN("-s --status <status>       Specify the status of the entry");
    PTN("            STATUS_CHANGED = 0 or 'changed'");
    PTN("            STATUS_ADDED = 1 or 'added'");
    PTN("            STATUS_DEPRECATED = 2 or 'deprecated'");
    PTN("            STATUS_REMOVED = 3 or 'removed'");
    PTN("            STATUS_FIXED = 4 or 'fixed'"); 
    PTN("            STATUS_SECURITY = 5 or 'security'");
}

void set_help()
{
    PTN("%sVariables to set: %s", BOLD, RESET);
    PTN("-h --help                  Prints this message");
    PTN("--version-major <major>    Specify the value of the major version");
    PTN("--version-minor <minor>    Specify the value of the minor version");
    PTN("--version-patch <patch>    Specify the value of the patch version");
    PTN("--version <full-version>   Specify the value of the full version");
    PTN("--config-dir <directory>   Specify the configuration file path"); // Not implemented
}
