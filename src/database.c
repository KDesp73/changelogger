#include "database.h"
#include "date.h"
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "config.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "version.h"
#include <stdlib.h>
#include <string.h>

size_t select_version_major(sqlite3 *db)
{
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

_Bool select_always_push(sqlite3* db)
{
    const char *sql = "SELECT always_push FROM Config WHERE id = 1;";
    sqlite3_stmt *stmt;
    _Bool always_push = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            always_push = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return always_push;
    
}

_Bool select_always_export(sqlite3* db)
{
    const char *sql = "SELECT always_export FROM Config WHERE id = 1;";
    sqlite3_stmt *stmt;
    _Bool always_export = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            always_export = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return always_export;
    
}

// Function to select the patch version
size_t select_version_patch(sqlite3 *db)
{
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


Entry* select_entries_order_by(sqlite3* db, const char* column, size_t *count)
{
    char *sql = clib_format_text("SELECT * FROM Entries ORDER BY %s;", column);
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].status = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.full = strdup((char*) sqlite3_column_text(stmt, 1));
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 2));
            entries[entry_count].date.full = strdup((char*) sqlite3_column_text(stmt, 4));

            parse_version(&entries[entry_count].version);
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;

    free(sql);
    return entries;
}

Release* select_releases(sqlite3* db, const char* condition, const char* order_by, size_t *count)
{
    char *sql = clib_buffer_init();
    clib_str_append(&sql, "SELECT * FROM Releases");
    if(condition != NULL){
        char* where = clib_format_text(" WHERE %s", condition);
        clib_str_append(&sql, where);
        free(where);
    }
    if(order_by != NULL){
        char* orderby = clib_format_text(" ORDER BY %s", order_by);
        clib_str_append(&sql, orderby);
        free(orderby);
    }
    clib_str_append(&sql, ";");

    sqlite3_stmt *stmt;
    Release* releases = NULL;
    size_t release_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            releases = realloc(releases, sizeof(Entry) * (release_count + 1));
            releases[release_count].version.full = strdup((char*) sqlite3_column_text(stmt, 0));
            releases[release_count].date.full = strdup((char*) sqlite3_column_text(stmt, 2));
            releases[release_count].pushed = sqlite3_column_int(stmt, 3);

            parse_version(&releases[release_count].version);
            releases[release_count].date = parse_date(releases[release_count].date.full);
            release_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = release_count;
    return releases;

}

Entry* select_entries(sqlite3* db, const char* condition, const char* order_by, size_t *count)
{
    char *sql = clib_buffer_init();
    clib_str_append(&sql, "SELECT * FROM Entries");
    if(condition != NULL){
        char* where = clib_format_text(" WHERE %s", condition);
        clib_str_append(&sql, where);
        free(where);
    }
    if(order_by != NULL){
        char* orderby = clib_format_text(" ORDER BY %s", order_by);
        clib_str_append(&sql, orderby);
        free(orderby);
    }
    clib_str_append(&sql, ";");

    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].status = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.full = strdup((char*) sqlite3_column_text(stmt, 1));
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 2));
            entries[entry_count].date.full = strdup((char*) sqlite3_column_text(stmt, 4));

            parse_version(&entries[entry_count].version);
            entries[entry_count].date = parse_date(entries[entry_count].date.full);
            entry_count++;
        }
    }
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

// Function to select entries by version
Entry* select_entries_version(sqlite3* db, const char* version, size_t *count)
{
    char *sql = clib_format_text("SELECT * FROM Entries WHERE version = %s;", version);
    sqlite3_stmt *stmt;
    Entry *entries = NULL;
    size_t entry_count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            entries = realloc(entries, sizeof(Entry) * (entry_count + 1));
            entries[entry_count].status = sqlite3_column_int(stmt, 0);
            entries[entry_count].version.full = strdup((char*) sqlite3_column_text(stmt, 1));
            entries[entry_count].message = strdup((const char *)sqlite3_column_text(stmt, 2));
            entries[entry_count].date.full = strdup((char*) sqlite3_column_text(stmt, 4));

            parse_version(&entries[entry_count].version);
            entry_count++;
        }
    }
    free(sql);
    sqlite3_finalize(stmt);
    *count = entry_count;
    return entries;
}

