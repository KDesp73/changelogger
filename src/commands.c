#include "config.h"
#include "database.h"
#include "date.h"
#include "entry.h"
#include "help.h"
#include "options.h"
#include "extern/sqlite.h"
#include "release.h"
#include "status.h"
#include "templates.h"
#include "version.h"
#include <stdio.h>
#include <strings.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "utils.h"
#include "commands.h"
#include "extern/querybuilder.h"

void command_init(Options options)
{
    if(!clib_directory_exists(CHANGELOG_DIR)) {
        clib_create_directory(CHANGELOG_DIR);
    } else INFO("%s/ is located in this directory", CHANGELOG_DIR);
    if(!clib_file_exists(SQLITE_DB)) {
        sqlite_create_database(SQLITE_DB);
        sqlite_execute_sql(SQLITE_DB, GENERATION_QUERY);
    } else INFO("%s is already created", SQLITE_DB);
    if(!config_exists()){
        query_builder_t* qb = create_query_builder();
        insert_q(qb, TABLE_CONFIG);
        columns_q(qb, "config_path, version_major, version_minor, version_patch");
        values_q(qb, "'', 0, 0, 0"); // "Starting with version 0.0.0"
        char* query = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, query);
        free(query);
    } else INFO("Config is already initialized");
    
}

void command_add(Options options)
{
    char* message = options.argv[options.argc-1];

    if(STREQ(message, command_to_string(COMMAND_ADD))) 
        PANIC("Message is not specified. Try: `%s add \"Your message\"`", EXECUTABLE_NAME);
    if(is_blank(message))
        PANIC("Message cannot be empty or blank");
    

    Date date;
    get_date(&date);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_ENTRIES);
    columns_q(qb, "message, status, version, date");
    char* values = clib_format_text("'%s', %d, '%s', '%s'", message, options.status, VERSION_UNRELEASED, date.full);
    values_q(qb, values);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);

    free_date(&date);
    free(values);
    free(query);
}

void command_delete(Options options)
{
   Options list_options = {
        .version.full = VERSION_UNRELEASED,
        .argc = !options.all,
        .argv = (char**) malloc(sizeof(char*) * 4)
    };

    if (list_options.argv == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    list_options.argv[0] = malloc(strlen("-V") + 1);
    if (list_options.argv[0] == NULL) {
        fprintf(stderr, "Memory allocation for argv[0] failed\n");
        free(list_options.argv);
        return;
    }
    strcpy(list_options.argv[0], "-V");

    command_list(list_options);

    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);

    size_t count;
    Entry* entries = select_entries(db, options.all ? NULL : "version = 'unreleased'", "date DESC", &count);
    sqlite3_close(db);

    int index = -1;
    char index_str[10];
    printf("\n");
    while (1) {
        printf("Enter index to delete: ");
        if (scanf("%9s", index_str) != 1) {
            ERRO("Error reading input.\n");
            clear_input_buffer();
            continue;
        }

        if (!is_number(index_str)) {
            ERRO("'%s' is not a number\n", index_str);
            clear_input_buffer();
            continue;
        }

        index = atoi(index_str);

        if(index < 1 || index > count) {
            if(count == 1) ERRO("index can only be 1\n");
            else ERRO("index must be between 1 and %zu\n", count);
            continue;
        }

        break;
    }
    char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
    query_builder_t* qb = create_query_builder();
    delete_q(qb, TABLE_ENTRIES);
    where_q(qb, condition);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);
    free(query);
    free(condition);
    free(qb);
}

char* update_config_version(const char* release_type)
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    if(STREQ(release_type, "major")){
        // Reset minor and patch versions
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, "0", CONFIG_CONDITION);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, "0", CONFIG_CONDITION);

        size_t major = select_version_major(db);
        char* major_inc = clib_format_text("%zu", major+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MAJOR, major_inc, CONFIG_CONDITION);
        free(major_inc);
    }

    if(STREQ(release_type, "minor")) {
        // Reset patch version
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, "0", CONFIG_CONDITION);

        size_t minor = select_version_minor(db);
        char* minor_inc = clib_format_text("%zu", minor+1);
        update(TABLE_CONFIG, CONFIG_VERSION_MINOR, minor_inc, CONFIG_CONDITION);
        free(minor_inc);
    }

    if(STREQ(release_type, "patch")) {
        size_t patch = select_version_patch(db);
        char* patch_inc = clib_format_text("%zu", patch+1);
        update(TABLE_CONFIG, CONFIG_VERSION_PATCH, patch_inc, CONFIG_CONDITION);
        free(patch_inc);
    }

    char* version = select_version_full(db);
    sqlite3_close(db);
    return version;
}

