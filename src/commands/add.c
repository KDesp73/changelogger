#include "commands.h"
#include "config.h"
#include "extern/clib.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "utils.h"
#include "templates.h"
#include <string.h>
#include <sys/wait.h>
#include "database.h"

char* extract_commit_messages(char* input);
int open_editor(char* editor);
void add_commits();
void add_entry(const char* message, Status status);

void command_add(Options options)
{
    if(options.commits){
        add_commits();
        return;
    }

    char* message = options.argv[options.argc-1];

    if(strlen(message) > 60) {
        ERRO("Message too long");
        exit(1);
    }

    if(STREQ(message, command_to_string(COMMAND_ADD))) 
        PANIC("Message is not specified. Try: `%s add \"Your message\"`", EXECUTABLE_NAME);
    if(is_blank(message))
        PANIC("Message cannot be empty or blank");
    
    add_entry(message, options.status);
}

char* extract_commit_messages(char* input)
{
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
    if(!STREQ(latest, "0.0.0") && is_blank(versions)){
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
            if(!is_blank(line) && !STREQ(line, "\n") && strlen(line) <= 60){
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

