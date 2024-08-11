#include "config.h"
#include "database.h"
#include "date.h"
#include "help.h"
#include "options.h"
#include "sqlite.h"
#include "status.h"
#include "version.h"
#include <ctype.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"
#include "commands.h"
#include "querybuilder.h"

void command_init()
{
    if(!clib_file_exists(CHANGELOG_FILE)) {
        clib_write_file(CHANGELOG_FILE, "# CHANGELOG\n", "w");
    } else INFO("%s is located in this directory", CHANGELOG_FILE);
    if(!clib_directory_exists(CHANGELOG_DIR)) {
        clib_create_directory(CHANGELOG_DIR);
    } else INFO("%s/ is located in this directory", CHANGELOG_DIR);
    if(!clib_file_exists(SQLITE_DB)) {
        sqlite_create_database(SQLITE_DB);
    } else INFO("%s is already created", SQLITE_DB);
    if(!config_exists()){
        query_builder_t* qb = create_query_builder();
        insert_q(qb, TABLE_CONFIG);
        columns_q(qb, "config_path, version_major, version_minor, version_patch");
        values_q(qb, "'', 0, 0, 0");
        char* query = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, query);
        free(query);
    } else INFO("Config is already initialized");
    
}

// TODO: add to clib.h
int is_blank(const char *str) {
    if (str == NULL) {
        return 1; // NULL is considered blank
    }

    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0; // Found a non-whitespace character
        }
        str++; 
    }

    return 1; 
}

void command_add(Options options)
{
    char* message = options.argv[options.argc-1];

    if(STREQ(message, command_to_string(COMMAND_ADD))) 
        PANIC("Message is not specified. Try: `%s add \"Your message\"`", EXECUTABLE_NAME);
    if(is_blank(message))
        PANIC("Message cannot be empty or blank");
    INFO("message: %s", message);

    
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* version = select_version_full(db);
    sqlite3_close(db);

    query_builder_t* qb = create_query_builder();

    insert_q(qb, TABLE_ENTRIES);
    columns_q(qb, "message, status, version, date");
    char* values = clib_format_text("'%s', %d, '%s', '%s'", message, options.status, version, get_current_date());
    values_q(qb, values);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);
    free(values);
    free(query);
}

void command_set(Options options)
{
    const char* condition = "id = 1";

    if(version_major_set(options)){
        char* value = clib_format_text("%zu", options.version.major);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, value, condition);
        free(value);
    }

    if(version_minor_set(options)){
        char* value = clib_format_text("%zu", options.version.minor);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, value, condition);
        free(value);
    }

    if(version_patch_set(options)){
        char* value = clib_format_text("%zu", options.version.patch);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, value, condition);
        free(value);
    }

    if(version_full_set(options)){
        char* value = clib_format_text("%zu", options.version.major);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, value, condition);
        free(value);

        value = clib_format_text("%zu", options.version.minor);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, value, condition);
        free(value);

        value = clib_format_text("%zu", options.version.patch);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, value, condition);
        free(value);
    }

    if(!is_blank(options.config_path)){
        char* value = clib_format_text("'%s'", options.config_path);
        update(TABLE_CONFIG, CONFIG_CONFIG_PATH, value, condition);
        free(value);
    } else {
        ERRO("Config path must not be blank");
    }
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

void execute_command(Command command, Options options)
{
    switch (command) {
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
            command_add(options);
            return;
        case COMMAND_SET:
            command_set(options);
            return;
        case COMMAND_LIST:
        case COMMAND_DELETE:
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
