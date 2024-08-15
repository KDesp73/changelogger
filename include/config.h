#ifndef CONFIG_H
#define CONFIG_H

#define EXECUTABLE_NAME "changelogger"
#define VERSION "0.0.7"
#define CHANGELOG_FILE "CHANGELOG.md"
#define CHANGELOG_DIR ".changelog"
#define SQLITE_DB ".changelog/changelog.db"
#define CHANGELOGGER_DEFAULT_CONFIG_PATH \
    clib_format_text("%s/.changelogger.yml", getenv("HOME"))

#define YML_ALWAYS_EXPORT "always-export"
#define YML_ALWAYS_PUSH "always-push"
#define YML_RELEASE_WARNING_MESSAGE "release-warning-message"
#define YML_CONFIG_PATH "config-path"

#define CONFIG_EXISTS 1
#define CONFIG_DOESNT_EXIST 0

typedef struct {
    _Bool always_export;
    _Bool always_push;
    const char* release_warning_message;
    const char* config_path;
    _Bool exists;
} Config;

void log_config(Config config);
int config_found(const char* path); // NULL uses the default path
Config parse_config(const char* path);
Config get_config();



#endif // CONFIG_H