void insert_release()
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* version = select_version_full(db);
    sqlite3_close(db);

    Date date;
    get_date(&date);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_RELEASES);
    columns_q(qb, FIELDS_RELEASES);
    char* values = clib_format_text("'%s', '%s'", version, date.full);
    values_q(qb, values);
    char* query = build_query(qb);
    
    sqlite_execute_sql(SQLITE_DB, query);

    free(query);
    free(values);
}

void command_export(Options options)
{
    INFO("Exporting %s...", CHANGELOG_FILE);
    char* buffer = clib_buffer_init();
    clib_str_append_ln(&buffer, TEMPLATE_HEADER);
    clib_str_append_ln(&buffer, TEMPLATE_NOTE);
    
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    size_t count;
    Entry* entries = select_entries_order_by(db, "version DESC, status ASC, date DESC", &count);
    if(entries == NULL) {
        INFO("Nothing to export");
        clib_write_file(CHANGELOG_FILE, buffer, "w");
        free(buffer);
        sqlite3_close(db);
        exit(0);
    }
    char* version = entries[0].version.full;
    Status status = entries[0].status;
    char* release_buffer = clib_buffer_init();

    for(size_t i = 0; i < count; ++i){
        if(i == 0 || !STREQ(version, entries[i].version.full)){
            clib_str_append_ln(&buffer, "");
            if(STREQ(entries[i].version.full, "0.0.0")){
                clib_str_append_ln(&buffer, TEMPLATE_UNRELEASED);
            } else {
                release_buffer = clib_buffer_init();
                char* condition = clib_format_text("version = '%s'", entries[i].version.full);
                char* date_str = select_str(TABLE_RELEASES, RELEASES_DATE, condition);
                free(condition);
                Date date = parse_date(date_str);
                char* line = TEMPLATE_RELEASE(entries[i].version.full, date.date);

                clib_str_append_ln(&buffer, line);
            }
            clib_str_append_ln(&release_buffer, "");
        }

        if(i == 0 || status != entries[i].status || !STREQ(version, entries[i].version.full)){
            clib_str_append_ln(&buffer, "");
            clib_str_append_ln(&release_buffer, "");
            clib_str_append_ln(&buffer, TEMPLATE_STATUS(entries[i].status));
            clib_str_append_ln(&release_buffer, TEMPLATE_STATUS(entries[i].status));
            clib_str_append_ln(&buffer, "");
            clib_str_append_ln(&release_buffer, "");
        }
        
        clib_str_append_ln(&buffer, TEMPLATE_ENTRY(entries[i].message));
        if(!STREQ(entries[i].version.full, "0.0.0")) {
            clib_str_append_ln(&release_buffer, TEMPLATE_ENTRY(entries[i].message));
        }

        status = entries[i].status;
        version = entries[i].version.full;


        char* file = clib_format_text("%s/%s.md", CHANGELOG_DIR, version);
        clib_write_file(file, release_buffer, "w");
        free(file);
    }
    free(release_buffer);

    clib_str_append_ln(&buffer, "");
    clib_str_append_ln(&buffer, "");
    
    char* url = select_str(TABLE_CONFIG, CONFIG_REMOTE_REPO, CONFIG_CONDITION);
    if(url != NULL){
        size_t versions_count;
        char** versions = select_releases_version(db, &versions_count);
        for(size_t i = 0; i < versions_count; ++i){
            char* release_url = clib_format_text("%s/releases/tag/v%s", url, versions[i]);
            clib_str_append_ln(&buffer, TEMPLATE_RELEASE_LINK(versions[i], release_url));
            free(release_url);
        }
        clib_str_append_ln(&buffer, "");
    }
    free(url);

    clib_write_file(CHANGELOG_FILE, buffer, "w");
    free(buffer);
    sqlite3_close(db);

    INFO("Export complete.");
}


