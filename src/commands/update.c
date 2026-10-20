#include "commands.h"
#include "config.h"
#include "utils.h"

void command_update(Options options)
{
    char* output = NULL;
    int status = execute_command_capture(
        "bash <(curl -s https://raw.githubusercontent.com/KDesp73/changelogger/refs/heads/main/config.sh) update",
        &output
    );
    free(output);

    if (status != 0) {
        ERRO("Failed to update %s", EXECUTABLE_NAME);
        exit(1);
    }

    INFO("%s updated successfully", EXECUTABLE_NAME);
}
