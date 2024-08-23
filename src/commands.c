#include "config.h"
#include "database.h"
#include "date.h"
#include "entry.h"
#include "help.h"
#include "options.h"
#include "extern/sqlite.h"
#include "parser.h"
#include "release.h"
#include "status.h"
#include "templates.h"
#include "version.h"
#include <stdio.h>
#include <strings.h>
#include <sys/wait.h>
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
        columns_q(qb, "config_path, version_major, version_minor, "
                      "version_patch, always_export, always_push, remote_repo");
        values_q(qb, "'', 0, 0, 0, 0, 0, ''"); // "Starting with version 0.0.0"
        char* query = build_query(qb);
        sqlite_execute_sql(SQLITE_DB, query);
        free(query);
    } else INFO("Config is already initialized");
}

char* extract_commit_messages(char* input) {
    char* output = malloc(strlen(input) + 1); 
    output[0] = '\0'; 

    char* line = strtok(input, "\n");
    while (line != NULL) {
        if (strncmp(line, "      ", 6) == 0) { 
            strcat(output, line + 6); 
            strcat(output, "\n");
        }
        line = strtok(NULL, "\n");
    }

    return output;
}

int open_editor(char* editor)
{
    pid_t pid = fork();
    if (pid == 0) {
        execlp(editor, editor, TEMP_FILE, NULL);
        perror("Error executing editor");
        return 1;
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) return 1;
        } else {
            return 1;
        }
    } else {
        perror("Error forking process");
        return 1;
    }

    return 0;
}

void add_commits()
{
    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    char* latest = select_version_full(db);
    sqlite3_close(db);
    char* git_command = NULL;
    
    // Check if tag exists
    char* tag_command = clib_format_text("git pull --quiet && git tag --list v%s", latest);
    char* versions = clib_execute_command(tag_command);
    free(tag_command);
    if(is_blank(versions)){
        ERRO("v%s is not pushed", latest);
        Version v = {
            .full = latest
        };
        parse_version(&v);
        v.patch = v.patch-1;
        make_version(&v);
        INFO("Setting v%s as the previous release", v.full);
        latest = v.full;
    }
    free(versions);


    if(STREQ(latest, "0.0.0")){
        git_command = clib_format_text("git shortlog -z");
    } else {
        git_command = clib_format_text("git shortlog -z v%s..HEAD", latest);
    }

    char* out = clib_execute_command(git_command);

    if(is_blank(out)){
        ERRO("No commit messages after v%s", latest);
        exit(1);
    }

    char* formatted_out = extract_commit_messages(out);
    free(out);

    char* buffer = clib_buffer_init();

    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_ADDED));
    clib_str_append_ln(&buffer, formatted_out);
    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_CHANGED));
    clib_str_append_ln(&buffer, "");
    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_REMOVED));
    clib_str_append_ln(&buffer, "");
    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_FIXED));
    clib_str_append_ln(&buffer, "");
    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_DEPRECATED));
    clib_str_append_ln(&buffer, "");
    clib_str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_SECURITY));
    clib_str_append_ln(&buffer, "");

    clib_write_file(TEMP_FILE, buffer, "w");
    free(buffer);

    char* editor = SELECT_CONFIG_EDITOR;
    int status = open_editor(editor);
    free(editor);
    if(status != 0) PANIC("Aborting adding commits...");

    if(!yes_or_no("Continue?")){
        INFO("Aborting adding commits...");
        remove(TEMP_FILE);
        exit(0);
    }

    char* user_edited_commits = clib_read_file(TEMP_FILE, "r");
    remove(TEMP_FILE);

    char* line = user_edited_commits;
    Status current_status = STATUS_ADDED;
    while (line != NULL && *line != '\0') {
        char* end = strchr(line, '\n');
        if (end != NULL) {
            *end = '\0';
        }

        if(STREQ(line, TEMPLATE_STATUS(STATUS_ADDED))){
            current_status = STATUS_ADDED;
        } else if(STREQ(line, TEMPLATE_STATUS(STATUS_CHANGED))){
            current_status = STATUS_CHANGED;
        } else if(STREQ(line, TEMPLATE_STATUS(STATUS_REMOVED))){
            current_status = STATUS_REMOVED;
        } else if(STREQ(line, TEMPLATE_STATUS(STATUS_FIXED))){
            current_status = STATUS_FIXED;
        } else if(STREQ(line, TEMPLATE_STATUS(STATUS_DEPRECATED))){
            current_status = STATUS_DEPRECATED;
        } else if(STREQ(line, TEMPLATE_STATUS(STATUS_SECURITY))){
            current_status = STATUS_SECURITY;
        } else { // Line is a commit message
            if(!is_blank(line) && !STREQ(line, "\n")){
                add_entry(line, current_status);
            }
        }

        line = (end != NULL) ? end + 1 : NULL;
    }
    free(formatted_out);
    free(user_edited_commits);
}

