BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Config" (
	"version_major"	INTEGER,
	"version_minor"	INTEGER,
	"version_patch"	INTEGER,
	"config_dir"	TEXT,
	"id"	INTEGER DEFAULT 0 UNIQUE,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Entries" (
	"status"	TEXT,
	"version"	TEXT,
	"message"	TEXT,
	"id"	INTEGER NOT NULL UNIQUE,
	"date"	TEXT,
	PRIMARY KEY("id" AUTOINCREMENT)
);
COMMIT;
