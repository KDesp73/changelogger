#include "commands.h"
#include "config.h"
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
    char* latest_q = shell_quote(latest);
    char* tag_command = str_format("git pull --quiet && git tag --list v%s", latest_q);
    char* versions = NULL;
    execute_command_capture(tag_command, &versions);
    free(tag_command);
    free(latest_q);
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
        git_command = str_format("git shortlog -z");
    } else {
        char* range_q = shell_quote(latest);
        git_command = str_format("git shortlog -z v%s..HEAD", range_q);
        free(range_q);
    }

    char* out = NULL;
    execute_command_capture(git_command, &out);
    free(git_command);

    if(is_blank(out)){
        ERRO("No commit messages after v%s", latest);
        exit(1);
    }

    char* formatted_out = extract_commit_messages(out);
    free(out);

    char* buffer = str_buffer_init();

    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_ADDED));
    str_append_ln(&buffer, formatted_out);
    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_CHANGED));
    str_append_ln(&buffer, "");
    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_REMOVED));
    str_append_ln(&buffer, "");
    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_FIXED));
    str_append_ln(&buffer, "");
    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_DEPRECATED));
    str_append_ln(&buffer, "");
    str_append_ln(&buffer, TEMPLATE_STATUS(STATUS_SECURITY));
    str_append_ln(&buffer, "");

    fs_write_file(TEMP_FILE, buffer, "w");
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

    char* user_edited_commits = fs_read_file(TEMP_FILE, "r");
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
    Date date;
    get_date(&date);

    char* message_q = sql_quote(message);
    char* version_q = sql_quote(VERSION_UNRELEASED);
    char* date_q = sql_quote(date.full);

    query_builder_t* qb = create_query_builder();
    insert_q(qb, TABLE_ENTRIES);
    columns_q(qb, "message, status, version, date");
    char* values = str_format("%s, %d, %s, %s", message_q, status, version_q, date_q);
    values_q(qb, values);
    char* query = build_query(qb);
    db_execute(query);

    free(message_q);
    free(version_q);
    free(date_q);
    free_date(&date);
    free(values);
    free(query);
}

