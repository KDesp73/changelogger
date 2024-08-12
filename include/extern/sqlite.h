#ifndef SQLITE_H
#define SQLITE_H

#include "database.h"
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

#ifndef SQLITEAPI
    #define SQLITEAPI static
#endif

#pragma GCC diagnostic ignored "-Wunused-function"

SQLITEAPI int sqlite_create_database(const char *db_name);
SQLITEAPI int sqlite_execute_sql(const char *db_name, const char *sql);
SQLITEAPI int sqlite_query_data(const char *db_name, const char *sql, int (*callback)(void*, int, char**, char**));
SQLITEAPI void sqlite_disable_logging(sqlite3* db);

SQLITEAPI void sqlite_disable_logging(sqlite3* db)
{
    int rc = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION, 0, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to disable load extension for db: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
}

SQLITEAPI int sqlite_create_database(const char *db_name) 
{
    sqlite3 *db;
    int rc = sqlite3_open(db_name, &db);
    
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    sqlite3_close(db);
    return 0; // Success
}

SQLITEAPI int sqlite_execute_sql(const char *db_name, const char *sql) 
{
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite_disable_logging(db);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        fprintf(stdout, "SQL executed successfully\n");
    }

    sqlite3_close(db);
    return rc; // Return the result code
}

// Function to query data from the database
SQLITEAPI int sqlite_query_data(const char *db_name, const char *sql, int (*callback)(void*, int, char**, char**)) 
{
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite_disable_logging(db);

    // Execute SQL statement
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    sqlite3_close(db);
    return rc; // Return the result code
}

#endif // SQLITE_H
