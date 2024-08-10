#include "commands.h"
#include "config.h"
#include "entry.h"
#include "help.h"
#include <stdio.h>
#include <sqlite3.h>

#define CLIB_IMPLEMENTATION
#include "clib.h"

// TODO: add to clib.h
#define LOOP_ARGS(opt, args) \
    while((opt = getopt_long(argc, argv, clib_generate_cli_format_string(args), clib_get_options(args), NULL)) != -1)

#define VERSION_MAJOR_ABBR (char)('v' + 'm')
#define VERSION_MINOR_ABBR (char)('v' + 'i')
#define VERSION_PATCH_ABBR (char)('v' + 'p')
#define CONFIG_DIR_ABBR (char)('c' + 'd' + 'i')

int main(int argc, char** argv)
{
    CliArguments args = clib_make_cli_arguments(7, 
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('v', "version", "Prints the version of the project", no_argument),
        clib_create_argument('s', "status", "", required_argument),
        clib_create_argument(VERSION_MAJOR_ABBR, "version-major", "", required_argument),
        clib_create_argument(VERSION_MINOR_ABBR, "version-minor", "", required_argument),
        clib_create_argument(VERSION_PATCH_ABBR, "version-patch", "", required_argument),
        clib_create_argument(CONFIG_DIR_ABBR, "config-dir", "", required_argument)
    );
    
    char* usage = clib_format_text("%s [-h | -v]", argv[0]); // TODO: update
    int opt;
    LOOP_ARGS(opt, args){
        switch (opt) {
        case 'h':
            help(usage, "Made by KDesp73");
            free(usage);
            exit(0);
        case 'v':
            printf("changelogger v%s\n", VERSION);
            exit(0);
        case 's':
            break;
        case VERSION_MAJOR_ABBR:
            break;
        case VERSION_MINOR_ABBR:
            break;
        case VERSION_PATCH_ABBR:
            break;
        default:
            INFO("Usage: %s", usage);
            exit(1);
        }
    }
    free(usage);

    execute_command(argc, argv);

    return 0;
}
