#include "database.h"
#include "clib.h"
#include "config.h"
#include "querybuilder.h"
#include "sqlite.h"
#include "version.h"
#include <stdlib.h>
#include <string.h>

size_t select_version_major(sqlite3 *db)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_major FROM Config WHERE id = 1;";
    sqlite3_stmt *stmt;
    size_t major = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            major = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return major;
}

// Function to select the minor version
size_t select_version_minor(sqlite3 *db)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_minor FROM Config WHERE id = 1;";
    sqlite3_stmt *stmt;
    size_t minor = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            minor = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return minor;
}

// Function to select the patch version
size_t select_version_patch(sqlite3 *db)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_patch FROM Config WHERE id = 1;";
    sqlite3_stmt *stmt;
    size_t patch = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            patch = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return patch;
}

char* select_version_full(sqlite3* db)
{
    Version v = {0};
    v.major = select_version_major(db);
    v.minor = select_version_minor(db);
    v.patch = select_version_patch(db);
    make_version(&v);

    return v.full;
}

// Function to select the config path
char* select_config_path(sqlite3 *db)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT config_path FROM Config WHERE id = 0;";
    sqlite3_stmt *stmt;
    char *config_path = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *path = (const char *)sqlite3_column_text(stmt, 0);
            config_path = strdup(path); // Duplicate the string
        }
    }
    sqlite3_finalize(stmt);
    return config_path;
}

// Function to select entries by version
Entry* select_entries_version(sqlite3 *db, size_t *count)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_major, version_minor, version_patch, message FROM Entries;";
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].version.major = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.minor = sqlite3_column_int(stmt, 1);
            entries[entry_count].version.patch = sqlite3_column_int(stmt, 2);
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 3));
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

// Function to select entries by status
Entry* select_entries_status(sqlite3 *db, Status status, size_t *count)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_major, version_minor, version_patch, message FROM Entries WHERE status = ?;";
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, status);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].version.major = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.minor = sqlite3_column_int(stmt, 1);
            entries[entry_count].version.patch = sqlite3_column_int(stmt, 2);
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 3));
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

// Function to select entries with date less than a given date
Entry* select_entries_date_less(sqlite3 *db, const char* date, size_t *count)
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_major, version_minor, version_patch, message FROM Entries WHERE date < ?;";
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].version.major = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.minor = sqlite3_column_int(stmt, 1);
            entries[entry_count].version.patch = sqlite3_column_int(stmt, 2);
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 3));
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

// Function to select entries with date greater than a given date
Entry* select_entries_date_greater(sqlite3 *db, const char* date, size_t *count) 
{
    sqlite_disable_logging(db);
    const char *sql = "SELECT version_major, version_minor, version_patch, message FROM Entries WHERE date > ?;";
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].version.major = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.minor = sqlite3_column_int(stmt, 1);
            entries[entry_count].version.patch = sqlite3_column_int(stmt, 2);
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 3));
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

void update(const char* table, const char* column, const char* value, const char* condition)
{
    query_builder_t* qb = create_query_builder();
    update_q(qb, table);
    set_q(qb, column, value);
    where_q(qb, condition);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);
    free(query);
}

int config_exists() {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int exists = 0;

    if (sqlite3_open(SQLITE_DB, &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    sqlite_disable_logging(db);

    const char *sql = "SELECT COUNT(*) FROM Config WHERE id = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        exists = (count > 0) ? 1 : 0; // Set exists to 1 if count > 0
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}
