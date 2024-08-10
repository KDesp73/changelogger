#include "help.h"
#include <stdio.h>

#define PTN(format, ...) \
    do { \
        fprintf(stdout, format, ##__VA_ARGS__); \
        fprintf(stdout, "\n"); \
    } while(0)

void help(char* usage, char* footer)
{
    PTN("Usage: %s", usage);
    PTN("");

    PTN("Commands: ");
    PTN("init                                   Initializes the changelog");
    PTN("list                                   Lists all entries");
    PTN("add <msg>                              Adds an entry");
    PTN("set <options>                          Sets a variable");     
    PTN("");
    PTN("Options: ");
    PTN("-h --help                              Prints this message");
    PTN("-v --version                           Prints the project version");
    PTN("-s --status <status>                   Specify the status of the entry");
    PTN("[set] --version-major <major>          Specify the value of the major version (used only by `set`)");
    PTN("[set] --version-minor <minor>          Specify the value of the minor version (used only by `set`)");
    PTN("[set] --version-patch <patch>          Specify the value of the patch version (used only by `set`)");
    PTN("[set] --config-dir <directory>         Specify the configuration file path (used only by `set`)"); // Not implemented

    PTN("");
    PTN("%s", footer);
}
