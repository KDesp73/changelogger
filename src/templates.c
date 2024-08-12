#include "templates.h"


char* template_status(Status status)
{
    switch (status) {
    case STATUS_UNSET:
        return "### Unset";
    case STATUS_UNKNOWN:
        return "### Unknown";
    case STATUS_NONE:
        return "### None";
    case STATUS_CHANGED:
        return "### Changed";
    case STATUS_ADDED:
        return "### Added";
    case STATUS_DEPRECATED:
        return "### Deprecated";
    case STATUS_REMOVED:
        return "### Removed";
    case STATUS_FIXED:
        return "### Fixed";
    case STATUS_SECURITY:
        return "### Security";
      break;
    }
}
