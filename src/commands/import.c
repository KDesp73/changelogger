#include "commands.h"
#include "config.h"
#include "database.h"
#include "date.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "parser.h"
#include "utils.h"
#include "version.h"

void load_version(Changelog changelog);
void load_releases(Changelog changelog);
void load_entries(Changelog changelog);
void load_remote_repo(Changelog changelog);

void command_import(Options options)
{
    if (options.file == NULL) {
        PANIC("Specify the file you want to import");
    }

    INFO("Importing %s", options.file);

    Changelog changelog = parse(options.file);
    if (!changelog.valid) {
        PANIC("Failed to import %s", options.file);
    }

    if (!options.yes) {
        WARN("This will replace all entries and releases");
        int y = yes_or_no("Continue?");

        if (!y) {
            free_changelog(&changelog);
            return;
        }
    }

    sqlite_execute_sql(SQLITE_DB, "DELETE FROM Entries");
    sqlite_execute_sql(SQLITE_DB, "DELETE FROM Releases");

    load_releases(changelog);
    load_entries(changelog);
    load_version(changelog);
    load_remote_repo(changelog);

    free_changelog(&changelog);
}

void load_releases(Changelog changelog)
{
    Date fallback_date;
    get_date(&fallback_date);

    for (size_t i = 0; i < changelog.release_count; ++i) {
        ParserRelease r = changelog.releases[i];
        if (r.version == NULL) {
            continue;
        }

        printf("Loading release %s\n", r.version);

        const char* date_str = r.date != NULL ? r.date : fallback_date.full;
        char* version_q = sql_quote(r.version);
        char* date_q = sql_quote(date_str);

        query_builder_t* qb = create_query_builder();
        insert_q(qb, TABLE_RELEASES);
        columns_q(qb, "version, date, pushed, yanked");
        char* values = clib_format_text("%s, %s, 0, %d", version_q, date_q, r.yanked);
        values_q(qb, values);
        char* sql = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, sql);

        free(version_q);
        free(date_q);
        free(values);
        free(sql);
        free(qb);
    }

    free_date(&fallback_date);
}

void load_entries(Changelog changelog)
{
    Date date;
    get_date(&date);

    for (size_t i = 0; i < changelog.release_count; ++i) {
        ParserRelease r = changelog.releases[i];
        for (size_t j = 0; j < r.change_count; ++j) {
            Change c = r.changes[j];
            if (c.message == NULL || c.status == NULL) {
                continue;
            }

            printf("Loading entry %s\n", c.message);
            CHECK_SQL_INJECTION(c.message);

            char* message_q = sql_quote(c.message);
            char* version_q = sql_quote(r.version != NULL ? r.version : VERSION_UNRELEASED);
            char* date_q = sql_quote(r.date != NULL ? r.date : date.full);

            query_builder_t* qb = create_query_builder();
            insert_q(qb, TABLE_ENTRIES);
            columns_q(qb, "message, status, version, date");
            char* values = clib_format_text(
                "%s, %d, %s, %s",
                message_q,
                get_status((char*) c.status),
                version_q,
                date_q
            );
            values_q(qb, values);
            char* sql = build_query(qb);
            sqlite_execute_sql(SQLITE_DB, sql);

            free(message_q);
            free(version_q);
            free(date_q);
            free(values);
            free(sql);
            free(qb);
        }
    }

    free_date(&date);
}

void load_version(Changelog changelog)
{
    const char* latest_version = NULL;
    for (size_t i = 0; i < changelog.release_count; ++i) {
        if (changelog.releases[i].version != NULL) {
            latest_version = changelog.releases[i].version;
            break;
        }
    }

    if (latest_version == NULL) {
        return;
    }

    Version v = {
        .full = strdup(latest_version)
    };
    parse_version(&v);

    printf("Loading version %s\n", v.full);

    char* major = clib_format_text("%zu", v.major);
    char* minor = clib_format_text("%zu", v.minor);
    char* patch = clib_format_text("%zu", v.patch);

    update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, major, CONFIG_CONDITION);
    update(TABLE_CONFIG, CONFIG_VERSION_MINOR, minor, CONFIG_CONDITION);
    update(TABLE_CONFIG, CONFIG_VERSION_PATCH, patch, CONFIG_CONDITION);

    free(major);
    free(minor);
    free(patch);
    free(v.full);
}

void load_remote_repo(Changelog changelog)
{
    if (changelog.remote_repo == NULL) {
        return;
    }

    char* remote = sql_quote(changelog.remote_repo);

    printf("Loading remote repo %s\n", changelog.remote_repo);

    update(TABLE_CONFIG, CONFIG_REMOTE_REPO, remote, CONFIG_CONDITION);
    free(remote);
}
