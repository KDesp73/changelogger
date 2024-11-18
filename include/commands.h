#ifndef COMMANDS_H
#define COMMANDS_H
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "options.h"

typedef enum {
    COMMAND_UNSET = -2,
    COMMAND_UNKNOWN = -1,
    COMMAND_INIT,
    COMMAND_ADD,
    COMMAND_LIST,
    COMMAND_SET,
    COMMAND_GET,
    COMMAND_DELETE,
    COMMAND_RELEASE,
    COMMAND_EXPORT,
    COMMAND_EDIT,
    COMMAND_PUSH,
    COMMAND_GENERATE,
    COMMAND_IMPORT,
    COMMAND_UPDATE,
} Command;

void execute_command(Command command, Options options);
Command get_command(char* command);
char* command_to_string(Command command);

void command_init(Options options);
void command_add(Options options);
void command_set(Options options);
void command_list(Options options);
void command_delete(Options options);
void command_release(Options options);
void command_export(Options options);
void command_get(Options options);
void command_edit(Options options);
void command_push(Options options);
void command_generate(Options options);
void command_import(Options options);
void command_update(Options options);

/**
 * NULL is true
 * In case of error it returns the error message
 */
char* is_usable(
    ArgumentAbbr abbr, 
    CliArguments args,
    Command command, 
    Command* compatible_commands, size_t commands_size
);

#endif // COMMANDS_H
