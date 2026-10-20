#ifndef COMMANDS_H
#define COMMANDS_H
#include "extern/cli.h"
#include "extern/fs.h"
#include "extern/logging.h"
#include "extern/str.h"
#include "options.h"

typedef enum {
    COMMAND_UNSET = -2,
    COMMAND_UNKNOWN = -1,
#define COMMAND(upper, lower, usage, description, helpfn) COMMAND_##upper,
#include "commands.inc"
#undef COMMAND
} Command;

void execute_command(Command command, Options options);
Command get_command(char* command);
char* command_to_string(Command command);

#define COMMAND(upper, lower, usage, description, helpfn) void command_##lower(Options options);
#include "commands.inc"
#undef COMMAND

/**
 * NULL is true
 * In case of error it returns the error message
 */
char* is_usable(
    ArgumentAbbr abbr, 
    cli_args_t args,
    Command command, 
    Command* compatible_commands, size_t commands_size
);

/* One entry per command: how it is dispatched and how help lists it */
typedef struct {
    Command command;
    const char* name;
    const char* usage;
    const char* description;
    HandlerFunc handler;
} CommandSpec;

const CommandSpec* command_specs(size_t* count);
Dispatcher* command_dispatcher(void);

#endif // COMMANDS_H
