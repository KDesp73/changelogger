#include "commands.h"
#include "database.h"
#include "utils.h"
#include "release_utils.h"

void command_set(Options options)
{
    if(version_full_set(options)){
        if(STREQ("0.0.0", options.version.full) || STREQ("unreleased", options.version.full)) {
            ERRO("Version can't be 'unreleased'");
        } else {
            update_version_full(options.version.full);
            insert_release(1);
        }
    }

    if(always_export_set(options)){
        char* value = clib_format_text("%d", options.always_export);
        update(TABLE_CONFIG, CONFIG_ALWAYS_EXPORT, value, CONFIG_CONDITION);
        free(value);
    }

    if(always_push_set(options)){
        char* value = clib_format_text("%d", options.always_push);
        update(TABLE_CONFIG, CONFIG_ALWAYS_PUSH, value, CONFIG_CONDITION);
        free(value);
    }

    if(!is_blank(options.config_path)){
        char* value = clib_format_text("'%s'", options.config_path);
        update(TABLE_CONFIG, CONFIG_CONFIG_PATH, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.config_path != NULL) // Only when it set blank by the user
            ERRO("Config path must not be blank");
    }

    if(!is_blank(options.remote_repo)) {
        char* value = clib_format_text("'%s'", options.remote_repo);
        update(TABLE_CONFIG, CONFIG_REMOTE_REPO, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.remote_repo != NULL) // Only when it set blank by the user
            ERRO("Remote repo must not be blank");
    }

    if(!is_blank(options.editor)) {
        char* value = clib_format_text("'%s'", options.editor);
        update(TABLE_CONFIG, CONFIG_EDITOR, value, CONFIG_CONDITION);
        free(value);
    } else {
        if(options.editor != NULL) // Only when it set blank by the user
            ERRO("Remote repo must not be blank");
    }
}

