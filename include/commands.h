#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    COMMAND_UNKNOWN = -1,
    COMMAND_INIT,
    COMMAND_ADD,
    COMMAND_LIST,
    COMMAND_DELETE,
    COMMAND_GIT_POPULATE,
} Command;

Command get_command(char* command);
void command_init();

void execute_command(int argc, char** argv);

#endif // COMMANDS_H