void add_entry(const char* message, Status status)
{
    CHECK_SQL_INJECTION(message);

    Date date;
    get_date(&date);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_ENTRIES);
    columns_q(qb, "message, status, version, date");
    char* values = clib_format_text("'%s', %d, '%s', '%s'", message, status, VERSION_UNRELEASED, date.full);
    values_q(qb, values);
    char* query = build_query(qb);
    sqlite_execute_sql(SQLITE_DB, query);

    free_date(&date);
    free(values);
    free(query);
}

void command_add(Options options)
{
    if(options.commits){
        add_commits();
        return;
    }

    char* message = options.argv[options.argc-1];

    if(STREQ(message, command_to_string(COMMAND_ADD))) 
        PANIC("Message is not specified. Try: `%s add \"Your message\"`", EXECUTABLE_NAME);
    if(is_blank(message))
        PANIC("Message cannot be empty or blank");
    
    add_entry(message, options.status);
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

void insert_release(_Bool pushed)
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
    char* values = clib_format_text("'%s', '%s', %d", version, date.full, pushed);
    values_q(qb, values);
    char* query = build_query(qb);
    
    sqlite_execute_sql(SQLITE_DB, query);

    free(query);
    free(values);
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
    qsort(entries, count, sizeof(Entry), compare_entries);

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
                int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);
                free(condition);
                Date date = parse_date(date_str);
                char* line = TEMPLATE_RELEASE(entries[i].version.full, date.date, yanked);

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

}

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

    char* command = clib_format_text("/usr/bin/clparse -f %s %s", fmt, CHANGELOG_FILE);
    char* export_string = clib_execute_command(command);
    printf("%s\n", export_string);
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
    
    _Bool arguments_used = status_set(options) || options.title != NULL || version_full_set(options);
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

        if(version_full_set(options)){
            char* condition = clib_format_text("message = '%s' AND date = '%s'", entries[index-1].message, entries[index-1].date.full);
            char* formated_version = clib_format_text("'%s'", options.version.full);
            update(TABLE_ENTRIES, ENTRIES_VERSION, formated_version, condition);
            free(formated_version);
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
            CHECK_SQL_INJECTION(message);
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
        !options.all &&
        !STREQ(key, "version") &&
        !STREQ(key, "config") &&
        !STREQ(key, "remote") &&
        !STREQ(key, "push") &&
        !STREQ(key, "editor") &&
        !STREQ(key, "export")
    ) { 
        PANIC("Invalid key: '%s'. Try %s get -h", key, EXECUTABLE_NAME);
    }

    if(options.all || STREQ(key, "version")){
        char* version = select_version_full(db);
        if(options.all) printf("Version: ");
        printf("%s\n", version);
    }
    if(options.all || STREQ(key, "config")) {
        char* config_path = select_config_path(db);
        if(options.all) printf("Config: ");
        printf("%s\n", (config_path == NULL) ? CHANGELOGGER_DEFAULT_CONFIG_PATH : config_path);
    } 
    if(options.all || STREQ(key, "remote")) {
        char* remote = SELECT_CONFIG_REMOTE;
        if(options.all) printf("Remote: ");
        printf("%s\n", (remote == NULL) ? "" : remote);
    } 
    if(options.all || STREQ(key, "push")) {
        int push = SELECT_CONFIG_PUSH;
        if(options.all) printf("Always push: ");
        printf("%d\n", push);
    } 
    if(options.all || STREQ(key, "export")) {
        int export = SELECT_CONFIG_EXPORT;
        if(options.all) printf("Always export: ");
        printf("%d\n", export);
    } 
    if(options.all || STREQ(key, "editor")) {
        char* editor = SELECT_CONFIG_EDITOR;
        if(options.all) printf("Editor: ");
        printf("%s\n", editor);
    }

    sqlite3_close(db);
}

#define DEFAULT_WARNING_MESSAGE \
    "Remember to commit and update " \
    "all the necessary parts of the " \
    "project before pushing the release."

