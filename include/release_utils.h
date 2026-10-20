#ifndef RELEASE_UTILS_H
#define RELEASE_UTILS_H

/*
 * Creates a new release and moves the unreleased entries to it in one
 * transaction. Returns the new version, which the caller must free.
 */
char* create_release(const char* release_type);

/*
 * `set -V <version>`: stores the version in Config and creates its release
 * row in one transaction.
 */
void create_release_for_version(const char* version);

void mark_release_pushed(const char* version);

#endif // RELEASE_UTILS_H
