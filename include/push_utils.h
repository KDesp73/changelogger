#ifndef PUSH_UTILS_H
#define PUSH_UTILS_H

#include "options.h"
void make_sure_user_wants_to_proceed_with_releasing(Options options);
int is_gh_cli_available();
void push_release(const char* version, const char* asset);
void handle_push_release(const char* version, Options options);


#endif // PUSH_UTILS_H
