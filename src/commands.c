#include "help.h"
#include "options.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include "extern/ansi.h"
#include "commands.h"

/*
 * Commands take their options by value, so each one gets a thin adapter that
 * fits cli.h's bool (*)(void*) handler signature.
 */
#define COMMAND(upper, lower, usage, description, helpfn) \
    static bool handle_##lower(void* ctx) { command_##lower(*(Options*) ctx); return true; }

#include "commands.inc"

#undef COMMAND

static const CommandSpec command_table[] = {
#define COMMAND(upper, lower, usage, description, helpfn) \
    { COMMAND_##upper, #lower, usage, description, handle_##lower },
#include "commands.inc"
#undef COMMAND
};

#define COMMAND_COUNT (sizeof(command_table) / sizeof(command_table[0]))

_Static_assert(sizeof(command_table) / sizeof(command_table[0]) <= MAX_COMMANDS,
               "more commands than the Dispatcher can hold");

static Dispatcher dispatcher;
static bool dispatcher_ready = false;

static void register_commands(void)
{
    if (dispatcher_ready) {
        return;
    }

    dispatcher_init(&dispatcher);
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        dispatcher_add(&dispatcher, command_table[i].name,
                       command_table[i].description, command_table[i].handler);
    }
    dispatcher_ready = true;
}

Dispatcher* command_dispatcher(void)
{
    register_commands();
    return &dispatcher;
}

const CommandSpec* command_specs(size_t* count)
{
    if (count != NULL) {
        *count = COMMAND_COUNT;
    }
    return command_table;
}

static const CommandSpec* find_spec(Command command)
{
    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        if (command_table[i].command == command) {
            return &command_table[i];
        }
    }
    return NULL;
}

Command get_command(char* command)
{
    if (command == NULL) {
        return COMMAND_UNSET;
    }

    for (size_t i = 0; i < COMMAND_COUNT; ++i) {
        if (STREQ(command, command_table[i].name)) {
            return command_table[i].command;
        }
    }

    return COMMAND_UNKNOWN;
}

void execute_command(Command command, Options options)
{
    if (command == COMMAND_UNKNOWN) {
        ERRO("Unknown command: %s", options.argv[1]);
        help();
        return;
    }
    if (command == COMMAND_UNSET) {
        ERRO("Command not found");
        help();
        return;
    }

    const CommandSpec* spec = find_spec(command);
    HandlerFunc handler = (spec != NULL)
        ? dispatcher_find(command_dispatcher(), spec->name)
        : NULL;

    if (handler == NULL) {
        help();
        return;
    }

    if (!handler(&options)) {
        exit(1);
    }
}

char* command_to_string(Command command)
{
    switch (command) {
#define COMMAND(upper, lower, usage, description, helpfn) case COMMAND_##upper: return #lower;
#include "commands.inc"
#undef COMMAND
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
