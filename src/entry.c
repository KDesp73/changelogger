#include "entry.h"
#include "extern/clib.h"

void log_entry(Entry e)
{
    printf("Message: %s\n", e.message);
    printf("Version: %s\n", (STREQ(e.version.full, "0.0.0") ? "unknown" : e.version.full));
    printf("Status: %s\n", status_to_string(e.status));
    printf("Date: %s\n", e.date.full);
}
