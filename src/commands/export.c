#include "commands.h"
#include "config.h"
#include "database.h"
#include "parser.h"
#include "templates.h"
#include "utils.h"
#include <unistd.h>

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

    char* clparse = find_executable("clparse");
    if (clparse == NULL) {
        ERRO("clparse not found in PATH. Install it from https://github.com/marcaddeo/clparse");
        exit(1);
    }

    char* compatible = clparse_compatible_copy(CHANGELOG_FILE);
    if (compatible == NULL) {
        ERRO("Failed to read %s", CHANGELOG_FILE);
        free(clparse);
        exit(1);
    }

    char* command = str_format("%s -f %s %s", clparse, fmt, compatible);
    free(clparse);
    char* export_string = NULL;
    int status = execute_command_capture(command, &export_string);
    free(command);
    unlink(compatible);
    free(compatible);

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
    if (sqlite3_open(SQLITE_DB, &db) != SQLITE_OK) {
        ERRO("Failed to open %s: %s", SQLITE_DB, sqlite3_errmsg(db));
        free(buffer);
        sqlite3_close(db);
        exit(1);
    }

    size_t count;
    int failed = 0;
    Entry* entries = select_entries_order_by(db, "version DESC, status ASC, date DESC", &count, &failed);

    /*
     * A failed read must never touch the existing CHANGELOG.md: writing the
     * header only would silently discard the whole changelog.
     */
    if (failed) {
        ERRO("Failed to read the entries from %s: %s", SQLITE_DB, sqlite3_errmsg(db));
        free(buffer);
        sqlite3_close(db);
        exit(1);
    }

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

                /*
                 * The release row can be missing when an entry was moved to a
                 * version that was never released (edit -V) or when the
                 * database was modified by hand. Fall back to the entry date
                 * instead of calling parse_date(NULL).
                 */
                Date release_date = {0};
                const char* date = NULL;
                if (date_str != NULL) {
                    release_date = parse_date(date_str);
                    date = release_date.date;
                } else if (entries[i].date.full != NULL) {
                    release_date = parse_date(entries[i].date.full);
                    date = release_date.date;
                } else {
                    date = "unknown";
                }

                char* line = TEMPLATE_RELEASE(entries[i].version.full, date, yanked);

                str_append_ln(&buffer, line);

                free(line);
                free(date_str);
                free_date(&release_date);
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
