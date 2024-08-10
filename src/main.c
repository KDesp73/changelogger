#include "commands.h"
#include "config.h"
#include "entry.h"
#include "help.h"
#include <stdio.h>
#define CLIB_IMPLEMENTATION
#include "clib.h"

// TODO: add to clib.h
#define LOOP_ARGS(opt, args) \
    while((opt = getopt_long(argc, argv, clib_generate_cli_format_string(args), clib_get_options(args), NULL)) != -1)

int main(int argc, char** argv)
{
    CliArguments args = clib_make_cli_arguments(2, 
        clib_create_argument('h', "help", "Prints this message", no_argument),
        clib_create_argument('v', "version", "Prints the version of the project", no_argument)
    );
    
    char* usage = clib_format_text("%s [-h | -v]", argv[0]);
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
        default:
            INFO("Usage: %s", usage);
            exit(1);
        }
    }
    free(usage);


    execute_command(argc, argv);

    return 0;
}
