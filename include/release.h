#ifndef RELEASE_H
#define RELEASE_H

#include "date.h"
#include "version.h"
typedef struct {
    Version version;
    Date date;
    _Bool pushed;
} Release;


#endif // RELEASE_H
