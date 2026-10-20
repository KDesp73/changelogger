#include "commands.h"
#include "database.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "push_utils.h"
#include "release_utils.h"
#include "utils.h"

void command_release(Options options)
{
    if(options.new != NULL && options.yank != NULL){
        PANIC("--new and --yank cannot be used together");
    }

    if(options.new != NULL){
        const char* release_type = options.new;
        if(
                !STREQ(release_type, "major") &&
                !STREQ(release_type, "minor") &&
                !STREQ(release_type, "patch")
        ){
            PANIC("Release type '%s' should be 'major', 'minor' or 'patch'. Try %s release -h", release_type, EXECUTABLE_NAME);
        }

        make_sure_user_wants_to_proceed_with_releasing(options);

        sqlite3* db;
        sqlite3_open(SQLITE_DB, &db);
        _Bool release = select_always_push(db);
        sqlite3_close(db);

        _Bool should_push = release || options.push;

        char* version = update_config_version(release_type);
        // The release always starts as NOT pushed: pushed must reflect
        // what gh actually did, not what we intend to do.
        insert_release(0);

        char* version_q = sql_quote(version);
        char* query = str_format("UPDATE Entries SET version = %s WHERE version = 'unreleased'", version_q);
        free(version_q);
        db_execute(query);
        free(query);

        if(!should_push) {
            free(version);
            return; // Do not push the release on Github
        }

        if(push_release(version, options.asset)){
            mark_release_pushed(version);
        } else {
            // push_release() already reported why. The release stays
            // pending, so `changelogger push -V <version>` can retry it.
            WARN("Release v%s was not marked as pushed.", version);
            free(version);
            exit(1); // The release itself exists, but the requested push failed
        }

        free(version);
        return;
    }

    if(options.yank != NULL){
        char* version = (char*)options.yank;
        char* version_q = sql_quote(version);
        char* condition = str_format("version = %s", version_q);
        free(version_q);
        int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);

        if(yanked) {
            WARN("Release v%s is already yanked", version);
            free(condition);
            exit(0);
        }

        update(TABLE_RELEASES, RELEASES_YANKED, "1", condition);
        free(condition);
        INFO("Release v%s was set as YANKED", version);
    }

    if(options.unyank != NULL){
        char* version = (char*)options.unyank;
        char* version_q = sql_quote(version);
        char* condition = str_format("version = %s", version_q);
        free(version_q);
        int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);

        if(!yanked) {
            WARN("Release v%s is already not yanked", version);
            free(condition);
            exit(0);
        }

        update(TABLE_RELEASES, RELEASES_YANKED, "0", condition);
        free(condition);
        INFO("Release v%s was set as not YANKED", version);
    }
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
        char* major_inc = str_format("%zu", major+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, major_inc, CONFIG_CONDITION);
        free(major_inc);
    }

    if(STREQ(release_type, "minor")) {
        // Reset patch version
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, "0", CONFIG_CONDITION);

        size_t minor = select_version_minor(db);
        char* minor_inc = str_format("%zu", minor+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, minor_inc, CONFIG_CONDITION);
        free(minor_inc);
    }

    if(STREQ(release_type, "patch")) {
        size_t patch = select_version_patch(db);
        char* patch_inc = str_format("%zu", patch+1);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, patch_inc, CONFIG_CONDITION);
        free(patch_inc);
    }

    char* version = select_version_full(db);
    sqlite3_close(db);
    return version;
}

void insert_release(_Bool pushed)
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

    char* version_q = sql_quote(version);
    char* date_q = sql_quote(date.full);
    char* values = str_format("%s, %s, %d", version_q, date_q, pushed);
    values_q(qb, values);
    char* query = build_query(qb);

    db_execute(query);

    free(version_q);
    free(date_q);
    free_date(&date);
    free(version);
    free(values);
    free(query);
    free_query_builder(qb);
}

/*
 * Flip the pushed flag for an existing release. Only called after gh
 * reported success, so the database never claims a push that failed.
 */
void mark_release_pushed(const char* version)
{
    char* version_q = sql_quote(version);
    char* condition = str_format("version = %s", version_q);
    free(version_q);

    update(TABLE_RELEASES, RELEASES_PUSHED, "1", condition);
    free(condition);
}

