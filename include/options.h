#ifndef OPTIONS_H
#define OPTIONS_H
#include "status.h"
#include "version.h"

typedef enum {
    ABBR_ALL = 'A',
    ABBR_ALWAYS_EXPORT = 'E',
    ABBR_ALWAYS_PUSH = 'P',
    ABBR_ASSET = 'B', // for binary
    ABBR_COMMITS = 'C',
    ABBR_CONFIG_PATH = 'c',
    ABBR_EDITOR = 'e',
    ABBR_FILE = 'f',
    ABBR_FORMAT = 'F',
    ABBR_HELP = 'h',
    ABBR_INDEX = 'I',
    ABBR_NEW = 'N',
    ABBR_NO = 'n',
    ABBR_PUSH = 'p',
    ABBR_RELEASES = 'r',
    ABBR_REMOTE_REPO = 'R',
    ABBR_STATUS = 's',
    ABBR_TITLE = 't',
    ABBR_UNYANK = 'U',
    ABBR_VERSION = 'v',
    ABBR_VERSION_FULL = 'V',
    ABBR_YANK = 'Y',
    ABBR_YES = 'y',
} ArgumentAbbr;

typedef struct {
    int argc;
    char** argv;
    Status status;
    Version version;
    const char* config_path;
    _Bool index;
    _Bool yes;
    _Bool no;
    const char* new;
    _Bool always_export;
    _Bool always_push;
    _Bool push;
    const char* remote_repo;
    const char* title;
    const char* file;
    const char* format;
    const char* yank;
    const char* unyank;
    _Bool all;
    _Bool releases;
    _Bool commits;
    const char* editor;
    const char* asset;
} Options;

int always_push_set(Options options);
int always_export_set(Options options);
int version_full_set(Options options);
int status_set(Options options);
void log_options(Options options);

#endif // OPTIONS_H
