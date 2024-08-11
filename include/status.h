#ifndef STATUS_H
#define STATUS_H

typedef enum {
    STATUS_UNSET = -2,
    STATUS_UNKNOWN = -1,
    STATUS_CHANGED,
    STATUS_ADDED,
    STATUS_DEPRECATED,
    STATUS_REMOVED,
    STATUS_FIXED,
    STATUS_SECURITY
} Status;

char* status_to_string(Status status);
Status get_status(char* status);

#endif // STATUS_H
