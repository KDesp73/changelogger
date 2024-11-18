#include "commands.h"
#include "config.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"

void command_init(Options options)
{
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
        columns_q(qb, "config_path, version_major, version_minor, "
                      "version_patch, always_export, always_push, remote_repo");
        values_q(qb, "'', 0, 0, 0, 0, 0, ''"); // "Starting with version 0.0.0"
        char* query = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, query);
        free(query);
    } else INFO("Config is already initialized");
}
