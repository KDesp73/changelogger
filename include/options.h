#ifndef OPTIONS_H
#define OPTIONS_H
#include "commands.h"
#include "status.h"
#include "version.h"

typedef struct {
    int argc;
    char** argv;
    Command command;
    Status status;
    Version version;
} Options;

void execute_command(Options options);
void log_options(Options options);

#endif // OPTIONS_H
