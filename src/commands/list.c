#include "commands.h"
#include "database.h"
#include "entry.h"
#include "options.h"
#include "status.h"
#include "utils.h"
#include <stddef.h>
#include <unistd.h>

void list_releases(Release* releases, size_t count);
void list_entries(Entry* entries, size_t count, Options options, char* condition);

void command_list(Options options)
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* condition = NULL;
    char* order_by= "date DESC";

    if(options.releases){
        size_t count = 0;
        Release* releases = select_releases(db, condition, order_by, &count);
        sqlite3_close(db);

        if (count == 0) {
            INFO("No entries found");
            exit(0);
        }
        list_releases(releases, count);
        return;
    }

    if(version_full_set(options) || status_set(options)) {
        condition = clib_buffer_init();
    }

    if(version_full_set(options)){
        char* version = clib_format_text("version = '%s'", (STREQ(options.version.full, "0.0.0")) ? VERSION_UNRELEASED : options.version.full);
        clib_str_append(&condition, version);
        free(version);
    }

    if(status_set(options)){
        if(version_full_set(options)) clib_str_append(&condition, " AND ");
        char* status = clib_format_text("status = %d", options.status);
        clib_str_append(&condition, status);
        free(status);
    }

    size_t count;
    Entry* entries = select_entries(db, condition, order_by, &count);
    sqlite3_close(db);

    if (count == 0) {
        INFO("No entries found");
        exit(0);
    }

    list_entries(entries, count, options, condition);
    if(condition != NULL)
        free(condition);
}

void list_releases(Release* releases, size_t count)
{
    if(!isatty(STDOUT_FILENO)) {
        for(size_t i = 0; i < count; ++i) {
            printf("%s\n", releases[i].version.full);
        }
        return;
    }

    // Define column offsets
    int index_offset = -5;
    int version_offset = -7;
    int date_offset = -19;
    int pushed_offset = -6;
    int yanked_offset = -6;

    // Define border characters
    const char* top_border_left = "┌";
    const char* top_border_right = "┐";
    const char* mid_border_left = "├";
    const char* mid_border_right = "┤";
    const char* bottom_border_left = "└";
    const char* bottom_border_right = "┘";
    const char* border_sep = "┼";

    // Generate dashes for each column
    char* index_dashes = char_repeat("─", -index_offset + 2);
    char* version_dashes = char_repeat("─", -version_offset + 2);
    char* date_dashes = char_repeat("─", -date_offset + 2);
    char* pushed_dashes = char_repeat("─", -pushed_offset + 2);
    char* yanked_dashes = char_repeat("─", -yanked_offset + 2);

    // Calculate the total length for each border
    size_t full_border_size = strlen(top_border_left) + strlen(index_dashes) +
                              strlen(border_sep) + strlen(version_dashes) +
                              strlen(border_sep) + strlen(pushed_dashes) +
                              strlen(border_sep) + strlen(yanked_dashes) +
                              strlen(border_sep) + strlen(date_dashes) +
                              strlen(top_border_right) + 1; // +1 for null terminator

    // Allocate and construct borders
    char* top_border = malloc(full_border_size);
    char* mid_border = malloc(full_border_size);
    char* bottom_border = malloc(full_border_size);

    snprintf(top_border, full_border_size, "%s%s┬%s┬%s┬%s┬%s%s",
             top_border_left, index_dashes, version_dashes, pushed_dashes,
             yanked_dashes, date_dashes, top_border_right);

    snprintf(mid_border, full_border_size, "%s%s┼%s┼%s┼%s┼%s%s",
             mid_border_left, index_dashes, version_dashes, pushed_dashes,
             yanked_dashes, date_dashes, mid_border_right);

    snprintf(bottom_border, full_border_size, "%s%s┴%s┴%s┴%s┴%s%s",
             bottom_border_left, index_dashes, version_dashes, pushed_dashes,
             yanked_dashes, date_dashes, bottom_border_right);

    // Print the table
    printf("%s\n", top_border); // Top border
    printf("│ %*s │ %*s │ %*s │ %*s │ %*s │\n",
           index_offset, "Index",
           version_offset, "Version",
           pushed_offset, "Pushed",
           yanked_offset, "Yanked",
           date_offset, "Date");
    printf("%s\n", mid_border); // Middle border

    for (size_t i = 0; i < count; ++i) {
        printf("│ %*zu │ %*s │ %*s │ %*s │ %*s │\n",
               index_offset, i + 1,
               version_offset, (STREQ(releases[i].version.full, "0.0.0")) ? VERSION_UNRELEASED : releases[i].version.full,
               pushed_offset, releases[i].pushed ? "True" : "False",
               yanked_offset, releases[i].yanked ? "True" : "False",
               date_offset, releases[i].date.full);
    }

    printf("%s\n", bottom_border); // Bottom border

    // Clean up
    free(releases);
    free(index_dashes);
    free(version_dashes);
    free(date_dashes);
    free(pushed_dashes);
    free(yanked_dashes);
    free(top_border);
    free(mid_border);
    free(bottom_border);
}

