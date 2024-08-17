#ifndef DATABASE_H
#define DATABASE_H

#include "config.h"
#include "entry.h"
#include "release.h"
#include <sqlite3.h>
#include <stdio.h>

#define TABLE_ENTRIES "Entries"
#define FIELDS_ENTRIES "message, status, version, date"
#define TABLE_CONFIG "Config"
#define FIELDS_CONFIG "config_path, version_major, version_minor, version_patch, always_export, always_push, remote_repo"
#define TABLE_RELEASES "Releases"
#define FIELDS_RELEASES "version, date, pushed"

#define ENTRIES_MESSAGE "message"
#define ENTRIES_VERSION "version"
#define ENTRIES_STATUS "status"
#define ENTRIES_DATE "date"

#define CONFIG_VERSION_MAJOR "version_major"
#define CONFIG_VERSION_MINOR "version_minor"
#define CONFIG_VERSION_PATCH "version_patch"
#define CONFIG_CONFIG_PATH "config_path"
#define CONFIG_ALWAYS_EXPORT "always_export"
#define CONFIG_ALWAYS_PUSH "always_push"
#define CONFIG_REMOTE_REPO "remote_repo"

#define RELEASES_VERSION "version"
#define RELEASES_TITLE "title"
#define RELEASES_DATE "date"
#define RELEASES_PUSHED "pushed"
#define RELEASES_YANKED "yanked"

#define CONFIG_CONDITION "id = 1"
#define VERSION_UNRELEASED "unreleased"

#define GENERATION_QUERY \
"BEGIN TRANSACTION;" \
"CREATE TABLE IF NOT EXISTS \"Entries\" (" \
	"\"status\"	INTEGER NOT NULL," \
	"\"version\"	TEXT NOT NULL," \
	"\"message\"	TEXT NOT NULL," \
	"\"id\"	INTEGER NOT NULL UNIQUE," \
	"\"date\"	TEXT NOT NULL," \
	"PRIMARY KEY(\"id\" AUTOINCREMENT)" \
");" \
"CREATE TABLE IF NOT EXISTS \"Config\" (" \
	"\"version_major\"	INTEGER," \
	"\"version_minor\"	INTEGER," \
	"\"version_patch\"	INTEGER," \
	"\"config_path\"	TEXT," \
	"\"remote_repo\"	TEXT," \
	"\"id\"	INTEGER DEFAULT 0 UNIQUE," \
	"\"always_export\"	INTEGER DEFAULT 0," \
	"\"always_push\"	INTEGER DEFAULT 0," \
	"PRIMARY KEY(\"id\" AUTOINCREMENT)" \
");" \
"CREATE TABLE IF NOT EXISTS \"Releases\" (" \
	"\"version\"	TEXT NOT NULL UNIQUE," \
	"\"id\"	INTEGER NOT NULL UNIQUE," \
    "\"date\" TEXT NOT NULL," \
    "\"pushed\"	INTEGER DEFAULT 0," \
    "\"yanked\"	INTEGER DEFAULT 0," \
	"PRIMARY KEY(\"id\" AUTOINCREMENT)" \
");" \
"COMMIT;"

char* select_version_full(sqlite3* db);
size_t select_version_major(sqlite3* db);
size_t select_version_minor(sqlite3* db);
size_t select_version_patch(sqlite3* db);
_Bool select_always_push(sqlite3* db);
_Bool select_always_export(sqlite3* db);
char* select_config_path(sqlite3* db);
char** select_releases_version(sqlite3* db, size_t* count);
Entry* select_entries_order_by(sqlite3* db, const char* order_by, size_t *count);
Release* select_releases(sqlite3* db, const char* condition, const char* order_by, size_t *count);
Entry* select_entries(sqlite3* db, const char* condition, const char* order_by, size_t *count);
Entry* select_entries_version(sqlite3* db, const char* version, size_t *count);
Entry* select_entries_status(sqlite3* db, Status status, size_t *count);
Entry* select_entries_date_less(sqlite3* db, const char* date, size_t *count);
Entry* select_entries_date_greater(sqlite3* db, const char* date, size_t *count);
void update(const char* table, const char* column, const char* value, const char* condition); // One value;
char* select_str(const char* table, const char* column, const char* condition);
int select_int(const char* table, const char* column, const char* condition);
int config_exists();
void load_config(Config config);

#define SELECT_CONFIG_REMOTE \
    select_str(TABLE_CONFIG, CONFIG_REMOTE_REPO, CONFIG_CONDITION)

#define SELECT_CONFIG_EXPORT \
    select_int(TABLE_CONFIG, CONFIG_ALWAYS_EXPORT, CONFIG_CONDITION)

#define SELECT_CONFIG_PUSH \
    select_int(TABLE_CONFIG, CONFIG_ALWAYS_PUSH, CONFIG_CONDITION)

#endif // DATABASE_H
