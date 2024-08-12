#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "status.h"
#define TEMPLATE_RELEASE(version, date) \
    clib_format_text("\n## [%s] - %s", version, date)

#define TEMPLATE_UNRELEASED "\n## [Unreleased]"

#define TEMPLATE_ENTRY(title) \
    clib_format_text("- %s", title)

char* template_status(Status status);
#define TEMPLATE_STATUS(status) \
    template_status(status)

#define TEMPLATE_RELEASE_LINK(version, link) \
    clib_format_text("[%s]: %s", version, link)

#define TEMPLATE_HEADER "# Changelog\n"

#define TEMPLATE_NOTE \
"All notable changes to this project will be documented in this file.\n" \
"\n" \
"The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)." \

#endif //TEMPLATES_H
