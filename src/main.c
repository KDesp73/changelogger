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

typedef enum {
    ABBR_HELP = 'h',
    ABBR_VERSION = 'v',
    ABBR_STATUS = 's',
    ABBR_VERSION_MAJOR = 'M',
    ABBR_VERSION_MINOR = 'm',
    ABBR_VERSION_PATCH = 'p',
    ABBR_VERSION_FULL = 'V',
    ABBR_CONFIG_PATH = 'c'
} ArgumentAbbr;

Options parse_options(int argc, char** argv) 
{
    Options options = {0};
    options.argc = argc;
    options.argv = argv;
    options.version.version = NULL;
    options.status = STATUS_UNSET;
    Command command = get_command(argv[1]);

    // NOTE: The help fields are not set since 
    // the help message is written by hand
    CliArguments args = clib_make_cli_arguments(7,  
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
            if(command == COMMAND_SET) set_help();
            else if(command == COMMAND_ADD) add_help();
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
            if(command != COMMAND_SET) PANIC("--version-major can only be used with `set`");
            if(!is_number(optarg)) PANIC("Major version value should be a number");

            options.version.version_major = atoi(optarg);
            break;
        case ABBR_VERSION_MINOR:
            if(command != COMMAND_SET) PANIC("--version-minor can only be used with `set`");
            if(!is_number(optarg)) PANIC("Minor version value should be a number");

            options.version.version_minor = atoi(optarg);
            break;
        case ABBR_VERSION_PATCH:
            if(command != COMMAND_SET) PANIC("--version-patch can only be used with `set`");
            if(!is_number(optarg)) PANIC("Patch version value should be a number");

            options.version.version_patch = atoi(optarg);
            break;
        case ABBR_VERSION_FULL:
            if(command != COMMAND_SET) PANIC("--version-full can only be used with `set`");
            if(!is_valid_version(optarg)) PANIC("Version is not valid");

            options.version.version = optarg;
            parse_version(&options.version);
            make_version(&options.version);
            break;
        default:
            exit(1);
        }
    }
    clib_clean_arguments(&args);

    if(command == COMMAND_UNKNOWN) {
        PANIC("Unknown command: %s", argv[1]);
    } else if(command == COMMAND_UNSET) {
        PANIC("A command should always be specified");
    } else {
        options.command = command;
    }

    if(options.version.version == NULL && is_version_set(options.version))
        make_version(&options.version);

    return options;
}

int main(int argc, char** argv)
{
    Options options = parse_options(argc, argv);
    // log_options(options);
    execute_command(options);

    return 0;
}