// Function to select entries by status
Entry* select_entries_status(sqlite3 *db, Status status, size_t *count)
{
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


int select_int(const char* table, const char* column, const char* condition)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int result = 0; // Default result value
    int rc;

    // Open the SQLite database
    rc = sqlite3_open(SQLITE_DB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1; // Return an error code
    }

    // Prepare the SQL statement
    char *sql;
    asprintf(&sql, "SELECT %s FROM %s WHERE %s;", column, table, condition);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_free(sql);
        sqlite3_close(db);
        return -1; // Return an error code
    }

    // Execute the statement and fetch the result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int(stmt, 0); // Get the integer value
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);

    return result; // Return the integer result
}

char* select_str(const char* table, const char* column, const char* condition) 
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *result = NULL;
    char *sql;
    int rc;

    rc = sqlite3_open(SQLITE_DB, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    asprintf(&sql, "SELECT %s FROM %s WHERE %s;", column, table, condition);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_free(sql);
        sqlite3_close(db);
        return NULL;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *text = (const char *)sqlite3_column_text(stmt, 0);
        if (text) {
            result = strdup(text); // Duplicate the string to return
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);

    return result; 
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

    const char *sql = "SELECT COUNT(*) FROM Config WHERE id = 1;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        exists = count > 0;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

char** select_releases_version(sqlite3* db, size_t* count) {
    sqlite3_stmt *stmt;
    char **versions = NULL;
    int rc;
    int capacity = 10;
    *count = 0; // Initialize count to 0

    const char *sql = "SELECT version FROM releases;";

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Allocate initial memory for the versions array
    versions = malloc(capacity * sizeof(char*));
    if (versions == NULL) {
        fprintf(stderr, "Failed to allocate memory for versions array.\n");
        sqlite3_finalize(stmt);
        return NULL;
    }

    // Execute the statement and fetch the results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *version = (const char *)sqlite3_column_text(stmt, 0);
        if (version) {
            // Resize the array if necessary
            if (*count >= capacity) {
                capacity *= 2;
                char **temp = realloc(versions, capacity * sizeof(char*));
                if (temp == NULL) {
                    fprintf(stderr, "Failed to reallocate memory for versions array.\n");
                    // Free previously allocated versions
                    for (size_t i = 0; i < *count; i++) {
                        free(versions[i]);
                    }
                    free(versions);
                    sqlite3_finalize(stmt);
                    return NULL;
                }
                versions = temp;
            }
            // Duplicate the version string and store it in the array
            versions[*count] = strdup(version);
            if (versions[*count] == NULL) {
                fprintf(stderr, "Failed to duplicate version string.\n");
                // Free previously allocated versions
                for (size_t i = 0; i < *count; i++) {
                    free(versions[i]);
                }
                free(versions);
                sqlite3_finalize(stmt);
                return NULL;
            }
            (*count)++; // Increment the count correctly
        }
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    // Resize the array to the actual number of versions found
    char **temp = realloc(versions, (*count + 1) * sizeof(char*)); // +1 for NULL terminator
    if (temp == NULL) {
        fprintf(stderr, "Failed to reallocate memory for versions array.\n");
        // Free previously allocated versions
        for (size_t i = 0; i < *count; i++) {
            free(versions[i]);
        }
        free(versions);
        return NULL;
    }
    versions = temp;
    versions[*count] = NULL; // NULL-terminate the array

    return versions; // Caller is responsible for freeing the array and its contents
}

void load_config(Config config)
{
    char* sql = clib_buffer_init();
    clib_str_append(&sql, "UPDATE Config SET ");

    char* value = clib_format_text("%s = %d", CONFIG_ALWAYS_EXPORT, config.always_export);
    clib_str_append(&sql, value);
    free(value);

    clib_str_append(&sql, ", ");

    value = clib_format_text("%s = %d", CONFIG_ALWAYS_PUSH, config.always_push);
    clib_str_append(&sql, value);
    free(value);

    clib_str_append(&sql, " WHERE id = 1;");

    sqlite_execute_sql(SQLITE_DB, sql);
}
