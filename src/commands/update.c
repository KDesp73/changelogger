#include "commands.h"
#include "config.h"
#include "extern/clib.h"

void command_update(Options options)
{
    clib_execute_command("bash <(curl -s https://raw.githubusercontent.com/KDesp73/changelogger/refs/heads/main/config.sh) update");

    INFO("%s updated successfully", EXECUTABLE_NAME);
    return;
}
