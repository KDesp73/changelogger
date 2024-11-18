#include "commands.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "parser.h"
#include "utils.h"

void load_version(Changelog changelog);
void load_releases(Changelog changelog);
void load_entries(Changelog changelog);
void load_remote_repo(Changelog changelog);

void command_import(Options options)
{
    if(options.file == NULL){
        PANIC("Specify the file you want to import");
    }

    INFO("Importing %s", options.file);

    char* delete_releases = "DELETE FROM Releases";
    char* delete_entries = "DELETE FROM Entries";

    if(!options.yes){
        WARN("This will replace all entries and releases");
        int y = yes_or_no("Continue?");

        if(!y) return;
    }

    sqlite_execute_sql(SQLITE_DB, delete_entries);
    sqlite_execute_sql(SQLITE_DB, delete_releases);

    Changelog changelog = parse(options.file);

    load_releases(changelog);
    load_entries(changelog);
    load_version(changelog);
    load_remote_repo(changelog);

    free_changelog(&changelog);
}

void load_releases(Changelog changelog)
{
    for(size_t i = 0; i < changelog.release_count; ++i){
        query_builder_t* qb = create_query_builder();
        ParserRelease r = changelog.releases[i];
        if(r.version == NULL) continue;

        printf("Loading release %s\n", r.version);

        insert_q(qb, TABLE_RELEASES);
        columns_q(qb, "version, date, pushed");
        char* values = clib_format_text("'%s', '%s', 0", r.version, r.date);
        values_q(qb, values);
        free(values);
        char* sql = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, sql);
        free(sql);
        free(qb);
    }
}

void load_entries(Changelog changelog)
{
    Date date;
    get_date(&date);

    for (size_t i =0; i < changelog.release_count; ++i) {
        ParserRelease r = changelog.releases[i];
        for(size_t j = 0; j < r.change_count; ++j){
            query_builder_t* qb = create_query_builder();
            Change c = r.changes[j];
            printf("Loading entry %s\n", c.message);

            insert_q(qb, TABLE_ENTRIES);
            columns_q(qb, "message, status, version, date");
            char* values = clib_format_text(
                "'%s', %d, '%s', '%s'", 
                c.message, 
                get_status((char*) c.status), 
                (r.version) ? r.version : VERSION_UNRELEASED, 
                (r.date) ? r.date : date.full
            );
            values_q(qb, values);
            free(values);
            char* sql = build_query(qb);
            sqlite_execute_sql(SQLITE_DB, sql);
            free(sql);
            free(qb);
        }
    }
}

void load_version(Changelog changelog)
{
    ParserRelease latest = changelog.releases[1];
    Version v = {
        .full = (char*) latest.version
    };
    parse_version(&v);

    printf("Loading version %s\n", v.full);

    char* major = clib_format_text("%zu", v.major);
    char* minor = clib_format_text("%zu", v.minor);
    char* patch = clib_format_text("%zu", v.patch);

    update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, major, CONFIG_CONDITION);
    update(TABLE_CONFIG, CONFIG_VERSION_MINOR, minor, CONFIG_CONDITION);
    update(TABLE_CONFIG, CONFIG_VERSION_PATCH, patch, CONFIG_CONDITION);

    free(major);
    free(minor);
    free(patch);
}

void load_remote_repo(Changelog changelog){
    if(changelog.remote_repo == NULL) return;

    char* remote = clib_format_text("'%s'", changelog.remote_repo);
    
    printf("Loading remote repo %s\n", changelog.remote_repo);

    update(TABLE_CONFIG, CONFIG_REMOTE_REPO, remote, CONFIG_CONDITION);
    free(remote);
}
