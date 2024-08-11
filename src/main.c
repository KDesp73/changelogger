#include "commands.h"
#include "config.h"
#include "utils.h"
#include "help.h"
#include "options.h"
#include "status.h"
#include "version.h"
#include <stdio.h>

#define CLIB_IMPLEMENTATION
#include "clib.h"

Options parse_options(int argc, char** argv, Command* command) 
{
    Options options = {0};
    options.argc = argc;
    options.argv = argv;
    options.version.full = NULL;
    options.status = STATUS_UNSET;
    *command = get_command(argv[1]);

    // NOTE: The help fields are not set since 
    // the help message is written by hand
    CliArguments args = clib_make_cli_arguments(8,  
        clib_create_argument(ABBR_HELP, "help", "", no_argument),
        clib_create_argument(ABBR_VERSION, "version", "", no_argument),
        clib_create_argument(ABBR_STATUS, "status", "", required_argument),
        clib_create_argument(ABBR_VERSION_MAJOR, "version-major", "", required_argument),
        clib_create_argument(ABBR_VERSION_MINOR, "version-minor", "", required_argument),
        clib_create_argument(ABBR_VERSION_PATCH, "version-patch", "", required_argument),
        clib_create_argument(ABBR_VERSION_FULL, "version-full", "", required_argument),
        clib_create_argument(ABBR_CONFIG_PATH, "config-path", "", required_argument)
    );

    int opt;
    LOOP_ARGS(opt, args){
        switch (opt) {
        case ABBR_HELP:
            if(*command == COMMAND_SET) set_help();
            else if(*command == COMMAND_ADD) add_help();
            else help();
            exit(0);
        case ABBR_VERSION:
            printf("%s v%s\n", EXECUTABLE_NAME, VERSION);
            exit(0);
        case ABBR_STATUS:
            if(is_number(optarg))
                options.status = atoi(optarg);
            else
                options.status = get_status(optarg);
            break;
        case ABBR_VERSION_MAJOR:
            if(*command != COMMAND_SET) PANIC("--version-major can only be used with `set`");
            if(!is_number(optarg)) PANIC("Major version value should be a number");

            options.version.major = atoi(optarg);
            break;
        case ABBR_VERSION_MINOR:
            if(*command != COMMAND_SET) PANIC("--version-minor can only be used with `set`");
            if(!is_number(optarg)) PANIC("Minor version value should be a number");

            options.version.minor = atoi(optarg);
            break;
        case ABBR_VERSION_PATCH:
            if(*command != COMMAND_SET) PANIC("--version-patch can only be used with `set`");
            if(!is_number(optarg)) PANIC("Patch version value should be a number");

            options.version.patch = atoi(optarg);
            break;
        case ABBR_VERSION_FULL:
            if(*command != COMMAND_SET) PANIC("--version-full can only be used with `set`");
            if(!is_valid_version(optarg)) PANIC("Version is not valid");

            options.version.full = optarg;
            parse_version(&options.version);
            make_version(&options.version);
            break;
        case ABBR_CONFIG_PATH:
            if(*command != COMMAND_SET) PANIC("--config-path can only be used with `set`");

            options.config_path = optarg;
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
    Command command;
    Options options = parse_options(argc, argv, &command);
    // log_options(options);
    execute_command(command, options);

    return 0;
}
