BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Entries" (
	"status"	INTEGER,
	"version"	TEXT,
	"message"	TEXT,
	"id"	INTEGER NOT NULL UNIQUE,
	"date"	TEXT,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Config" (
	"version_major"	INTEGER,
	"version_minor"	INTEGER,
	"version_patch"	INTEGER,
	"config_path"	TEXT,
	"id"	INTEGER DEFAULT 0 UNIQUE,
	"always_export"	INTEGER,
	"remote_repo"	TEXT,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Releases" (
	"version"	TEXT NOT NULL UNIQUE,
	"title"	TEXT NOT NULL,
	"id"	INTEGER NOT NULL UNIQUE,
	PRIMARY KEY("id" AUTOINCREMENT)
);
COMMIT;