void make_sure_user_wants_to_proceed_with_releasing(Options options)
{
    if(!options.yes) {
        Config config = get_config();
        WARN("%s", (config.release_warning_message == NULL) ? DEFAULT_WARNING_MESSAGE : config.release_warning_message);
        int y_n = yes_or_no("Continue?");

        if(!y_n) {
            INFO("Aborting...");
            exit(0);
        }
    }
}

void push_release(const char* version, Options options)
{
    command_export((Options){0});
    char* gh_command = clib_format_text("gh release create v%s -F %s/%s.md -t v%s", version, CHANGELOG_DIR, version, version);
    clib_execute_command(gh_command);
    free(gh_command);
}

void command_push(Options options)
{
    if(version_full_set(options)){
        char* version = options.version.full;
        char* condition = clib_format_text("version = '%s'", version);
        int pushed = select_int(TABLE_RELEASES, RELEASES_PUSHED, condition);

        if(pushed) {
            WARN("This release is already pushed. Aborting.");
            free(condition);
            return;
        }

        make_sure_user_wants_to_proceed_with_releasing(options);
        push_release(version, options);
        update(TABLE_RELEASES, RELEASES_PUSHED, "1", condition);
        free(condition);
    } else {
        ERRO("Version is not set.");
        INFO("Try: %s push -V <version>", EXECUTABLE_NAME);
    }
}

