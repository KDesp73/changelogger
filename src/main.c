#include "commands.h"
#include "config.h"
#include "database.h"
#include "utils.h"
#include "help.h"
#include "options.h"
#include "status.h"
#include "version.h"
#include <stdio.h>

#define CLIB_IMPLEMENTATION
#include "extern/clib.h"

void help_message(Command command)
{
    switch (command) {
        case COMMAND_ADD:
            add_help();
            break;
        case COMMAND_LIST:
            list_help();
            break;
        case COMMAND_SET:
            set_help();
            break;
        case COMMAND_GET:
            get_help();
            break;
        case COMMAND_DELETE:
            delete_help();
            break;
        case COMMAND_RELEASE:
            release_help();
            break;
        case COMMAND_EXPORT:
            export_help();
            break;
        case COMMAND_EDIT:
            edit_help();
            break;
        case COMMAND_PUSH:
            push_help();
            break;
        case COMMAND_GENERATE:
            generate_help();
            break;
        case COMMAND_UNSET:
        case COMMAND_UNKNOWN:
        case COMMAND_INIT:
        default:
            help();
            break;
    }
}

Options parse_options(int argc, char** argv, Command* command) 
{
    Options options = {0};
    options.argc = argc;
    options.argv = argv;
    options.version.full = NULL;
    options.status = STATUS_ADDED;
    *command = get_command(argv[1]);

    // NOTE: The help fields are not set since 
    // the help message is written by hand
    CliArguments args = clib_make_cli_arguments(16,
        clib_create_argument(ABBR_HELP, "help", "", no_argument),
        clib_create_argument(ABBR_VERSION, "version", "", no_argument),
        clib_create_argument(ABBR_STATUS, "status", "", required_argument),
        clib_create_argument(ABBR_CONFIG_PATH, "config-path", "", required_argument),
        clib_create_argument(ABBR_REMOTE_REPO, "remote-repo", "", required_argument),
        clib_create_argument(ABBR_ALL, "all", "", no_argument),
        clib_create_argument(ABBR_NEW, "new", "", required_argument),
        clib_create_argument(ABBR_NO, "no", "", no_argument),
        clib_create_argument(ABBR_YES, "yes", "", no_argument),
        clib_create_argument(ABBR_INDEX, "index", "", no_argument),
        clib_create_argument(ABBR_ALWAYS_EXPORT, "always-export", "", required_argument),
        clib_create_argument(ABBR_ALWAYS_PUSH, "always-push", "", required_argument),
        clib_create_argument(ABBR_VERSION_FULL, "version-full", "", required_argument),
        clib_create_argument(ABBR_TITLE, "title", "", required_argument),
        clib_create_argument(ABBR_RELEASES, "releases", "", no_argument),
        clib_create_argument(ABBR_PUSH, "push", "", no_argument)
    );

    int opt;
    LOOP_ARGS(opt, args){
        switch (opt) {
        case ABBR_HELP:
            help_message(*command);
            exit(0);
        case ABBR_VERSION:
            printf("%s v%s\n", EXECUTABLE_NAME, VERSION);
            exit(0);
        case ABBR_STATUS:
            if(is_number(optarg)){
                options.status = atoi(optarg);
                if(options.status < 1 || options.status > 6) 
                    PANIC("Status should be between 1 and 6");
            } else
                options.status = get_status(optarg);
            break;
        case ABBR_VERSION_FULL:
            if(
                *command != COMMAND_LIST &&
                *command != COMMAND_PUSH
            ) PANIC("--version-full can only be used with `list` and `push`");
            if(!STREQ(optarg, VERSION_UNRELEASED) && !is_valid_version(optarg)) PANIC("Version '%s' is not valid", optarg);

            options.version.full = optarg;
            parse_version(&options.version);
            break;
        case ABBR_CONFIG_PATH:
            if(*command != COMMAND_SET) PANIC("--config-path can only be used with `set`");

            options.config_path = optarg;
            break;
        case ABBR_NO:
            options.no = true;
            break;
        case ABBR_YES:
            options.yes = true;
            break;
        case ABBR_NEW:
            options.new = optarg;
            break;
        case ABBR_INDEX:
            options.index = true;
            break;
        case ABBR_REMOTE_REPO:
            options.remote_repo = optarg;
            break;
        case ABBR_ALWAYS_EXPORT:
            options.always_export = is_true(optarg);
            break;
        case ABBR_ALWAYS_PUSH:
            options.always_push = is_true(optarg);
            break;
        case ABBR_TITLE:
            if(*command != COMMAND_EDIT) PANIC("--title can only be used with `edit`");

            options.title = optarg;
            break;
        case ABBR_ALL:
            options.all = true;
            break;
        case ABBR_RELEASES:
            if(*command != COMMAND_LIST) PANIC("--releases can only be used with `list`");

            options.releases = true;
            break;
        case ABBR_PUSH:
            if(*command != COMMAND_RELEASE) PANIC("--push can only be used with `release`");

            options.push = true;
            break;
        default:
            exit(1);
        }
    }
    clib_clean_arguments(&args);

    if(*command == COMMAND_UNKNOWN) {
        PANIC("Unknown command: %s", argv[1]);
    } else if(*command == COMMAND_UNSET) {
        PANIC("A command should always be specified");
    } 

    if(options.version.full == NULL && is_version_set(options.version))
        make_version(&options.version);

    return options;
}

int main(int argc, char** argv)
{
    Config config = get_config();
    if(config.exists) load_config(config);

    Command command;
    Options options = parse_options(argc, argv, &command);

    execute_command(command, options);

    sqlite3* db;
    sqlite3_open(SQLITE_DB, &db);
    _Bool export = select_always_export(db);
    sqlite3_close(db);

    if(export && (
         command == COMMAND_ADD ||
         command == COMMAND_EDIT ||
         command == COMMAND_DELETE)
    ) {
        execute_command(COMMAND_EXPORT, options);
    }

    return 0;
}
