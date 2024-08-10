#ifndef ENTRY_H
#define ENTRY_H

#include <stdio.h>

typedef struct {
    char* version;
    size_t version_major;
    size_t version_minor;
    size_t version_patch;
} Version;

Version make_version(size_t major, size_t minor, size_t patch);
Version parse_version(char* version);
void free_version(Version* v);

typedef enum {
    STATUS_CHANGED,
    STATUS_ADDED,
    STATUS_DEPRECATED,
    STATUS_REMOVED,
    STATUS_FIXED,
    STATUS_SECURITY
} Status;

typedef struct {
    size_t year;
    size_t month;
    size_t day;
    size_t hours;
    size_t minutes;
    size_t seconds;

    char* full;
    char* date;
    char* time;
} Date;

void get_date(Date* date);
void free_date(Date* date);
void log_date(Date date);

typedef struct {
    Version version;
    Status status;
    Date date;
    char* message;
} Entry;

#endif // ENTRY_H
