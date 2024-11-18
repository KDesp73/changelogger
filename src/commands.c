#include "help.h"
#include "options.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
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
    }

    help();
}

char* command_to_string(Command command)
{
    switch (command) {
    case COMMAND_UNSET:
    case COMMAND_UNKNOWN:
        return "";
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
      break;
    }

    return "";
}

char* is_usable(
    ArgumentAbbr abbr, 
    CliArguments args,
    Command command, 
    Command* compatible_commands, size_t commands_size
)
{
    char* commands_str = clib_buffer_init();
    for(size_t i = 0; i < commands_size; ++i){
        char* com;
        if(i == 0)
            com = clib_format_text("%s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);
        else if(i == commands_size - 1)
            com = clib_format_text(" or %s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);
        else 
            com = clib_format_text(", %s%s%s", BOLD, command_to_string(compatible_commands[i]), RESET);

        clib_str_append(&commands_str, com);
        free(com);
    }

    for(size_t i = 0; i < commands_size; ++i) {
        if(command == compatible_commands[i]) return NULL;
    }

    char* flag = NULL;
    for(size_t i = 0; i < args.count; i++){
        if(args.args[i]->abr == abbr) flag = args.args[i]->full;
    }

    char* ret = clib_format_text("--%s can only be used with %s", flag, commands_str);
    free(commands_str);
    return ret;
}
