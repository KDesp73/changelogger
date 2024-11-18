#include "commands.h"
#include "database.h"
#include "extern/querybuilder.h"
#include "extern/sqlite.h"
#include "utils.h"

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

