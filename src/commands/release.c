#include "commands.h"
#include "database.h"
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

        // The release always starts as NOT pushed: `pushed` must reflect what
        // gh actually did, not what we intend to do.
        char* version = create_release(release_type);

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

/*
 * Bumps the requested component of the current version without writing
 * anything; the caller decides when the new version is stored.
 */
static char* next_version(const char* release_type, size_t* major, size_t* minor, size_t* patch)
{
    sqlite3* db;
    if (sqlite3_open(SQLITE_DB, &db) != SQLITE_OK) {
        PANIC("Failed to open %s: %s", SQLITE_DB, sqlite3_errmsg(db));
    }

    *major = select_version_major(db);
    *minor = select_version_minor(db);
    *patch = select_version_patch(db);
    sqlite3_close(db);

    if (STREQ(release_type, "major")) {
        (*major)++;
        *minor = 0;
        *patch = 0;
    } else if (STREQ(release_type, "minor")) {
        (*minor)++;
        *patch = 0;
    } else if (STREQ(release_type, "patch")) {
        (*patch)++;
    }

    return str_format("%zu.%zu.%zu", *major, *minor, *patch);
}

/*
 * Creates the release and moves the unreleased entries to it in a single
 * transaction. A failure must not leave a release behind while the entries
 * are still marked as unreleased (or the new version in Config without its
 * release row). The release is always recorded as NOT pushed: only a
 * successful push may mark it otherwise.
 */
char* create_release(const char* release_type)
{
    size_t major, minor, patch;
    char* version = next_version(release_type, &major, &minor, &patch);

    Date date;
    get_date(&date);

    char* version_q = sql_quote(version);
    char* date_q = sql_quote(date.full);

    char* script = str_format(
        "BEGIN TRANSACTION;"
        "UPDATE \"" TABLE_CONFIG "\" SET "
            CONFIG_VERSION_MAJOR " = %zu, "
            CONFIG_VERSION_MINOR " = %zu, "
            CONFIG_VERSION_PATCH " = %zu "
            "WHERE " CONFIG_CONDITION ";"
        "INSERT INTO \"" TABLE_RELEASES "\" ("
            RELEASES_VERSION ", " RELEASES_DATE ", " RELEASES_PUSHED ") "
            "VALUES (%s, %s, 0);"
        "UPDATE \"" TABLE_ENTRIES "\" SET " ENTRIES_VERSION " = %s "
            "WHERE " ENTRIES_VERSION " = '%s';"
        "COMMIT;",
        major, minor, patch,
        version_q, date_q,
        version_q, VERSION_UNRELEASED
    );

    db_execute(script);

    free(script);
    free(version_q);
    free(date_q);
    free_date(&date);

    return version;
}

/*
 * `set -V <version>`: records the version in Config and creates its release
 * row in a single transaction. Versions are unique, so setting the same
 * version twice is rejected before anything is written.
 */
void create_release_for_version(const char* version)
{
    if (!is_valid_version(version)) {
        PANIC("Version '%s' is not valid", version);
    }

    size_t major = 0, minor = 0, patch = 0;
    if (sscanf(version, "%zu.%zu.%zu", &major, &minor, &patch) != 3) {
        PANIC("Version '%s' is not valid", version);
    }

    char* version_q = sql_quote(version);
    char* condition = str_format("version = %s", version_q);
    char* existing = select_str(TABLE_RELEASES, RELEASES_VERSION, condition);
    free(condition);

    if (existing != NULL) {
        free(existing);
        free(version_q);
        PANIC("Release v%s already exists", version);
    }

    Date date;
    get_date(&date);
    char* date_q = sql_quote(date.full);

    char* script = str_format(
        "BEGIN TRANSACTION;"
        "UPDATE \"" TABLE_CONFIG "\" SET "
            CONFIG_VERSION_MAJOR " = %zu, "
            CONFIG_VERSION_MINOR " = %zu, "
            CONFIG_VERSION_PATCH " = %zu "
            "WHERE " CONFIG_CONDITION ";"
        "INSERT INTO \"" TABLE_RELEASES "\" ("
            RELEASES_VERSION ", " RELEASES_DATE ", " RELEASES_PUSHED ") "
            "VALUES (%s, %s, 0);"
        "COMMIT;",
        major, minor, patch, version_q, date_q
    );

    db_execute(script);

    free(script);
    free(version_q);
    free(date_q);
    free_date(&date);
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

