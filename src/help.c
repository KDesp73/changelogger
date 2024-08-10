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
    PTN("init                           Initializes the changelog");
    PTN("list                           Lists all entries");
    PTN("add <msg>                      Adds an entry");
    PTN("    <msg> -s <status>");
    PTN("");
    PTN("Options: ");
    PTN("-h --help                      Prints this message");
    PTN("-v --version                   Prints the project version");
    PTN("");
    PTN("%s", footer);
}
