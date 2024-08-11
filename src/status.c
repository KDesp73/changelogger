#include "status.h"
#include "clib.h"
#include <stdlib.h>

char* status_to_string(Status status)
{
    switch (status) {
    case STATUS_CHANGED:
        return "changed";
    case STATUS_ADDED:
        return "added";
    case STATUS_DEPRECATED:
        return "deprecated";
    case STATUS_REMOVED:
        return "removed";
    case STATUS_FIXED:
        return "fixed";
    case STATUS_SECURITY:
        return "security";
    default:
        break;
    }
    return NULL;
}



Status get_status(char* status)
{
#define COMPARE_AND_RETURN_STATUS(s) \
    else if (STREQ(status, status_to_string(s))) return s;

    if(status == NULL) return STATUS_UNSET;
    COMPARE_AND_RETURN_STATUS(STATUS_ADDED)
    COMPARE_AND_RETURN_STATUS(STATUS_FIXED)
    COMPARE_AND_RETURN_STATUS(STATUS_DEPRECATED)
    COMPARE_AND_RETURN_STATUS(STATUS_CHANGED)
    COMPARE_AND_RETURN_STATUS(STATUS_SECURITY)
    else return STATUS_UNKNOWN;

 #undef COMPARE_AND_RETURN_STATUS
}