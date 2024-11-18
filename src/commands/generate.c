#include "commands.h"
#include "config.h"

void command_generate(Options options)
{
    char* value = options.argv[options.argc-1];

    if(STREQ(value, command_to_string(COMMAND_GENERATE))) {
        ERRO("A value must be specified");
        INFO("Try: %s generate -h", EXECUTABLE_NAME);
        exit(EXIT_FAILURE);
    }
    
    if(
        !STREQ(value, "config") &&
        !STREQ(value, "man") &&
        !STREQ(value, "autocomplete") 
    ) {
        ERRO("'%s' is not a valid value", value);
        INFO("Try: %s generate -h", EXECUTABLE_NAME);
        exit(EXIT_FAILURE);
    }

    if(STREQ(value, "config")){
        char* config = 
            "# ~/.config/.changelogger.yml\n"
            "\n"
            "editor: nvim\n"
            "always-export: true\n"
            "always-push: false\n"
            "release-warning-message: \"Remember to update the version and commit everything important!\"\n"
            "# config-path: /path/to/another/config/to/be/used\n";

        char* default_path = CHANGELOGGER_DEFAULT_CONFIG_PATH;

        if(config_found(default_path)){
            WARN("A config is found at '%s'", default_path);
            INFO("Generated config would be:\n%s", config);
            return;
        }

        clib_write_file(default_path, config, "w");
        free(default_path);
    } else if(STREQ(value, "autocomplete")){
        if (getuid() != 0) {
            PANIC("Installing autocompletion requires elevated privileges. Please run with sudo.\n");
        }

        install_autocompletion(SHELL_ZSH);
        INFO("%s autocomplete installed", shell_to_string(SHELL_ZSH));

        install_autocompletion(SHELL_BASH);
        INFO("%s autocomplete installed", shell_to_string(SHELL_BASH));

        install_autocompletion(SHELL_FISH);
        INFO("%s autocomplete installed", shell_to_string(SHELL_FISH));
    } else {
        PANIC("Generating %s is not implemented yet", value);
    }
}
