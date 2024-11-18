#ifndef CONFIG_H
#define CONFIG_H

#define EXECUTABLE_NAME "changelogger"
#define VERSION "0.0.18"

#define CHANGELOG_FILE "CHANGELOG.md"
#define CHANGELOG_DIR ".changelog"
#define SQLITE_DB ".changelog/changelog.db"
#define TEMP_FILE ".changelog/commits.md"
#define CHANGELOGGER_DEFAULT_CONFIG_PATH \
    clib_format_text("%s/.config/.changelogger.yml", getenv("HOME"))

#define BASH_AUTOCOMPLETE_PATH "/etc/bash_completion.d/changelogger.bash"
#define ZSH_AUTOCOMPLETE_PATH "/usr/share/zsh/functions/Completion/_changelogger"
#define FISH_AUTOCOMPLETE_PATH "/etc/fish/conf.d/changelogger.fish"

#define YML_ALWAYS_EXPORT "always-export"
#define YML_ALWAYS_PUSH "always-push"
#define YML_RELEASE_WARNING_MESSAGE "release-warning-message"
#define YML_CONFIG_PATH "config-path"
#define YML_EDITOR "editor"

#define CONFIG_EXISTS 1
#define CONFIG_DOESNT_EXIST 0

typedef enum {
    SHELL_UNKNOWN,
    SHELL_ZSH,
    SHELL_FISH,
    SHELL_BASH
} Shell;

typedef struct {
    _Bool always_export;
    _Bool always_push;
    const char* release_warning_message;
    const char* config_path;
    const char* editor;
    _Bool exists;
} Config;

#define EMPTY_CONFIG \
{ \
    .always_export = 0, \
    .always_push = 0, \
    .exists = CONFIG_DOESNT_EXIST, \
    .release_warning_message = NULL, \
    .editor = NULL, \
    .config_path = NULL \
}

void log_config(Config config);
int config_found(const char* path);
Config parse_config(const char* path);
Config get_config();
void load_config(Config config);
void free_config(Config* config);

Shell get_shell();
char* shell_to_string(Shell shell);
Shell string_to_shell(const char* str);
int install_autocompletion(Shell shell);

#endif // CONFIG_H
