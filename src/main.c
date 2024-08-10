#define CLIB_IMPLEMENTATION
#include "clib.h"

#define CHANGELOG_FILE "CHANGELOG.md"

int main(int argc, char** argv)
{
    if(!clib_file_exists(CHANGELOG_FILE)) {
        clib_write_file(CHANGELOG_FILE, "# CHANGELOG\n", "w");
    }

    return 0;
}
