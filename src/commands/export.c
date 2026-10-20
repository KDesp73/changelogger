#include "commands.h"
#include "config.h"
#include "database.h"
#include "templates.h"
#include "utils.h"

void export_markdown();

void command_export(Options options)
{
    char* fmt = NULL;
    if(options.format == NULL){
        options.format = "md";
    }

    if(
        !STREQ(options.format, "md") &&
        !STREQ(options.format, "markdown") &&
        !STREQ(options.format, "json") &&
        !STREQ(options.format, "yaml") &&
        !STREQ(options.format, "yml")
    ){
        PANIC("Format must be 'md', 'markdown', 'json', 'yml' or 'yaml'");
    }
    fmt = (char*) options.format;

    if(STREQ(fmt, "md"))
        INFO("Exporting %s...", CHANGELOG_FILE);
    export_markdown();
    if(STREQ(fmt, "md")){
        INFO("Export complete.");
        return;
    }

    char* command = str_format("/usr/bin/clparse -f %s %s", fmt, CHANGELOG_FILE);
    char* export_string = NULL;
    int status = execute_command_capture(command, &export_string);
    free(command);

    if (status != 0) {
        free(export_string);
        ERRO("Failed to export %s to %s", CHANGELOG_FILE, fmt);
        exit(1);
    }

    printf("%s\n", export_string != NULL ? export_string : "");
    free(export_string);
}

int compare_entries(const void* a, const void* b) {
    const Entry* entry_a = (const Entry*)a;
    const Entry* entry_b = (const Entry*)b;

    int comp_versions = compare_versions(entry_a->version, entry_b->version);
    if(comp_versions != 0){
        return comp_versions;
    }

    if (entry_a->status != entry_b->status) {
        return entry_a->status - entry_b->status;
    }
    return strcmp(entry_b->date.full, entry_a->date.full);
}
void export_markdown()
{
    char* buffer = str_buffer_init();
    str_append_ln(&buffer, TEMPLATE_HEADER);
    str_append_ln(&buffer, TEMPLATE_NOTE);
    
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    size_t count;
    Entry* entries = select_entries_order_by(db, "version DESC, status ASC, date DESC", &count);
    if(entries == NULL) {
        INFO("Nothing to export");
        fs_write_file(CHANGELOG_FILE, buffer, "w");
        free(buffer);
        sqlite3_close(db);
        exit(0);
    }
    qsort(entries, count, sizeof(Entry), compare_entries);

    char* version = entries[0].version.full;
    Status status = entries[0].status;
    char* release_buffer = str_buffer_init();

    for(size_t i = 0; i < count; ++i){
        if(i == 0 || !STREQ(version, entries[i].version.full)){
            str_append_ln(&buffer, "");
            if(STREQ(entries[i].version.full, "0.0.0")){
                str_append_ln(&buffer, TEMPLATE_UNRELEASED);
            } else {
                release_buffer = str_buffer_init();
                char* condition = str_format("version = '%s'", entries[i].version.full);
                char* date_str = select_str(TABLE_RELEASES, RELEASES_DATE, condition);
                int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);
                free(condition);
                Date date = parse_date(date_str);
                char* line = TEMPLATE_RELEASE(entries[i].version.full, date.date, yanked);

                str_append_ln(&buffer, line);
            }
            str_append_ln(&release_buffer, "");
        }

        if(i == 0 || status != entries[i].status || !STREQ(version, entries[i].version.full)){
            str_append_ln(&buffer, "");
            str_append_ln(&release_buffer, "");
            str_append_ln(&buffer, TEMPLATE_STATUS(entries[i].status));
            str_append_ln(&release_buffer, TEMPLATE_STATUS(entries[i].status));
            str_append_ln(&buffer, "");
            str_append_ln(&release_buffer, "");
        }
        
        str_append_ln(&buffer, TEMPLATE_ENTRY(entries[i].message));
        if(!STREQ(entries[i].version.full, "0.0.0")) {
            str_append_ln(&release_buffer, TEMPLATE_ENTRY(entries[i].message));
        }

        status = entries[i].status;
        version = entries[i].version.full;


        char* file = str_format("%s/%s.md", CHANGELOG_DIR, version);
        fs_write_file(file, release_buffer, "w");
        free(file);
    }
    free(release_buffer);

    str_append_ln(&buffer, "");
    str_append_ln(&buffer, "");
    
    char* url = select_str(TABLE_CONFIG, CONFIG_REMOTE_REPO, CONFIG_CONDITION);
    if(!is_blank(url)){
        size_t versions_count;
        char** versions = select_releases_version(db, &versions_count);
        for(size_t i = 0; i < versions_count; ++i){
            char* release_url = str_format("%s/releases/tag/v%s", url, versions[i]);
            str_append_ln(&buffer, TEMPLATE_RELEASE_LINK(versions[i], release_url));
            free(release_url);
        }
        str_append_ln(&buffer, "");
    }
    free(url);

    fs_write_file(CHANGELOG_FILE, buffer, "w");
    free(buffer);
    sqlite3_close(db);

}
