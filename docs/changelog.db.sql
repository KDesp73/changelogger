BEGIN TRANSACTION; 
    CREATE TABLE IF NOT EXISTS Entries ( 
        status	INTEGER NOT NULL, 
        version	TEXT NOT NULL, 
        message	TEXT NOT NULL, 
        id	INTEGER NOT NULL UNIQUE, 
        date	TEXT NOT NULL, 
        PRIMARY KEY(id AUTOINCREMENT) 
    ); 
    CREATE TABLE IF NOT EXISTS Config ( 
        version_major	INTEGER, 
        version_minor	INTEGER, 
        version_patch	INTEGER, 
        config_path	TEXT, 
        remote_repo	TEXT, 
        id	INTEGER DEFAULT 0 UNIQUE, 
        always_export	INTEGER DEFAULT 0, 
        always_push	INTEGER DEFAULT 0, 
        editor TEXT DEFAULT 'vim',
        PRIMARY KEY(id AUTOINCREMENT) 
    ); 
    CREATE TABLE IF NOT EXISTS Releases ( 
        version	TEXT NOT NULL UNIQUE, 
        id	INTEGER NOT NULL UNIQUE, 
        date TEXT NOT NULL, 
        pushed	INTEGER DEFAULT 0, 
        yanked	INTEGER DEFAULT 0, 
        PRIMARY KEY(id AUTOINCREMENT) 
    ); 
COMMIT;

