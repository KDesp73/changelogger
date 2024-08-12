#ifndef STATUS_H
#define STATUS_H

// IMPORTANT: Do not change the order!
typedef enum {
    STATUS_UNSET = -2,
    STATUS_UNKNOWN = -1,
    STATUS_NONE,
    STATUS_ADDED,
    STATUS_CHANGED,
    STATUS_REMOVED,
    STATUS_FIXED,
    STATUS_DEPRECATED,
    STATUS_SECURITY
} Status;

char* status_to_string(Status status);
Status get_status(char* status);

#endif // STATUS_H
