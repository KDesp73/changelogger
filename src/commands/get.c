#include "commands.h"
#include "database.h"

void command_get(Options options)
{
    char* key = options.argv[options.argc-1];
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);

    if(
        !options.all &&
        !STREQ(key, "version") &&
        !STREQ(key, "config") &&
        !STREQ(key, "remote") &&
        !STREQ(key, "push") &&
        !STREQ(key, "editor") &&
        !STREQ(key, "export")
    ) { 
        PANIC("Invalid key: '%s'. Try %s get -h", key, EXECUTABLE_NAME);
    }

    if(options.all || STREQ(key, "version")){
        char* version = select_version_full(db);
        if(options.all) printf("Version: ");
        printf("%s\n", version);
        free(version);
    }
    if(options.all || STREQ(key, "config")) {
        char* config_path = select_config_path(db);
        if(options.all) printf("Config: ");
        char* default_path = CHANGELOGGER_DEFAULT_CONFIG_PATH;
        printf("%s\n", (config_path == NULL) ? default_path : config_path);
        free(default_path);
        free(config_path);
    } 
    if(options.all || STREQ(key, "remote")) {
        char* remote = SELECT_CONFIG_REMOTE;
        if(options.all) printf("Remote: ");
        printf("%s\n", (remote == NULL) ? "" : remote);
        free(remote);
    } 
    if(options.all || STREQ(key, "push")) {
        int push = SELECT_CONFIG_PUSH;
        if(options.all) printf("Always push: ");
        printf("%d\n", push);
    } 
    if(options.all || STREQ(key, "export")) {
        int export = SELECT_CONFIG_EXPORT;
        if(options.all) printf("Always export: ");
        printf("%d\n", export);
    } 
    if(options.all || STREQ(key, "editor")) {
        char* editor = SELECT_CONFIG_EDITOR;
        if(options.all) printf("Editor: ");
        printf("%s\n", editor);
        free(editor);
    }

    sqlite3_close(db);
}
