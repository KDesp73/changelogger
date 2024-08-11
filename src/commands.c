#include "config.h"
#include "help.h"
#include "options.h"
#include "sqlite.h"
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "commands.h"
#include <sys/stat.h>

// TODO: add to clib.h
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755) // 0755 is the permission mode
#endif

int createDirectory(const char *path) {
    if (MKDIR(path) == 0) {
        return 1;
    } else {
        if (errno == EEXIST) {
            printf("Directory already exists: %s\n", path);
        } else {
            perror("Error creating directory");
        }
        return 0; 
    }
}

// TODO: add to clib.h
int directoryExists(const char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        return 0;
    }

    return S_ISDIR(statbuf.st_mode);
}

void command_init()
{
    if(!clib_file_exists(CHANGELOG_FILE)) {
        clib_write_file(CHANGELOG_FILE, "# CHANGELOG\n", "w");
    } else INFO("%s is located in this directory", CHANGELOG_FILE);
    if(!directoryExists(CHANGELOG_DIR)) {
        createDirectory(CHANGELOG_DIR);
    } else INFO("%s/ is located in this directory", CHANGELOG_DIR);
    if(!clib_file_exists(SQLITE_DB)) {
        create_database(SQLITE_DB);
    } else INFO("%s is already created", SQLITE_DB);
}

Command get_command(char* command)
{
#define COMPARE_AND_RETURN_COMMAND(c) \
    else if(STREQ(command, command_to_string(c))) return c;

    if(command == NULL) return COMMAND_UNSET;
    COMPARE_AND_RETURN_COMMAND(COMMAND_ADD)
    COMPARE_AND_RETURN_COMMAND(COMMAND_INIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_SET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_LIST)
    COMPARE_AND_RETURN_COMMAND(COMMAND_DELETE)
    else return COMMAND_UNKNOWN;

#undef COMPARE_AND_RETURN_COMMAND
}

void execute_command(Options options)
{
    switch (options.command) {
        case COMMAND_INIT:
            command_init();
            return;
        case COMMAND_UNKNOWN:
            ERRO("Unknown command: %s", options.argv[1]);
            return;
        case COMMAND_UNSET:
            ERRO("Command not found");
            break;
        case COMMAND_ADD:
        case COMMAND_LIST:
        case COMMAND_DELETE:
        case COMMAND_SET:
            PANIC("Not implemented yet.");
    }

    help();
}


char* command_to_string(Command command)
{
    switch (command) {
    case COMMAND_UNSET:
    case COMMAND_UNKNOWN:
        return "";
    case COMMAND_INIT:
        return "init";
    case COMMAND_ADD:
        return "add";
    case COMMAND_LIST:
        return "list";
    case COMMAND_SET:
        return "set";
    case COMMAND_DELETE:
        return "delete";
    }

    return "";
}
