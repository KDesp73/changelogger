#ifndef DATE_H
#define DATE_H

#include <stdio.h>

typedef struct {
    size_t year;
    size_t month;
    size_t day;
    size_t hours;
    size_t minutes;
    size_t seconds;

    char* full;
    char* date;
    char* time;
} Date;

void get_date(Date* date);
void free_date(Date* date);
Date parse_date(const char* date);
void log_date(Date date);

#endif // DATE_H
