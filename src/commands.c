#include "config.h"
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

// TODO: add to clib.h
#define STREQ(x, y) (strcmp(x, y) == 0) 

Command get_command(char* command)
{
    if(STREQ(command, "init")) return COMMAND_INIT;
    else if(STREQ(command, "add")) return COMMAND_ADD;
    else return COMMAND_UNKNOWN;
}

void execute_command(int argc, char** argv)
{
    char* c = argv[argc-1];
    Command command = get_command(c);
    switch (command) {
    case COMMAND_INIT:
        command_init();
        break;
    case COMMAND_UNKNOWN:
        ERRO("Unknown command: %s", c);
        break;
    case COMMAND_ADD:
    case COMMAND_LIST:
    case COMMAND_DELETE:
    case COMMAND_GIT_POPULATE:
      break;
    case COMMAND_SET:
      break;
    }
}
