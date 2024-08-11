#include <ctype.h>

int is_number(const char *str) 
{
    if (*str == '\0') {
        return 0;
    }

    if (*str == '-') {
        str++;
    }

    int has_digits = 0;
    int has_decimal = 0;

    while (*str) {
        if (isdigit(*str)) {
            has_digits = 1;
        } else if (*str == '.') {
            if (has_decimal) {
                return 0;
            }
            has_decimal = 1;
        } else {
            return 0;
        }
        str++;
    }

    return has_digits; 
}