void command_release(Options options)
{
    if(options.new != NULL && options.yank != NULL){
        PANIC("--new and --yank cannot be used together");
    }

    if(options.new != NULL){
        const char* release_type = options.new;
        if(
                !STREQ(release_type, "major") &&
                !STREQ(release_type, "minor") &&
                !STREQ(release_type, "patch")
          ){
            PANIC("Release type '%s' should be 'major', 'minor' or 'patch'. Try %s release -h", release_type, EXECUTABLE_NAME);
        }

        make_sure_user_wants_to_proceed_with_releasing(options);

        sqlite3* db;
        sqlite3_open(SQLITE_DB, &db);
        _Bool release = select_always_push(db);
        sqlite3_close(db);

        _Bool should_push = release || options.push;

        char* version = update_config_version(release_type);
        insert_release(should_push);
        char* query = clib_format_text("UPDATE Entries SET version = '%s' WHERE version = 'unreleased'", version);
        sqlite_execute_sql(SQLITE_DB, query);


        if(!should_push) return; // Do not push the release on Github

        push_release(version, options);
        return;
    }

    if(options.yank != NULL){
        char* version = (char*)options.yank;
        char* condition = clib_format_text("version = '%s'", version);
        int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);

        if(yanked) {
            WARN("Release v%s is already yanked", version);
            free(condition);
            exit(0);
        }

        update(TABLE_RELEASES, RELEASES_YANKED, "1", condition);
        free(condition);
        INFO("Release v%s was set as YANKED", version);
    }

    if(options.unyank != NULL){
        char* version = (char*)options.unyank;
        char* condition = clib_format_text("version = '%s'", version);
        int yanked = select_int(TABLE_RELEASES, RELEASES_YANKED, condition);

        if(!yanked) {
            WARN("Release v%s is already not yanked", version);
            free(condition);
            exit(0);
        }

        update(TABLE_RELEASES, RELEASES_YANKED, "0", condition);
        free(condition);
        INFO("Release v%s was set as not YANKED", version);
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
    int yanked_offset = -6;
    char* index_dashes = char_repeat('-', -index_offset + 2); // +2 for left and right padding
    char* version_dashes = char_repeat('-', -version_offset + 2);
    char* date_dashes = char_repeat('-', -date_offset + 2);
    char* pushed_dashes = char_repeat('-', -pushed_offset + 2);
    char* yanked_dashes = char_repeat('-', -yanked_offset + 2);

    printf("| Index | %*s | %*s | %*s | %*s |\n", version_offset, "Version", pushed_offset, "Pushed", yanked_offset, "Yanked", date_offset, "Date");
    printf("|%s+%s+%s+%s+%s|\n", 
            index_dashes,
            version_dashes,
            pushed_dashes,
            yanked_dashes,
            date_dashes
          );

    for(size_t i = 0; i < count; ++i){
        printf(
                "| %*zu | %*s | %*s | %*s | %*s |\n",
                index_offset,
                i+1, 
                version_offset,
                (STREQ(releases[i].version.full, "0.0.0")) ? VERSION_UNRELEASED : releases[i].version.full, 
                pushed_offset,
                releases[i].pushed ? "True" : "False",
                yanked_offset,
                releases[i].yanked ? "True" : "False",
                date_offset,
                releases[i].date.full
              );
    }

    free(releases);
    free(index_dashes);
    free(pushed_dashes);
    free(yanked_dashes);
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
    int title_offset = -60;
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

void command_generate(Options options)
{
    char* value = options.argv[options.argc-1];

    if(STREQ(value, command_to_string(COMMAND_GENERATE))) {
        ERRO("A value must be specified");
        INFO("Try: %s generate -h", EXECUTABLE_NAME);
        exit(EXIT_FAILURE);
    }
    
    if(
        !STREQ(value, "config") &&
        !STREQ(value, "man") &&
        !STREQ(value, "autocomplete") 
    ) {
        ERRO("'%s' is not a valid value", value);
        INFO("Try: %s generate -h", EXECUTABLE_NAME);
        exit(EXIT_FAILURE);
    }

    if(STREQ(value, "config")){
        char* config = 
            "# ~/.config/.changelogger.yml\n"
            "\n"
            "editor: nvim\n"
            "always-export: true\n"
            "always-push: false\n"
            "release-warning-message: \"Remember to update the version and commit everything important!\"\n"
            "# config-path: /path/to/another/config/to/be/used\n";

        char* default_path = CHANGELOGGER_DEFAULT_CONFIG_PATH;

        if(config_found(default_path)){
            WARN("A config is found at '%s'", default_path);
            INFO("Generated config would be:\n%s", config);
            return;
        }

        clib_write_file(default_path, config, "w");
        free(default_path);
    } else if(STREQ(value, "autocomplete")){
        if (getuid() != 0) {
            PANIC("Installing autocompletion requires elevated privileges. Please run with sudo.\n");
        }

        install_autocompletion(SHELL_ZSH);
        INFO("%s autocomplete installed", shell_to_string(SHELL_ZSH));

        install_autocompletion(SHELL_BASH);
        INFO("%s autocomplete installed", shell_to_string(SHELL_BASH));
    } else {
        PANIC("Generating %s is not implemented yet", value);
    }

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

void load_version(Changelog changelog){
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
        if(options.remote_repo != NULL) // Only when it set blank by the user
            ERRO("Remote repo must not be blank");
    }

    if(!is_blank(options.editor)) {
        char* value = clib_format_text("'%s'", options.editor);
        update(TABLE_CONFIG, CONFIG_EDITOR, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.editor != NULL) // Only when it set blank by the user
            ERRO("Remote repo must not be blank");
    }
}


Command get_command(char* command)
{
#define COMPARE_AND_RETURN_COMMAND(c) \
    else if(STREQ(command, command_to_string(c))) return c;

    if(command == NULL) return COMMAND_UNSET;
    COMPARE_AND_RETURN_COMMAND(COMMAND_EXPORT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_PUSH)
    COMPARE_AND_RETURN_COMMAND(COMMAND_ADD)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_INIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_SET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_LIST)
    COMPARE_AND_RETURN_COMMAND(COMMAND_DELETE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_RELEASE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_EDIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GENERATE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_IMPORT)
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
    case COMMAND_PUSH:
        command_push(options);
        return;
    case COMMAND_GENERATE:
        command_generate(options);
        return;
    case COMMAND_IMPORT:
        command_import(options);
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
    case COMMAND_PUSH:
        return "push";
    case COMMAND_GENERATE:
        return "generate";
    case COMMAND_IMPORT:
        return "import";
      break;
    }

    return "";
}

char* is_usable(
    ArgumentAbbr abbr, 
    CliArguments args,
    Command command, 
    Command* compatible_commands, size_t commands_size
)
{
    char* commands_str = clib_buffer_init();
    for(size_t i = 0; i < commands_size; ++i){
        char* com;
        if(i == 0)
            com = clib_format_text("%s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);
        else if(i == commands_size - 1)
            com = clib_format_text(" or %s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);
        else 
            com = clib_format_text(", %s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);

        clib_str_append(&commands_str, com);
        free(com);
    }

    for(size_t i = 0; i < commands_size; ++i) {
        if(command == compatible_commands[i]) return NULL;
    }

    char* flag = NULL;
    for(size_t i = 0; i < args.count; i++){
        if(args.args[i]->abr == abbr) flag = args.args[i]->full;
    }

    char* ret = clib_format_text("--%s can only be used with %s", flag, commands_str);
    free(commands_str);
    return ret;
}