void command_edit(Options options)
{
    Options list_options = {
        .version.full = VERSION_UNRELEASED,
        .argc = !options.all,
        .argv = (char**) malloc(sizeof(char*) * 4)
    };

    if (list_options.argv == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    list_options.argv[0] = malloc(strlen("-V") + 1);
    if (list_options.argv[0] == NULL) {
        fprintf(stderr, "Memory allocation for argv[0] failed\n");
        free(list_options.argv);
        return;
    }
    strcpy(list_options.argv[0], "-V");

    command_list(list_options);

    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);

    size_t count;
    Entry* entries = select_entries(db, options.all ? NULL : "version = 'unreleased'", "date DESC", &count);
    sqlite3_close(db);


    int index = -1;
    char index_str[10];
    printf("\n");
    while (1) {
        printf("Enter index to edit: ");
        if (scanf("%9s", index_str) != 1) {
            ERRO("Error reading input.\n");
            clear_input_buffer();
            continue;
        }

        if (!is_number(index_str)) {
            ERRO("'%s' is not a number\n", index_str);
            clear_input_buffer();
            continue;
        }

        index = atoi(index_str);

        if(index < 1 || index > count) {
            if(count == 1) ERRO("index can only be 1\n");
            else ERRO("index must be between 1 and %zu\n", count);
            continue;
        }

        break;
    }
    
    _Bool arguments_used = status_set(options) || options.title != NULL;
    if(arguments_used){
        if(options.title != NULL){
            char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
            char* formated_message = clib_format_text("'%s'", options.title);
            update(TABLE_ENTRIES, ENTRIES_MESSAGE, formated_message, condition);
            free(formated_message);
            free(condition);
        }
        
        if(status_set(options)){
            char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
            char* formated_status = clib_format_text("%d", options.status);
            update(TABLE_ENTRIES, ENTRIES_STATUS, formated_status, condition);
            free(formated_status);
            free(condition);
        }
        return;
    }

    char choice[2];
    while (1) {
        printf("Select value to edit {(t)itle | (s)tatus}: ");
        if (scanf("%1s", choice) != 1) {
            ERRO("Error reading input.\n");
            clear_input_buffer();
            continue;
        }

        if (choice[0] == 't' || choice[0] == 's') {
            break;
        } else {
            ERRO("'%s' is not a valid option. Please enter 'm' or 's'.\n", choice);
            clear_input_buffer();
        }
    }

    char message[50];
    Status s = STATUS_UNSET;
    if(choice[0] == 't'){
        clear_input_buffer();
        while(1){
            printf("Enter new message: ");
            if (fgets(message, sizeof(message), stdin) == NULL) {
                fprintf(stderr, "Error reading input.\n");
                continue;
            }

            message[strcspn(message, "\n")] = '\0';

            if (is_blank(message)) {
                ERRO("New message cannot be blank\n");
                clear_input_buffer();
                continue;
            }
            break;
        }

        char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
        char* formated_message = clib_format_text("'%s'", message);
        update(TABLE_ENTRIES, ENTRIES_MESSAGE, formated_message, condition);
        free(formated_message);
        free(condition);
    } else if(choice[0] == 's'){
        char status[10];
        clear_input_buffer();
        while(1){
            printf("Enter new status: ");
            if (fgets(status, sizeof(status), stdin) == NULL) {
                fprintf(stderr, "Error reading input.\n");
                continue;
            }

            status[strcspn(status, "\n")] = '\0';

            if (is_blank(status)) {
                ERRO("New status cannot be blank\n");
                clear_input_buffer();
                continue;
            }
            s = get_status(status);
            if(s == STATUS_NONE || s == STATUS_UNSET || s == STATUS_UNKNOWN){
                ERRO("Invalid status: '%s'\n", status);
                continue;
            }
            break;
        }

        char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
        char* formated_status = clib_format_text("%d", s);
        update(TABLE_ENTRIES, ENTRIES_STATUS, formated_status, condition);
        free(formated_status);
        free(condition);
    }
}

