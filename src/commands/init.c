#include "commands.h"
#include "config.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"

void command_init(Options options)
{
    (void) options;

    if(!fs_directory_exists(CHANGELOG_DIR)) {
        fs_create_directory(CHANGELOG_DIR);
    } else INFO("%s/ is located in this directory", CHANGELOG_DIR);

    if(!fs_file_exists(SQLITE_DB)) {
        sqlite_create_database(SQLITE_DB);
    } else INFO("%s is already created", SQLITE_DB);

    // Idempotent: also adds tables that a database from an older version is
    // missing, so `init` doubles as the schema upgrade path.
    db_execute(GENERATION_QUERY);

    if(!config_exists()){
        query_builder_t* qb = create_query_builder();
        insert_q(qb, TABLE_CONFIG);
        columns_q(qb, "config_path, version_major, version_minor, "
                      "version_patch, always_export, always_push, remote_repo");
        values_q(qb, "'', 0, 0, 0, 0, 0, ''"); // "Starting with version 0.0.0"
        char* query = build_query(qb);
        db_execute(query);
        free(query);
    } else INFO("Config is already initialized");

    char* schema = str_format("PRAGMA user_version = %d;", SCHEMA_VERSION);
    db_execute(schema);
    free(schema);
}
