#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef struct {
    const char* status;
    const char* message;
} Change;

typedef struct {
    const char* version;
    const char* link;
    const char* date;
    Change* changes;
    size_t change_count;
    _Bool yanked;
} ParserRelease;

typedef struct {
    const char* title;
    const char* description;
    const char* remote_repo;
    ParserRelease* releases;
    size_t release_count;
} Changelog;

Changelog parse(const char* file);
void log_changelog(Changelog changelog);
void free_changelog(Changelog* changelog);


#endif // PARSER_H
