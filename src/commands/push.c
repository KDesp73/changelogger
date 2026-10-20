#include "commands.h"
#include "database.h"
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
    // Silence gh's output: `gh --version` otherwise prints to our stdout
    // on every push, and `gh auth status` writes its result to stderr,
    // which makes parsing its stdout unreliable across gh versions.
    if (system("gh --version >/dev/null 2>&1") != 0) {
        ERRO("gh-cli is not available on your system.\n");
        return 0;
    }

    if (system("gh auth status >/dev/null 2>&1") != 0) {
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

    char* asset_arg = NULL;
    if (asset != NULL && !is_blank(asset)) {
        asset_arg = shell_quote(asset);
    }

    char* gh_command = NULL;
    if (asset_arg != NULL) {
        gh_command = str_format(
            "gh release create v%s %s -F %s/%s.md -t v%s",
            version,
            asset_arg,
            CHANGELOG_DIR,
            version,
            version
        );
    } else {
        gh_command = str_format(
            "gh release create v%s -F %s/%s.md -t v%s",
            version,
            CHANGELOG_DIR,
            version,
            version
        );
    }

    // gh prints its own errors (stderr stays attached to the terminal),
    // and the release URL on stdout, which we echo back.
    char* out = NULL;
    int status = execute_command_capture(gh_command, &out);
    if (out != NULL) {
        fputs(out, (status == 0) ? stdout : stderr);
        free(out);
    }

    free(asset_arg);
    free(gh_command);

    if (status != 0) {
        ERRO("Could not push release v%s to Github (gh exited with code %d)", version, status);
        return 0;
    }

    return 1;
}

void handle_push_release(const char* version, Options options)
{
    char* version_q = sql_quote(version);
    char* condition = str_format("version = %s", version_q);
    free(version_q);
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
        // push_release() already reported the failure. Leave pushed = 0
        // so the release stays pending and `changelogger push` can be
        // retried once the problem is fixed.
        WARN("Release v%s was not marked as pushed.", version);
    }
    free(condition);
}

