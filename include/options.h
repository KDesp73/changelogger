#ifndef OPTIONS_H
#define OPTIONS_H
#include "commands.h"
#include "status.h"
#include "version.h"

typedef struct {
    Command command;
    Status status;
    Version version;
} Options;

#endif // OPTIONS_H