void command_get(Options options)
{
    char* key = options.argv[options.argc-1];
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);

    if(
        !STREQ(key, "version") &&
        !STREQ(key, "config") &&
        !STREQ(key, "remote") &&
        !STREQ(key, "push") &&
        !STREQ(key, "export")
    ) { 
        PANIC("Invalid key: '%s'. Try %s get -h", key, EXECUTABLE_NAME);
    }

    if(STREQ(key, "version")){
        char* version = select_version_full(db);
        printf("%s\n", version);
    } else if(STREQ(key, "config")) {
        char* config_path = select_config_path(db);
        printf("%s\n", (config_path == NULL) ? "" : config_path);
    } else if(STREQ(key, "remote")) {
        char* remote = SELECT_CONFIG_REMOTE;
        printf("%s\n", (remote == NULL) ? "" : remote);
    } else if(STREQ(key, "push")) {
        int push = SELECT_CONFIG_PUSH;
        printf("%d\n", push);
    } else if(STREQ(key, "export")) {
        int export = SELECT_CONFIG_EXPORT;
        printf("%d\n", export);
    }
        
    sqlite3_close(db);
}

void command_release(Options options)
{
    if(options.new == NULL) {
        ERRO("Options other than '--new' are not implemented or accepted");
        return;
    }

    const char* release_type = options.new;

    if(
        !STREQ(release_type, "major") &&
        !STREQ(release_type, "minor") &&
        !STREQ(release_type, "patch")
    ){
        PANIC("Release type '%s' should be 'major', 'minor' or 'patch'. Try %s release -h", release_type, EXECUTABLE_NAME);
    }

    char* version = update_config_version(release_type);
    insert_release();
    char* query = clib_format_text("UPDATE Entries SET version = '%s' WHERE version = 'unreleased'", version);
    sqlite_execute_sql(SQLITE_DB, query);

    command_export(options);

    
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    _Bool release = select_always_export(db);
    sqlite3_close(db);

    if(release || options.push) {
        char* gh_command = clib_format_text("gh release create v%s -F %s/%s.md -t v%s", version, CHANGELOG_DIR, version, version);
        clib_execute_command(gh_command);
        free(gh_command);
    }
}

void list_releases(sqlite3* db, Options options, char* condition, char* order_by)
{
    size_t count = 0;
    Release* releases = select_releases(db, condition, order_by, &count);
    sqlite3_close(db);
    if(condition != NULL) free(condition);

    if(count == 0){
        INFO("No entries found");
        exit(0);
    }

    int index_offset = -5;
    int version_offset = -7;
    int date_offset = -19;
    int pushed_offset = -6;
    char* index_dashes = char_repeat('-', -index_offset + 2); // +2 for left and right padding
    char* version_dashes = char_repeat('-', -version_offset + 2);
    char* date_dashes = char_repeat('-', -date_offset + 2);
    char* pushed_dashes = char_repeat('-', -pushed_offset + 2);

    printf("| Index | %*s | %*s | %*s |\n", version_offset, "Version", pushed_offset, "Pushed", date_offset, "Date");
    printf("|%s+%s+%s+%s|\n", 
            index_dashes,
            version_dashes,
            pushed_dashes,
            date_dashes
          );

    for(size_t i = 0; i < count; ++i){
        printf(
                "| %*zu | %*s | %*d | %*s |\n",
                index_offset,
                i+1, 
                version_offset,
                (STREQ(releases[i].version.full, "0.0.0")) ? VERSION_UNRELEASED : releases[i].version.full, 
                pushed_offset,
                releases[i].pushed,
                date_offset,
                releases[i].date.full
              );
    }

    free(releases);
    free(index_dashes);
    free(pushed_dashes);
    free(version_dashes);
    free(date_dashes);
}

