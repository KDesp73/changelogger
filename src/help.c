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
    } while(0)


void help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("changelogger <command> <options> <value>");
    PTN("");

    PTN("%sCOMMANDS%s", BOLD, RESET);
    PTNI("init                          Initializes the changelog");
    PTNI("add <msg> [<options>]         Adds an entry");
    PTNI("list [<options>]              Lists all entries");
    PTNI("edit <options>                Edit any of the unreleased entries");
    PTNI("delete <options>              Delete any of the unreleased entries");
    PTNI("set <options>                 Sets a variable");     
    PTNI("get <options>                 Returns a variable");
    PTNI("release <options>             Creates a new release");
    PTNI("push <options>                Push an unpushed release to Github");
    PTNI("export [<options>]            Exports the CHANGELOG.md file");
    PTNI("generate <value>              Generate various files");
    PTNI("import <options>              Import CHANGELOG.md file into the database");

    PTN("");

    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                     Prints this message");
    PTNI("-v --version                  Prints the project version");

    PTN("");
    PTN("Made by KDesp73");
    PTN("Check out https://keepachangelog.com/en/1.1.0/ for more information on changelogs");
}

void add_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                     Prints this message");
    PTNI("-C --commits                  Add unreleased commit messages taken from the `git shortlog` command");
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
    PTNI("--config-dir <directory>           Specify the configuration file path");
    PTNI("--remote-repo <url>                Specify the url of the remote repo");
    PTNI("--editor <editor>                  Specify the editor to use");
    PTNI("--always-push <value>              Specify whether to immediately push the release");
    PTNI("--always-export <value>            Specify whether to export the %s file", CHANGELOG_FILE); 
    PTNI("                                   after 'add', 'delete' and 'edit'");
    PTNI("      Where always_export and always_push are set to 1 for: 1, true, TRUE, True, yes, y, YES, Yes");
    PTNI("                                          are set to 0 for every other value");
}

void list_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s list <option> <value>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                     Prints this message");
    PTNI("-V --version-full <value>     Set the version to filter the list");
    PTNI("-s --status <value>           Set the status to filter the list");
    PTNI("-r --releases                 List only the releases");
}

void delete_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s delete <options>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
    PTNI("-A --all              List all entries and have them available for deletion");
}

void get_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s get <variable>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sVARIABLES%s", BOLD, RESET);
    PTNI("version               Latest release");
    PTNI("export                Always export CHANGELOG.md");
    PTNI("remote                The remote git repo url");
    PTNI("config                The path to the configuration file");
    PTNI("push                  Always push the release on Github");
    PTNI("editor                Editor to use when editing a file is needed");
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
}

void export_help()
{
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                                             Prints this message");
    PTNI("-F --format {md | markdown | json | yml | yaml }      Specify in which format to export");
}

void push_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s push -V <version>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
    PTNI("-V --version-full     Specify the version of the release you want to push");
    PTNI("-y --yes              Skip the confirmation message");
}

void release_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s release --new {major | minor | patch} [-p]", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help                         Prints this message");
    PTNI("-N --new {major | minor | patch}  Creates a new release");
    PTNI("-p --push                         Push the release on github");
    PTNI("-y --yes                          Skip the confirmation message");
    PTNI("-Y --yank <version>               Set a release as YANKED");
    PTNI("-U --unyank <version>             Set a release as not YANKED");
}

void edit_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s edit <options>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
    PTNI("-s --status           Specify new status");
    PTNI("-t --title            Specify new title/message");
    PTNI("-V --version-full     Specify new version");
    PTNI("-A --all              List all entries and have them available for editing");
}

void generate_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s generate <value>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sVALUES%s", BOLD, RESET);
    PTNI("config                A starting point for your config file (~/.changelogger.yml)");
    PTNI("autocomplete          Autocomplete for the active shell");
    PTNI("man                   Man page for %s", EXECUTABLE_NAME);
}

void import_help()
{
    PTN("%sUSAGE%s", BOLD, RESET);
    PTNI("%s import <options>", EXECUTABLE_NAME);
    PTN("");
    PTN("%sOPTIONS%s", BOLD, RESET);
    PTNI("-h --help             Prints this message");
    PTNI("-f --file <path>      File to import");
}
