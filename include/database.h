#ifndef DATABASE_H
#define DATABASE_H

#include "entry.h"
#include "sqlite.h"
#include <stdio.h>

#define TABLE_ENTRIES "Entries"
#define TABLE_CONFIG "Config"
#define FIELDS_ENTRIES "message, status, version, date"
#define FIELDS_CONFIG "config_path, version_major, version_minor, version_patch"

#define ENTRIES_MESSAGE "message"
#define ENTRIES_VERSION "version"
#define ENTRIES_STATUS "status"
#define ENTRIES_DATE "date"

#define CONFIG_VERSION_MAJOR "version_major"
#define CONFIG_VERSION_MINOR "version_minor"
#define CONFIG_VERSION_PATCH "version_patch"
#define CONFIG_CONFIG_PATH "config_path"

#define GENERATION_QUERY \
"BEGIN TRANSACTION;" \
"CREATE TABLE IF NOT EXISTS \"Entries\" (" \
	"\"status\"	INTEGER," \
	"\"version\"	TEXT," \
	"\"message\"	TEXT," \
	"\"id\"	INTEGER NOT NULL UNIQUE," \
	"\"date\"	TEXT," \
	"PRIMARY KEY(\"id\" AUTOINCREMENT)" \
");" \
"CREATE TABLE IF NOT EXISTS \"Config\" (" \
	"\"version_major\"	INTEGER," \
	"\"version_minor\"	INTEGER," \
	"\"version_patch\"	INTEGER," \
	"\"config_path\"	TEXT," \
	"\"id\"	INTEGER DEFAULT 0 UNIQUE," \
	"PRIMARY KEY(\"id\" AUTOINCREMENT)" \
");" \
"COMMIT;"

char* select_version_full(sqlite3* db);
size_t select_version_major(sqlite3* db);
size_t select_version_minor(sqlite3* db);
size_t select_version_patch(sqlite3* db);
char* select_config_path(sqlite3* db);
Entry* select_entries_version(sqlite3* db, size_t *count);
Entry* select_entries_status(sqlite3* db, Status status, size_t *count);
Entry* select_entries_date_less(sqlite3* db, const char* date, size_t *count);
Entry* select_entries_date_greater(sqlite3* db, const char* date, size_t *count);
void update(const char* table, const char* column, const char* value, const char* condition);
int config_exists();

#endif // DATABASE_H
