#include "help.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include "extern/ansi.h"
#include "commands.h"

Command get_command(char* command)
{
#define COMPARE_AND_RETURN_COMMAND(c) \
    else if(STREQ(command, command_to_string(c))) return c;

    if(command == NULL) return COMMAND_UNSET;
    COMPARE_AND_RETURN_COMMAND(COMMAND_EXPORT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_PUSH)
    COMPARE_AND_RETURN_COMMAND(COMMAND_ADD)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_INIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_SET)
    COMPARE_AND_RETURN_COMMAND(COMMAND_LIST)
    COMPARE_AND_RETURN_COMMAND(COMMAND_DELETE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_RELEASE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_EDIT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_GENERATE)
    COMPARE_AND_RETURN_COMMAND(COMMAND_IMPORT)
    COMPARE_AND_RETURN_COMMAND(COMMAND_UPDATE)
    else return COMMAND_UNKNOWN;

#undef COMPARE_AND_RETURN_COMMAND
}

void execute_command(Command command, Options options)
{
    switch (command) {
    case COMMAND_UNKNOWN:
        ERRO("Unknown command: %s", options.argv[1]);
        break;
    case COMMAND_UNSET:
        ERRO("Command not found");
        break;
    case COMMAND_INIT:
        command_init(options);
        return;
    case COMMAND_ADD:
        command_add(options);
        return;
    case COMMAND_SET:
        command_set(options);
        return;
    case COMMAND_LIST:
        command_list(options);
        return;
    case COMMAND_DELETE:
        command_delete(options);
        return;
    case COMMAND_RELEASE:
        command_release(options);
        return;
    case COMMAND_EXPORT:
        command_export(options);
        return;
    case COMMAND_GET:
        command_get(options);
        return;
    case COMMAND_EDIT:
        command_edit(options);
        return;
    case COMMAND_PUSH:
        command_push(options);
        return;
    case COMMAND_GENERATE:
        command_generate(options);
        return;
    case COMMAND_IMPORT:
        command_import(options);
        return;
    case COMMAND_UPDATE:
        command_update(options);
        return;
    }

    help();
}

char* command_to_string(Command command)
{
    switch (command) {
    case COMMAND_INIT:
        return "init";
    case COMMAND_ADD:
        return "add";
    case COMMAND_LIST:
        return "list";
    case COMMAND_SET:
        return "set";
    case COMMAND_DELETE:
        return "delete";
    case COMMAND_RELEASE:
        return "release";
    case COMMAND_GET:
        return "get";
    case COMMAND_EXPORT:
        return "export";
    case COMMAND_EDIT:
        return "edit";
    case COMMAND_PUSH:
        return "push";
    case COMMAND_GENERATE:
        return "generate";
    case COMMAND_IMPORT:
        return "import";
    case COMMAND_UPDATE:
        return "update";
    case COMMAND_UNSET:
    case COMMAND_UNKNOWN:
    default:
        break;
    }

    return "";
}

char* is_usable(
    ArgumentAbbr abbr, 
    cli_args_t args,
    Command command, 
    Command* compatible_commands, size_t commands_size
)
{
    char* commands_str = str_buffer_init();
    for(size_t i = 0; i < commands_size; ++i){
        char* com;
        if(i == 0)
            com = str_format("%s%s%s", ANSI_BOLD, command_to_string(compatible_commands[i]), ANSI_RESET);
        else if(i == commands_size - 1)
            com = str_format(" or %s%s%s", ANSI_BOLD, command_to_string(compatible_commands[i]), ANSI_RESET);
        else 
            com = str_format(", %s%s%s", ANSI_BOLD, command_to_string(compatible_commands[i]), ANSI_RESET);

        str_append(&commands_str, com);
        free(com);
    }

    for(size_t i = 0; i < commands_size; ++i) {
        if(command == compatible_commands[i]) {
            free(commands_str);
            return NULL;
        }
    }

    char* flag = "";
    for(size_t i = 0; i < args.count; i++){
        if(args.args[i]->abr == abbr) flag = args.args[i]->full;
    }

    char* ret = str_format("--%s can only be used with %s", flag, commands_str);
    free(commands_str);
    return ret;
}
