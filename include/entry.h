#ifndef ENTRY_H
#define ENTRY_H

#include "date.h"
#include "status.h"
#include "version.h"
#include <stdio.h>

typedef struct {
    Version version;
    Status status;
    Date date;
    char* message;
} Entry;

#endif // ENTRY_H
