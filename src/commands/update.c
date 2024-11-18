#include "commands.h"
#include "config.h"
#include "extern/clib.h"

void command_update(Options options)
{
    clib_execute_command("bash <(curl -s https://raw.githubusercontent.com/KDesp73/changelogger/refs/heads/main/config.sh) update");
    char* rc = clib_execute_command("echo $?");

    if(STREQ(rc, "0")){
        INFO("%s updated successfully", EXECUTABLE_NAME);
        return;
    }

    ERRO("Could not update %s", EXECUTABLE_NAME);
}
