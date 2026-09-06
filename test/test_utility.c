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
    bool ret;

    /* pg_make_data() */
    PGStringList *row = pg_string_list_new();
    for (int i = 0; i < 10; i++)
    {
        PGString *col = pg_string_new(8);
        if (i != 3 && i != 6 && i != 9)
        {
            pg_string_format(col, "%d", i);
        }
        pg_string_list_add(row, col);
    }

    {
        const char *delimiter = ",";
        const char *bracket = "\"";
        const char *eol = "\n";
        const bool null_bracket = false;
        const char *expect = "\"0\",\"1\",\"2\",,\"4\",\"5\",,\"7\",\"8\",\n";
        PGString *response = pg_make_data(row, delimiter, bracket, eol, null_bracket);
        ret |= assert_cmp(expect, pg_string_get(response), "pg_make_data check1");
        pg_string_free(response);
    }

    {
        const char *delimiter = ",";
        const char *bracket = "\"";
        const char *eol = "\n";
        const bool null_bracket = true;
        const char *expect = "\"0\",\"1\",\"2\",\"\",\"4\",\"5\",\"\",\"7\",\"8\",\"\"\n";
        PGString *response = pg_make_data(row, delimiter, bracket, eol, null_bracket);
        ret |= assert_cmp(expect, pg_string_get(response), "pg_make_data check2");
        pg_string_free(response);
    }

    {
        const char *delimiter = ",";
        const char *bracket = "'";
        const char *eol = "\n";
        const bool null_bracket = false;
        const char *expect = "'0','1','2',,'4','5',,'7','8',\n";
        PGString *response = pg_make_data(row, delimiter, bracket, eol, null_bracket);
        ret |= assert_cmp(expect, pg_string_get(response), "pg_make_data check3");
        pg_string_free(response);
    }

    {
        const char *delimiter = ",";
        const char *bracket = "'";
        const char *eol = "\n";
        const bool null_bracket = true;
        const char *expect = "'0','1','2','','4','5','','7','8',''\n";
        PGString *response = pg_make_data(row, delimiter, bracket, eol, null_bracket);
        ret |= assert_cmp(expect, pg_string_get(response), "pg_make_data check3");
        pg_string_free(response);
    }

    pg_string_list_free(row);

    return ret;
}