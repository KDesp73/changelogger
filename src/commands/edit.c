#include "commands.h"
#include "database.h"
#include "utils.h"

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

