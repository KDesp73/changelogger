#include "commands.h"
#include "config.h"
#include "database.h"
#include "date.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "parser.h"
#include "utils.h"
#include "version.h"

void load_version(Changelog changelog, char** script);
void load_releases(Changelog changelog, char** script);
void load_entries(Changelog changelog, char** script);
void load_remote_repo(Changelog changelog, char** script);

/*
 * The import is built as one script and executed in a single transaction.
 * Splitting it into per-statement calls would delete the old changelog before
 * knowing whether the new one can be stored, and an interrupted import would
 * leave a half-populated database behind.
 */
static void append_insert(char** script, const char* table, const char* columns, const char* values)
{
    query_builder_t* qb = create_query_builder();
    insert_q(qb, table);
    columns_q(qb, columns);
    values_q(qb, values);
    char* sql = build_query(qb);

    str_append(script, sql);
    str_append(script, ";\n");

    free(sql);
    free_query_builder(qb);
}

static void append_update(char** script, const char* table, const char* column,
                          const char* value, const char* condition)
{
    query_builder_t* qb = create_query_builder();
    update_q(qb, table);
    set_q(qb, column, value);
    where_q(qb, condition);
    char* sql = build_query(qb);

    str_append(script, sql);
    str_append(script, ";\n");

    free(sql);
    free_query_builder(qb);
}

void command_import(Options options)
{
    if (options.file == NULL) {
        PANIC("Specify the file you want to import");
    }

    // Fail before parsing and printing progress if there is nothing to import into.
    if (!fs_file_exists(SQLITE_DB)) {
        PANIC("No changelog found. Run '%s init' first", EXECUTABLE_NAME);
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

    char* script = str_buffer_init();
    str_append_ln(&script, "BEGIN TRANSACTION;");
    str_append_ln(&script, "DELETE FROM Entries;");
    str_append_ln(&script, "DELETE FROM Releases;");

    load_releases(changelog, &script);
    load_entries(changelog, &script);
    load_version(changelog, &script);
    load_remote_repo(changelog, &script);

    str_append_ln(&script, "COMMIT;");

    db_execute(script);

    free(script);
    free_changelog(&changelog);
}

void load_releases(Changelog changelog, char** script)
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

        char* values = str_format("%s, %s, 0, %d", version_q, date_q, r.yanked);
        append_insert(script, TABLE_RELEASES, "version, date, pushed, yanked", values);

        free(version_q);
        free(date_q);
        free(values);
    }

    free_date(&fallback_date);
}

void load_entries(Changelog changelog, char** script)
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

            Status entry_status = get_status((char*) c.status);
            if (entry_status == STATUS_UNKNOWN || entry_status == STATUS_UNSET) {
                WARN("Skipping entry with unknown status '%s': %s", c.status, c.message);
                continue;
            }

            char* message_q = sql_quote(c.message);
            char* version_q = sql_quote(r.version != NULL ? r.version : VERSION_UNRELEASED);
            char* date_q = sql_quote(r.date != NULL ? r.date : date.full);

            char* values = str_format(
                "%s, %d, %s, %s",
                message_q,
                entry_status,
                version_q,
                date_q
            );
            append_insert(script, TABLE_ENTRIES, "message, status, version, date", values);

            free(message_q);
            free(version_q);
            free(date_q);
            free(values);
        }
    }

    free_date(&date);
}

void load_version(Changelog changelog, char** script)
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

    char* major = str_format("%zu", v.major);
    char* minor = str_format("%zu", v.minor);
    char* patch = str_format("%zu", v.patch);

    append_update(script, TABLE_CONFIG, CONFIG_VERSION_MAJOR, major, CONFIG_CONDITION);
    append_update(script, TABLE_CONFIG, CONFIG_VERSION_MINOR, minor, CONFIG_CONDITION);
    append_update(script, TABLE_CONFIG, CONFIG_VERSION_PATCH, patch, CONFIG_CONDITION);

    free(major);
    free(minor);
    free(patch);
    free(v.full);
}

void load_remote_repo(Changelog changelog, char** script)
{
    if (changelog.remote_repo == NULL) {
        return;
    }

    char* remote = sql_quote(changelog.remote_repo);

    printf("Loading remote repo %s\n", changelog.remote_repo);

    append_update(script, TABLE_CONFIG, CONFIG_REMOTE_REPO, remote, CONFIG_CONDITION);
    free(remote);
}
