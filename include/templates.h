#ifndef TEMPLATES_H
#define TEMPLATES_H

#include "extern/str.h"
#include "status.h"

#define TEMPLATE_YANKED "[YANKED]"

#define TEMPLATE_RELEASE(version, date, yanked) \
    str_format("\n## [%s] - %s%s", version, date, (yanked) ? " " TEMPLATE_YANKED : "")

#define TEMPLATE_UNRELEASED "\n## [Unreleased]"

#define TEMPLATE_ENTRY(title) \
    str_format("- %s", title)

char* template_status(Status status);
#define TEMPLATE_STATUS(status) \
    template_status(status)

#define TEMPLATE_RELEASE_LINK(version, link) \
    str_format("[%s]: %s", version, link)

#define TEMPLATE_HEADER "# Changelog\n"


#define TEMPLATE_NOTE \
"All notable changes to this project will be documented in this file.\n" \
"\n" \
"The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html)." \

#endif //TEMPLATES_H