void list_entries(sqlite3* db, Options options, char* condition, char* order_by)
{
    size_t count;
    Entry* entries = select_entries(db, condition, order_by, &count);
    sqlite3_close(db);
    if(condition != NULL) free(condition);

    if(count == 0){
        INFO("No entries found");
        exit(0);
    }

    int index_offset = -5;
    int title_offset = -50;
    int status_offset = -10;
    int version_offset = -10;
    int date_offset = -19;
    char* index_dashes = char_repeat('-', -index_offset + 2); // +2 for left and right padding
    char* title_dashes = char_repeat('-', -title_offset + 2);
    char* status_dashes = char_repeat('-', -status_offset + 2);
    char* version_dashes = char_repeat('-', -version_offset + 2);
    char* date_dashes = char_repeat('-', -date_offset + 2);

    printf("| Index | %*s | %*s | %*s | %*s |\n", title_offset, "Title", status_offset, "Status", version_offset, "Version", date_offset, "Date");
    printf("|%s+%s+%s+%s+%s|\n", 
            index_dashes,
            title_dashes,
            status_dashes,
            version_dashes,
            date_dashes
          );

    for(size_t i = 0; i < count; ++i){
        printf(
                "| %*zu | %*s | %*s | %*s | %*s |\n", 
                index_offset,
                i+1, 
                title_offset,
                entries[i].message, 
                status_offset,
                status_to_string(entries[i].status), 
                version_offset,
                (STREQ(entries[i].version.full, "0.0.0")) ? VERSION_UNRELEASED : entries[i].version.full, 
                date_offset,
                entries[i].date.full
              );
    }

    free(entries);
    free(index_dashes);
    free(title_dashes);
    free(status_dashes);
    free(version_dashes);
    free(date_dashes);
}

void command_list(Options options)
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* condition = NULL;
    char* order_by= "date DESC";

    if(options.releases){
        list_releases(db, options, condition, order_by);
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

    list_entries(db, options, condition, order_by);
}

void command_set(Options options)
{
    if(always_export_set(options)){
        char* value = clib_format_text("%d", options.always_export);
        update(TABLE_CONFIG, CONFIG_ALWAYS_EXPORT, value, CONFIG_CONDITION);
        free(value);
    }

    if(always_push_set(options)){
        char* value = clib_format_text("%d", options.always_push);
        update(TABLE_CONFIG, CONFIG_ALWAYS_PUSH, value, CONFIG_CONDITION);
        free(value);
    }

    if(!is_blank(options.config_path)){
        char* value = clib_format_text("'%s'", options.config_path);
        update(TABLE_CONFIG, CONFIG_CONFIG_PATH, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.config_path != NULL) // Only when it set blank by the user
            ERRO("Config path must not be blank");
    }

    if(!is_blank(options.remote_repo)) {
        char* value = clib_format_text("'%s'", options.remote_repo);
        update(TABLE_CONFIG, CONFIG_REMOTE_REPO, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.config_path != NULL) // Only when it set blank by the user
            ERRO("Remote repo must not be blank");
    }
}


Command get_command(char* command)
{
#define COMPARE_AND_RETURN_COMMAND(c) \
    else if(STREQ(command, command_to_string(c))) return c;

    if(command == NULL) return COMMAND_UNSET;
    COMPARE_AND_RETURN_COMMAND(COMMAND_EXPORT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_ADD)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_INIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_SET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_LIST)
    COMPARE_AND_RETURN_COMMAND(COMMAND_DELETE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_RELEASE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_EDIT)
    else return COMMAND_UNKNOWN;

#undef COMPARE_AND_RETURN_COMMAND
}

void execute_command(Command command, Options options)
{
    switch (command) {
    case COMMAND_UNKNOWN:
        ERRO("Unknown command: %s", options.argv[1]);
        break;
    case COMMAND_UNSET:
        ERRO("Command not found");
        break;
    case COMMAND_INIT:
        command_init(options);
        return;
    case COMMAND_ADD:
        command_add(options);
        return;
    case COMMAND_SET:
        command_set(options);
        return;
    case COMMAND_LIST:
        command_list(options);
        return;
    case COMMAND_DELETE:
        command_delete(options);
        return;
    case COMMAND_RELEASE:
        command_release(options);
        return;
    case COMMAND_EXPORT:
        command_export(options);
        return;
    case COMMAND_GET:
        command_get(options);
        return;
    case COMMAND_EDIT:
        command_edit(options);
        return;
    }

    help();
}

char* command_to_string(Command command)
{
    switch (command) {
    case COMMAND_UNSET:
    case COMMAND_UNKNOWN:
        return "";
    case COMMAND_INIT:
        return "init";
    case COMMAND_ADD:
        return "add";
    case COMMAND_LIST:
        return "list";
    case COMMAND_SET:
        return "set";
    case COMMAND_DELETE:
        return "delete";
    case COMMAND_RELEASE:
        return "release";
    case COMMAND_GET:
        return "get";
    case COMMAND_EXPORT:
        return "export";
    case COMMAND_EDIT:
        return "edit";
      break;
    }

    return "";
}
