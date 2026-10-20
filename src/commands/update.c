#include "commands.h"
#include "config.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Directory of the binary that is being run. argv[0] is used when it contains
 * a path, so `./changelogger update` updates the local build and not whatever
 * happens to come first in PATH.
 */
static char* executable_dir(const char* argv0)
{
    char* path = NULL;

    if (argv0 != NULL && strchr(argv0, '/') != NULL) {
        if (argv0[0] == '/') {
            path = str_format("%s", argv0);
        } else {
            char cwd[4096];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                path = str_format("%s/%s", cwd, argv0);
            }
        }
    } else {
        path = find_executable(EXECUTABLE_NAME);
    }

    if (path == NULL) {
        return NULL;
    }

    char* slash = strrchr(path, '/');
    if (slash == NULL) {
        free(path);
        return NULL;
    }

    if (slash == path) {
        slash[1] = '\0'; // Binary at the filesystem root
    } else {
        *slash = '\0';
    }

    return path;
}

/*
 * Downloads and runs the configure script.
 *
 * The script is downloaded to a temporary file instead of using bash's
 * process substitution, because execute_command_capture() runs commands with
 * /bin/sh (dash on Debian/Ubuntu does not support <(...)). curl -f also makes
 * a missing script a hard failure instead of running an empty one.
 */
void command_update(Options options)
{
    const char* url = "https://raw.githubusercontent.com/KDesp73/changelogger/refs/heads/main/configure";

    // Update the binary that is actually being invoked, not just the first one
    // in PATH.
    char* bindir_arg = strdup("");
    char* argv0 = (options.argc > 0 && options.argv != NULL) ? options.argv[0] : NULL;
    char* bindir = executable_dir(argv0);
    if (bindir != NULL) {
        char* quoted = shell_quote(bindir);
        if (quoted != NULL) {
            char* arg = str_format(" --bindir %s", quoted);
            if (arg != NULL) {
                free(bindir_arg);
                bindir_arg = arg;
            }
            free(quoted);
        }
        free(bindir);
    }

    char* url_q = shell_quote(url);
    char* command = str_format(
        "tmp=$(mktemp) && trap 'rm -f \"$tmp\"' EXIT && "
        "curl -fsSL %s -o \"$tmp\" && bash \"$tmp\" update%s",
        url_q,
        bindir_arg
    );
    free(url_q);
    free(bindir_arg);

    if (command == NULL) {
        PANIC("Could not build the update command");
    }

    char* output = NULL;
    int status = execute_command_capture(command, &output);
    if (output != NULL && output[0] != '\0') {
        fputs(output, (status == 0) ? stdout : stderr);
    }
    free(output);
    free(command);

    if (status != 0) {
        ERRO("Failed to update %s", EXECUTABLE_NAME);
        exit(1);
    }

    INFO("%s updated successfully", EXECUTABLE_NAME);
}