void free_entries(Entry** entries, size_t count)
{
    Entry* e = *entries;
    for(size_t i = 0; i < count; ++i){
        if(e[i].message != NULL)
            freec(e[i].message);
        free_version(&e[i].version);
        free_date(&e[i].date);
    }
    free(*entries);
}

#define DEFAULT_MAX_ENTRIES 15
void list_entries(Entry* entries, size_t count, Options options, char* condition)
{
    if(!isatty(STDOUT_FILENO)) {
        for(size_t i = 0; i < count; ++i) {
            Entry e = entries[i];
            printf("%s\t%s\t%s\t%s\n", e.message, status_to_string(e.status), e.version.full, e.date.full);
        }
        return;
    }

    // Define column offsets (TODO: check largest length for each column)
    int index_offset = -5;
    int title_offset = -60;
    int status_offset = -10;
    int version_offset = -10;
    int date_offset = -19;

    // Define border characters
    const char* top_border_left = "┌";
    const char* top_border_right = "┐";
    const char* mid_border_left = "├";
    const char* mid_border_right = "┤";
    const char* bottom_border_left = "└";
    const char* bottom_border_right = "┘";
    const char* border_sep = "┼";

    const char* dash = "─";
    char* index_dashes = char_repeat(dash, -index_offset + 2);
    char* title_dashes = char_repeat(dash, -title_offset + 2);
    char* status_dashes = char_repeat(dash, -status_offset + 2);
    char* version_dashes = char_repeat(dash, -version_offset + 2);
    char* date_dashes = char_repeat(dash, -date_offset + 2);

    // Calculate the total border size
    size_t full_border_size = 
        strlen(top_border_left) + strlen(index_dashes) +
        strlen(border_sep) + strlen(title_dashes) +
        strlen(border_sep) + strlen(status_dashes) +
        strlen(border_sep) + strlen(version_dashes) +
        strlen(border_sep) + strlen(date_dashes) +
        strlen(top_border_right) + 1; // +1 for null terminator

    // Allocate memory for borders
    char* top_border = malloc(full_border_size);
    char* mid_border = malloc(full_border_size);
    char* bottom_border = malloc(full_border_size);

    // Construct borders with proper null termination
    snprintf(top_border, full_border_size, "%s%s┬%s┬%s┬%s┬%s%s",
             top_border_left, index_dashes, title_dashes, status_dashes,
             version_dashes, date_dashes, top_border_right);
    snprintf(mid_border, full_border_size, "%s%s┼%s┼%s┼%s┼%s%s",
             mid_border_left, index_dashes, title_dashes, status_dashes,
             version_dashes, date_dashes, mid_border_right);
    snprintf(bottom_border, full_border_size, "%s%s┴%s┴%s┴%s┴%s%s",
             bottom_border_left, index_dashes, title_dashes, status_dashes,
             version_dashes, date_dashes, bottom_border_right);

    // Print the table
    printf("%s\n", top_border); // Top border
    printf("│ %*s │ %*s │ %*s │ %*s │ %*s │\n",
           index_offset, "Index",
           title_offset, "Title",
           status_offset, "Status",
           version_offset, "Version",
           date_offset, "Date");
    printf("%s\n", mid_border); // Middle border

    for (size_t i = 0; i < ((options.all || condition || count < DEFAULT_MAX_ENTRIES) ? count : DEFAULT_MAX_ENTRIES); ++i) {
        printf("│ %*zu │ %*s │ %*s │ %*s │ %*s │\n",
               index_offset, i + 1,
               title_offset, entries[i].message,
               status_offset, status_to_string(entries[i].status),
               version_offset, (STREQ(entries[i].version.full, "0.0.0")) ? VERSION_UNRELEASED : entries[i].version.full,
               date_offset, entries[i].date.full);
    }

    printf("%s\n", bottom_border); // Bottom border

    // Clean up
    free_entries(&entries, count);
    free(index_dashes);
    free(title_dashes);
    free(status_dashes);
    free(version_dashes);
    free(date_dashes);
    free(top_border);
    free(mid_border);
    free(bottom_border);
}
