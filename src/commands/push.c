#include "commands.h"
#include "database.h"
#include "extern/clib.h"
#include "utils.h"
#include "push_utils.h"

void command_push(Options options)
{
    char* version = NULL;
    if(version_full_set(options)) {
        version = options.version.full;
    } else {
        sqlite3* db;
        sqlite3_open(SQLITE_DB, &db);
        version = select_version_full(db);
        sqlite3_close(db);
    }

    handle_push_release(version, options);
}

#define DEFAULT_WARNING_MESSAGE \
    "Remember to commit and update " \
    "all the necessary parts of the " \
    "project before pushing the release."

void make_sure_user_wants_to_proceed_with_releasing(Options options)
{
    if(!options.yes) {
        Config config = get_config();
        WARN("%s", (config.release_warning_message == NULL) ? DEFAULT_WARNING_MESSAGE : config.release_warning_message);
        int y_n = yes_or_no("Continue?");

        if(!y_n) {
            INFO("Aborting...");
            exit(0);
        }
    }
}

int is_gh_cli_available()
{
    int status = system("gh --version");
    if (status != 0) {
        ERRO("gh-cli is not available on your system.\n");
        return 0;
    }

    char buffer[1024];
    FILE *fp = popen("gh auth status", "r");
    if (fp == NULL) {
        ERRO("Failed to run gh auth status command.\n");
        return 0;
    }

    // Initialize a variable to track if "Logged in" was found
    int logged_in = 0;

    // Read each line of output
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Check if the line contains "Logged in"
        if (strstr(buffer, "Logged in") != NULL) {
            logged_in = 1;  // Set flag if logged in
            break; // We found the necessary information, no need to continue
        }
    }
    
    pclose(fp);

    // Check if we were logged in
    if (!logged_in) {
        ERRO("gh-cli is not properly set up. Please run 'gh auth login' to authenticate.\n");
        return 0;
    }

    return 1;
}

int push_release(const char* version, const char* asset)
{
    if (!is_gh_cli_available()){
        exit(1);
    }

    command_export((Options){0});

    char* asset_str = clib_format_text(" '%s' ", asset);
    char* gh_command = clib_format_text(
        "gh release create v%s%s-F %s/%s.md -t v%s",
        version, 
        (asset) ? asset_str : " ", 
        CHANGELOG_DIR, 
        version, 
        version
    );
    printf("%s\n", gh_command);
    clib_execute_command(gh_command);
    char* rc = clib_execute_command("echo $?");
    free(asset_str);
    free(gh_command);

    return STREQ(rc, "0");
}

void handle_push_release(const char* version, Options options)
{
    char* condition = clib_format_text("version = '%s'", version);
    int pushed = select_int(TABLE_RELEASES, RELEASES_PUSHED, condition);

    if(pushed) {
        WARN("This release is already pushed. Aborting.");
        free(condition);
        return;
    }

    make_sure_user_wants_to_proceed_with_releasing(options);
    if(push_release(version, options.asset)){
        update(TABLE_RELEASES, RELEASES_PUSHED, "1", condition);
    } else {
        // ERRO("Could not push release to Github");
    }
    free(condition);
}

