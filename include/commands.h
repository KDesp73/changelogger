#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    COMMAND_UNSET = -2,
    COMMAND_UNKNOWN = -1,
    COMMAND_INIT,
    COMMAND_ADD,
    COMMAND_LIST,
    COMMAND_SET,
    COMMAND_DELETE,
} Command;

Command get_command(char* command);
void command_init();
char* command_to_string(Command command);

#endif // COMMANDS_H
