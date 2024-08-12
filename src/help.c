#include "help.h"
#include "config.h"
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include <stdio.h>

#define PTN(format, ...) \
    do { \
        fprintf(stdout, format, ##__VA_ARGS__); \
        fprintf(stdout, "\n"); \
    } while(0)

// With 2 space indent
#define PTNI(format, ...) \
    do { \
        PTN("  "format, ##__VA_ARGS__); \
    } while(0);


void help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("changelogger [-h | -v] %sTODO%s", COLOR_BG(1), RESET); // TODO: update
    PTN("");

    PTN("%sCOMMANDS%s", BOLD, RESET);
    PTNI("init                          Initializes the changelog");
    PTNI("list [<options>]              Lists all entries");
    PTNI("add <msg> [<options>]         Adds an entry");
    PTNI("set <options>                 Sets a variable");     
    PTNI("get <options>                 Returns a variable");
    PTNI("delete <options>              Deletes one or more entries");
    PTNI("release <options>             Creates a new release");
    PTNI("export [<options>]            Exports the CHANGELOG.md file")

    PTN("");

    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                     Prints this message");
    PTNI("-v --version                  Prints the project version");

    PTN("");
    PTN("Made by KDesp73");
    PTN("Check out https://keepachangelog.com/en/1.0.0/ for more information on changelogs");
}

void add_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                     Prints this message");
    PTNI("-s --status <status>          Specify the status of the entry (default is 'added')");
    PTNI("            STATUS_ADDED = 1 or 'added'");
    PTNI("            STATUS_CHANGED = 2 or 'changed'");
    PTNI("            STATUS_REMOVED = 3 or 'removed'");
    PTNI("            STATUS_FIXED = 4 or 'fixed'"); 
    PTNI("            STATUS_DEPRECATED = 5 or 'deprecated'");
    PTNI("            STATUS_SECURITY = 6 or 'security'");
}

void set_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s set <option> <value>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sVARIABLES%s", BOLD, RESET);
    PTNI("-h --help                          Prints this message");
    PTNI("--version-major <major>            Specify the value of the major version");
    PTNI("--version-minor <minor>            Specify the value of the minor version");
    PTNI("--version-patch <patch>            Specify the value of the patch version");
    PTNI("--version-full <full-version>      Specify the value of the full version");
    PTNI("--config-dir <directory>           Specify the configuration file path");
    PTNI("--always-export <value>            Specify whether to export the %s file", CHANGELOG_FILE); 
    PTNI("                                   after every command (WARNING: Always means ALWAYS)");
    PTNI("      Where always_export is set to 1 for: 1, true, TRUE, True, yes, y, YES, Yes");
    PTNI("                          is set to 0 for every other value (even NULL)");
}

void list_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                          Prints this message");
}

void delete_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                          Prints this message");
}

void export_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                          Prints this message");
}

void release_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s release --new {major | minor | patch}", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
    PTNI("-N --new              Creates a new release");
}

