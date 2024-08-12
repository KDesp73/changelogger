#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "status.h"
#define TEMPLATE_RELEASE(version, date) \
    clib_format_text("## [%s] - %s", version, date)

#define TEMPLATE_UNRELEASED "## [Unreleased]"

#define TEMPLATE_ENTRY(title) \
    clib_format_text("- %s", title)

char* template_status(Status status);
#define TEMPLATE_STATUS(status) \
    template_status(status)

#define TEMPLATE_RELEASE_LINK(version, link) \
    clib_format_text("[%s](%s)", version, link)

#endif //TEMPLATES_H
