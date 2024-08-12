#ifndef OPTIONS_H
#define OPTIONS_H
#include "status.h"
#include "version.h"

typedef enum {
    ABBR_ALL = 'A',
    ABBR_ALWAYS_EXPORT = 'E',
    ABBR_CONFIG_PATH = 'c',
    ABBR_HELP = 'h',
    ABBR_INDEX = 'I',
    ABBR_NEW = 'N',
    ABBR_NO = 'n',
    ABBR_STATUS = 's',
    ABBR_VERSION = 'v',
    ABBR_VERSION_FULL = 'V',
    ABBR_VERSION_MAJOR = 'M',
    ABBR_VERSION_MINOR = 'm',
    ABBR_VERSION_PATCH = 'p',
    ABBR_YES = 'y',
    ABBR_REMOTE_REPO = 'R',
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
    const char* remote_repo;
} Options;

int always_export_set(Options options);
int version_full_set(Options options);
int version_major_set(Options options);
int version_minor_set(Options options);
int version_patch_set(Options options);
void log_options(Options options);

#endif // OPTIONS_H
