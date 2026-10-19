#ifndef RELEASE_UTILS_H
#define RELEASE_UTILS_H

char* update_config_version(const char* release_type);
void insert_release(_Bool pushed);
void mark_release_pushed(const char* version);

#endif // RELEASE_UTILS_H
