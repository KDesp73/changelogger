#include "entry.h"
#include <stdlib.h>
#include <time.h>

void get_date(Date* date)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    date->year = tm_info->tm_year + 1900; // tm_year is years since 1900
    date->month = tm_info->tm_mon + 1;    // tm_mon is 0-11
    date->day = tm_info->tm_mday;
    date->hours = tm_info->tm_hour;
    date->minutes = tm_info->tm_min;
    date->seconds = tm_info->tm_sec;

    date->full = (char*)malloc(100 * sizeof(char)); // Adjust size as needed
    date->date = (char*)malloc(20 * sizeof(char)); // Adjust size as needed
    date->time = (char*)malloc(20 * sizeof(char)); // Adjust size as needed

    snprintf(date->full, 100, "%04zu-%02zu-%02zu %02zu:%02zu:%02zu", 
             date->year, date->month, date->day, 
             date->hours, date->minutes, date->seconds);
    snprintf(date->date, 20, "%04zu-%02zu-%02zu", 
             date->year, date->month, date->day);
    snprintf(date->time, 20, "%02zu:%02zu:%02zu", 
             date->hours, date->minutes, date->seconds);}

void free_date(Date* date) 
{
    // Free allocated memory
    free(date->full);
    free(date->date);
    free(date->time);
}

void log_date(Date date)
{
    printf("Full: %s\n", date.full);
    printf("Date: %s\n", date.date);
    printf("Time: %s\n", date.time);
    printf("Seconds: %zu\n", date.seconds);
    printf("Minutes: %zu\n", date.minutes);
    printf("Hours: %zu\n", date.hours);
    printf("Day: %zu\n", date.day);
    printf("Month: %zu\n", date.month);
    printf("Year: %zu\n", date.year);
}
