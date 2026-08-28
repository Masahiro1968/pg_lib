#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pg_string.h"
#include "pg_utility.h"
#include "check_functions.h"

int main(void)
{
    int rc = 0;
    bool ret;

    /* pg_datetime_parse() */
    const char *date_time = "2024-03-16 18:10:13";
    PGDateTime val;
    pg_datetime_parse(date_time, &val);
    rc |= assert_value(2024, val.year, "pg_datetime_parse year");
    rc |= assert_value(3, val.month, "pg_datetime_parse month");
    rc |= assert_value(16, val.day, "pg_datetime_parse day");
    rc |= assert_value(18, val.hour, "pg_datetime_parse hour");
    rc |= assert_value(10, val.minute, "pg_datetime_parse minute");
    rc |= assert_value(13, val.second, "pg_datetime_parse second");

    return rc;
}