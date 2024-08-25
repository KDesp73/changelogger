#include "database.h"
#include <string.h>
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "config.h"
#include "utils.h"
#include <yaml.h>


void install_autocompletion(Shell shell)
{
    char* path = NULL;
    switch (shell) {
    case SHELL_ZSH:
        path = ZSH_AUTOCOMPLETE_PATH;
    case SHELL_BASH:
        path = BASH_AUTOCOMPLETE_PATH;
    case SHELL_UNKNOWN:
      break;
    }

    char* command = clib_format_text("curl -s https://raw.githubusercontent.com/KDesp73/changelogger/main/docs/_changelogger.%s -o %s", shell_to_string(shell), path);
    clib_execute_command(command);
}

char* shell_to_string(Shell shell)
{
    switch(shell){
    case SHELL_UNKNOWN:
        return "";
    case SHELL_BASH:
        return "bash";
    case SHELL_ZSH:
        return "zsh";
    }
    return "";
}

Shell get_shell()
{
    char* shell = getenv("SHELL");

#define COMPARE_AND_RETURN_SHELL(x) \
    if(strstr(shell, shell_to_string(x)) != NULL) return x

    COMPARE_AND_RETURN_SHELL(SHELL_ZSH);
    COMPARE_AND_RETURN_SHELL(SHELL_BASH);

#undef COMPARE_AND_RETURN_SHELL

    return SHELL_UNKNOWN;
}

int config_found(const char* path)
{
    if(path == NULL) return false;

    return clib_file_exists(path);
}


void free_config(Config* config)
{
    if(config->editor != NULL)
        free((char*) config->editor);
    if(config->config_path != NULL)
        free((char*) config->config_path);
    if(config->release_warning_message != NULL)
        free((char*) config->release_warning_message);
}

Config parse_config(const char* path)
{
    const char* default_config = CHANGELOGGER_DEFAULT_CONFIG_PATH;
    const char* file = (path == NULL) ? default_config : path;
    const char* yaml_data = clib_read_file(file, "r");

    free((char*) default_config);

    Config config = {0};

    yaml_parser_t parser;
    yaml_event_t event;

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize YAML parser\n");
        free((char*) yaml_data);
        exit(1);
    }
    yaml_parser_set_input_string(&parser, (unsigned char*)yaml_data, strlen(yaml_data));

    while (1) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Error parsing YAML: %s\n", parser.problem);
            yaml_parser_delete(&parser);
            free((char*) yaml_data);
            exit(1);
        }

// Devil's work
#define IF_LABEL_FOUND(label) \
    if (strcmp((char*)event.data.scalar.value, label) == 0) { \
        if (yaml_parser_parse(&parser, &event) && event.type == YAML_SCALAR_EVENT)

        switch (event.type) {
            case YAML_MAPPING_START_EVENT:
                break;
            case YAML_SCALAR_EVENT:
                IF_LABEL_FOUND(YML_ALWAYS_EXPORT)
                    config.always_export = is_true((const char*) event.data.scalar.value);
                } else IF_LABEL_FOUND(YML_ALWAYS_PUSH)
                    config.always_push = is_true((const char*)event.data.scalar.value);
                } else IF_LABEL_FOUND(YML_RELEASE_WARNING_MESSAGE) 
                    config.release_warning_message = strdup((char*)event.data.scalar.value);
                } else IF_LABEL_FOUND(YML_CONFIG_PATH) 
                    config.config_path = strdup((char*)event.data.scalar.value);
                } else IF_LABEL_FOUND(YML_EDITOR) 
                    config.editor = strdup((char*) event.data.scalar.value);
                }
                break;
            case YAML_MAPPING_END_EVENT:
                break;
            case YAML_STREAM_END_EVENT:
                yaml_event_delete(&event);
                goto cleanup;
            default:
                break;
        }

        yaml_event_delete(&event);
    }

cleanup:
    yaml_parser_delete(&parser);
    free((char*) yaml_data);

#undef IF_LABEL_FOUND

    return config;
}
void log_config(Config config)
{
    printf("%s: %d\n", YML_ALWAYS_EXPORT, config.always_export);
    printf("%s: %d\n", YML_ALWAYS_PUSH, config.always_push);
    printf("%s: %s\n", YML_RELEASE_WARNING_MESSAGE, config.release_warning_message);
}

Config get_config()
{
    char* alt_config_path = select_str(TABLE_CONFIG, CONFIG_CONFIG_PATH, CONFIG_CONDITION);
    char* default_config_path = CHANGELOGGER_DEFAULT_CONFIG_PATH;

    _Bool alt_exists = config_found(alt_config_path);
    _Bool default_exists = config_found(default_config_path);

    if(alt_exists) {
        Config alt_config = parse_config(alt_config_path);
        free(alt_config_path);
        free(default_config_path);
        alt_config.exists = CONFIG_EXISTS;
        return alt_config;
    }

    free(alt_config_path);

    if(!default_exists) {
        Config empty_config = EMPTY_CONFIG;
        free(default_config_path);
        return empty_config;
    }

    Config default_config = parse_config(default_config_path);
    default_config.exists = CONFIG_EXISTS;
    if(default_config.config_path != NULL){
        const char* redirect_path = strdup(default_config.config_path);

        if(config_found(redirect_path)){
            free_config(&default_config);
            Config redirect_config = parse_config(redirect_path);
            redirect_config.exists = CONFIG_EXISTS;
            free(default_config_path);
            return redirect_config;
        }
        free((char*) redirect_path);
    }
    free(default_config_path);

    return default_config;
}
