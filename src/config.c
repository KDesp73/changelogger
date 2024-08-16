#include "database.h"
#define CLIB_IMPLEMENTATION
#include "extern/clib.h"
#include "config.h"
#include "utils.h"
#include <yaml.h>

int config_found(const char* path)
{
    if(path == NULL) return false;

    return clib_file_exists(path);
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
        exit(1);
    }
    yaml_parser_set_input_string(&parser, (unsigned char*)yaml_data, strlen(yaml_data));

    while (1) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Error parsing YAML: %s\n", parser.problem);
            yaml_parser_delete(&parser);
            exit(1);
        }

// Devil's work
#define LABEL_FOUND(label) \
    if (strcmp((char*)event.data.scalar.value, label) == 0) { \
        if (yaml_parser_parse(&parser, &event) && event.type == YAML_SCALAR_EVENT)

        switch (event.type) {
            case YAML_MAPPING_START_EVENT:
                break;
            case YAML_SCALAR_EVENT:
                LABEL_FOUND(YML_ALWAYS_EXPORT)
                    config.always_export = is_true((const char*) event.data.scalar.value);
                } else LABEL_FOUND(YML_ALWAYS_PUSH)
                    config.always_push = is_true((const char*)event.data.scalar.value);
                } else LABEL_FOUND(YML_RELEASE_WARNING_MESSAGE) 
                    config.release_warning_message = strdup((char*)event.data.scalar.value);
                } else LABEL_FOUND(YML_CONFIG_PATH) 
                    config.config_path = strdup((char*)event.data.scalar.value);
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

#undef LABEL_FOUND

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
        alt_config.exists = CONFIG_EXISTS;
        return alt_config;
    }

    if(!default_exists) {
        Config empty_config = {0};
        empty_config.exists = CONFIG_DOESNT_EXIST;
        return empty_config;
    }

    Config default_config = parse_config(default_config_path);
    default_config.exists = CONFIG_EXISTS;
    if(default_config.config_path != NULL){
        const char* redirect_path = default_config.config_path;

        if(config_found(redirect_path)){
            Config redirect_config = parse_config(redirect_path);
            redirect_config.exists = CONFIG_EXISTS;
            return redirect_config;
        }
    }

    return default_config;
}
