#ifndef COMMANDS_H
#define COMMANDS_H
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
} Command;

void execute_command(Command command, Options options);
Command get_command(char* command);
void command_init();
void command_add(Options options);
void command_set(Options options);
void command_list(Options options);
void command_delete(Options options);
void command_release(Options options);
char* command_to_string(Command command);

#endif // COMMANDS_H
