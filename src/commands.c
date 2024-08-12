#include "config.h"
#include "database.h"
#include "date.h"
#include "help.h"
#include "options.h"
#include "extern/sqlite.h"
#include "version.h"
#include <ctype.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "commands.h"
#include "extern/querybuilder.h"

void command_init(Options options)
{
    if(!clib_file_exists(CHANGELOG_FILE)) {
        clib_write_file(CHANGELOG_FILE, "# CHANGELOG\n\n\n", "w");
    } else INFO("%s is located in this directory", CHANGELOG_FILE);
    if(!clib_directory_exists(CHANGELOG_DIR)) {
        clib_create_directory(CHANGELOG_DIR);
    } else INFO("%s/ is located in this directory", CHANGELOG_DIR);
    if(!clib_file_exists(SQLITE_DB)) {
        sqlite_create_database(SQLITE_DB);
        sqlite_execute_sql(SQLITE_DB, GENERATION_QUERY);
    } else INFO("%s is already created", SQLITE_DB);
    if(!config_exists()){
        query_builder_t* qb = create_query_builder();
        insert_q(qb, TABLE_CONFIG);
        columns_q(qb, "config_path, version_major, version_minor, version_patch");
        values_q(qb, "'', 0, 0, 0"); // "Starting with version 0.0.0"
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
    

    Date date;
    get_date(&date);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_ENTRIES);
    columns_q(qb, "message, status, version, date");
    char* values = clib_format_text("'%s', %d, '%s', '%s'", message, options.status, "unknown", date.full);
    values_q(qb, values);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);
    free(values);
    free(query);
}

void command_delete(Options options)
{
    INFO("delete");
}

char* update_config_version(const char* release_type)
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    if(STREQ(release_type, "major")){
        // Reset minor and patch versions
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, "0", CONFIG_CONDITION);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, "0", CONFIG_CONDITION);

        size_t major = select_version_major(db);
        char* major_inc = clib_format_text("%zu", major+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, major_inc, CONFIG_CONDITION);
        free(major_inc);
    }

    if(STREQ(release_type, "minor")) {
        // Reset patch version
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, "0", CONFIG_CONDITION);

        size_t minor = select_version_minor(db);
        char* minor_inc = clib_format_text("%zu", minor+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, minor_inc, CONFIG_CONDITION);
        free(minor_inc);
    }

    if(STREQ(release_type, "patch")) {
        size_t patch = select_version_patch(db);
        char* patch_inc = clib_format_text("%zu", patch+1);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, patch_inc, CONFIG_CONDITION);
        free(patch_inc);
    }

    char* version = select_version_full(db);
    sqlite3_close(db);
    return version;
}

void insert_release()
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* version = select_version_full(db);
    sqlite3_close(db);

    Date date;
    get_date(&date);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_RELEASES);
    columns_q(qb, FIELDS_RELEASES);
    char* values = clib_format_text("'%s', '%s'", version, date.full);
    values_q(qb, values);
    char* query = build_query(qb);
    
    sqlite_execute_sql(SQLITE_DB, query);

    free(query);
    free(values);
}

void command_export(Options options)
{
    INFO("Exporting %s...", CHANGELOG_FILE);
}

void command_get(Options options)
{

}

void command_release(Options options)
{
    if(options.new == NULL) {
        ERRO("Options other than '--new' are not implemented or accepted");
        return;
    }

    const char* release_type = options.new;

    if(
        !STREQ(release_type, "major") &&
        !STREQ(release_type, "minor") &&
        !STREQ(release_type, "patch")
    ){
        PANIC("Release type '%s' should be 'major', 'minor' or 'patch'. Try %s release -h", release_type, EXECUTABLE_NAME);
    }

    char* version = update_config_version(release_type);
    insert_release();
    char* query = clib_format_text("UPDATE Entries SET version = '%s' WHERE version = 'unknown'", version);
    sqlite_execute_sql(SQLITE_DB, query);

    // TODO: $ gh release create...
}

void command_list(Options options)
{
    INFO("list");
}

void command_set(Options options)
{
    if(version_major_set(options)){
        char* value = clib_format_text("%zu", options.version.major);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, value, CONFIG_CONDITION);
        free(value);
    }

    if(version_minor_set(options)){
        char* value = clib_format_text("%zu", options.version.minor);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, value, CONFIG_CONDITION);
        free(value);
    }

    if(version_patch_set(options)){
        char* value = clib_format_text("%zu", options.version.patch);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, value, CONFIG_CONDITION);
        free(value);
    }

    if(version_full_set(options)){
        char* value = clib_format_text("%zu", options.version.major);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, value, CONFIG_CONDITION);
        free(value);

        value = clib_format_text("%zu", options.version.minor);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, value, CONFIG_CONDITION);
        free(value);

        value = clib_format_text("%zu", options.version.patch);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, value, CONFIG_CONDITION);
        free(value);
    }

    if(always_export_set(options)){
        char* value = clib_format_text("%d", options.always_export);
        update(TABLE_CONFIG, CONFIG_ALWAYS_EXPORT, value, CONFIG_CONDITION);
        free(value);
    }

    if(!is_blank(options.config_path)){
        char* value = clib_format_text("'%s'", options.config_path);
        update(TABLE_CONFIG, CONFIG_CONFIG_PATH, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.config_path != NULL) // Only when it set blank by the user
            ERRO("Config path must not be blank");
    }
}


Command get_command(char* command)
{
#define COMPARE_AND_RETURN_COMMAND(c) \
    else if(STREQ(command, command_to_string(c))) return c;

    if(command == NULL) return COMMAND_UNSET;
    COMPARE_AND_RETURN_COMMAND(COMMAND_EXPORT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_ADD)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_INIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_SET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_LIST)
    COMPARE_AND_RETURN_COMMAND(COMMAND_DELETE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_RELEASE)
    else return COMMAND_UNKNOWN;

#undef COMPARE_AND_RETURN_COMMAND
}

void execute_command(Command command, Options options)
{
    switch (command) {
    case COMMAND_UNKNOWN:
        ERRO("Unknown command: %s", options.argv[1]);
        break;
    case COMMAND_UNSET:
        ERRO("Command not found");
        break;
    case COMMAND_INIT:
        command_init(options);
        return;
    case COMMAND_ADD:
        command_add(options);
        return;
    case COMMAND_SET:
        command_set(options);
        return;
    case COMMAND_LIST:
        command_list(options);
        return;
    case COMMAND_DELETE:
        command_delete(options);
        return;
    case COMMAND_RELEASE:
        command_release(options);
        return;
    case COMMAND_EXPORT:
        command_export(options);
        return;
    case COMMAND_GET:
        command_get(options);
        break;
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
    case COMMAND_RELEASE:
        return "release";
    case COMMAND_GET:
        return "get";
    case COMMAND_EXPORT:
        return "export";
    }

    return "";
}
