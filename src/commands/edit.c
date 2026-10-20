#include "commands.h"
#include "database.h"
#include "utils.h"

void command_edit(Options options)
{
    /*
     * Entries may only point at versions that exist in Releases. Letting an
     * arbitrary version through (which used to work) left entries orphaned and
     * made `export` crash on the missing release date. "0.0.0" is how
     * `parse_version` represents the unreleased version, so it is exempt too.
     */
    _Bool unreleased = version_full_set(options) &&
                       (STREQ(options.version.full, VERSION_UNRELEASED) ||
                        STREQ(options.version.full, "0.0.0"));

    if (version_full_set(options) && !unreleased) {
        char* version_q = sql_quote(options.version.full);
        char* condition = str_format("version = %s", version_q);
        char* release = select_str(TABLE_RELEASES, RELEASES_VERSION, condition);
        free(condition);
        free(version_q);

        if (release == NULL) {
            ERRO("There is no release with version '%s'", options.version.full);
            INFO("Create it with '%s release --new patch' or import a changelog that contains it", EXECUTABLE_NAME);
            exit(1);
        }
        free(release);
    }

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
            if (input_finished()) {
                putchar('\n');
                ERRO("No input available, aborting");
                free(entries);
                exit(1);
            }
            ERRO("Error reading input.");
            clear_input_buffer();
            continue;
        }

        if (!is_number(index_str)) {
            ERRO("'%s' is not a number", index_str);
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
            char* condition = entry_condition(entries[index-1].message, entries[index-1].date.full);
            char* formated_message = sql_quote(options.title);
            update(TABLE_ENTRIES, ENTRIES_MESSAGE, formated_message, condition);
            free(formated_message);
            free(condition);
        }
        
        if(status_set(options)){
            char* condition = entry_condition(entries[index-1].message, entries[index-1].date.full);
            char* formated_status = str_format("%d", options.status);
            update(TABLE_ENTRIES, ENTRIES_STATUS, formated_status, condition);
            free(formated_status);
            free(condition);
        }

        if(version_full_set(options)){
            char* condition = entry_condition(entries[index-1].message, entries[index-1].date.full);
            char* formated_version = sql_quote(unreleased ? VERSION_UNRELEASED : options.version.full);
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
            if (input_finished()) {
                putchar('\n');
                ERRO("No input available, aborting");
                free(entries);
                exit(1);
            }
            ERRO("Error reading input.");
            clear_input_buffer();
            continue;
        }

        if (choice[0] == 't' || choice[0] == 's') {
            break;
        } else {
            ERRO("'%s' is not a valid option. Please enter 't' or 's'.", choice);
            clear_input_buffer();
        }
    }

    char* message = NULL;
    size_t message_cap = 0;
    Status s = STATUS_UNSET;
    if(choice[0] == 't'){
        clear_input_buffer();
        while(1){
            printf("Enter new message: ");
            ssize_t read = getline(&message, &message_cap, stdin);
            if (read == -1) {
                if (input_finished()) {
                    putchar('\n');
                    ERRO("No input available, aborting");
                    free(message);
                    free(entries);
                    exit(1);
                }
                fprintf(stderr, "Error reading input.\n");
                continue;
            }

            if (read > 0 && message[read - 1] == '\n') {
                message[read - 1] = '\0';
            }

            if (is_blank(message)) {
                ERRO("New message cannot be blank\n");
                continue;
            }
            break;
        }

        char* condition = entry_condition(entries[index-1].message, entries[index-1].date.full);
        char* formated_message = sql_quote(message);
        update(TABLE_ENTRIES, ENTRIES_MESSAGE, formated_message, condition);
        free(formated_message);
        free(condition);
        free(message);
    } else if(choice[0] == 's'){
        char status[10];
        clear_input_buffer();
        while(1){
            printf("Enter new status: ");
            if (fgets(status, sizeof(status), stdin) == NULL) {
                if (input_finished()) {
                    putchar('\n');
                    ERRO("No input available, aborting");
                    free(entries);
                    exit(1);
                }
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

        char* condition = entry_condition(entries[index-1].message, entries[index-1].date.full);
        char* formated_status = str_format("%d", s);
        update(TABLE_ENTRIES, ENTRIES_STATUS, formated_status, condition);
        free(formated_status);
        free(condition);
    }
}

