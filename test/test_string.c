#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "pg_string.h"
#include "pg_logger.h"
#include "check_functions.h"

int string_test_01()
{
    int ret = 0;

    PGString *string = pg_string_new(16);

    int base_size = pg_string_set(string, "This is a sample string.");
    ret |= assert_value(24, base_size, "base_size check");

    int replace_size = pg_string_replace(string, "sample", "test test test test test test");
    ret |= assert_cmp(
        "This is a test test test test test test string.",
        pg_string_get(string), "replace text check");
    ret |= assert_value(47, replace_size, "replace_size check");

    pg_string_free(string);

    return ret;
}

int string_test_02()
{
    int ret = 0;

    PGString *string = pg_string_new(16);

    int base_size = pg_string_set(string, "This is a sample string.");
    ret |= assert_value(24, base_size, "base_size check");

    int pos = pg_string_find(string, "sample");
    ret |= assert_value(10, pos, "find text check");

    pg_string_free(string);

    return ret;
}

int string_test_03()
{
    int ret = 0;

    PGString *string1 = pg_string_new(16);  // 少ないのですぐにrealloc()される
    PGString *string2 = pg_string_new(256); // 余裕があるので足りなくなってからrealloc()される

    pg_string_set(string1, "   ABCDEFGHIJKLMN   ");
    pg_string_set(string2, "   abcdefghijklmn   ");

    pg_string_trim_right(string1);
    ret |= assert_cmp("   ABCDEFGHIJKLMN", pg_string_get(string1),
                      "pg_string_trim_right check");
    ret |= assert_value(17, pg_string_size(string1),
                        "string size after pg_string_trim_right check");

    pg_string_trim_left(string2);
    ret |= assert_cmp("abcdefghijklmn   ", pg_string_get(string2),
                      "pg_string_trim_left check");
    ret |= assert_value(17, pg_string_size(string1),
                        "string size after pg_string_trim_left check");

    const int join_size = pg_string_join(string1, string2);
    ret |= assert_cmp("   ABCDEFGHIJKLMNabcdefghijklmn   ", pg_string_get(string1),
                      "pg_string_join check");
    ret |= assert_value(34, pg_string_size(string1),
                        "string size after pg_string_join check");

    pg_string_trim(string1);
    ret |= assert_cmp("ABCDEFGHIJKLMNabcdefghijklmn", pg_string_get(string1),
                      "pg_string_trim check");
    ret |= assert_value(28, pg_string_size(string1),
                        "string size after pg_string_trim check");

    pg_string_free(string2);
    pg_string_free(string1);

    return ret;
}

int string_test_04()
{
    int ret = 0;

    PGString *path = pg_string_new(PATH_MAX);
    pg_string_set(path, "/usr/local/bin/postgres");
    PGStringList *parts = pg_string_split(path, '/');

    ret |= assert_cmp("usr", pg_string_get(pg_string_list_get(parts, 0)),
                      "pg_string_split #1 check");
    ret |= assert_cmp("local", pg_string_get(pg_string_list_get(parts, 1)),
                      "pg_string_split #2 check");
    ret |= assert_cmp("bin", pg_string_get(pg_string_list_get(parts, 2)),
                      "pg_string_split #3 check");
    ret |= assert_cmp("postgres", pg_string_get(pg_string_list_get(parts, 3)),
                      "pg_string_split #4 check");

    pg_string_list_free(parts);
    pg_string_free(path);

    return ret;
}

int string_test_05()
{
    int ret = 0;

    PGStringList *string_list = pg_string_list_new();

    for (int i = 1; i <= 100; i++)
    {
        PGString *value = pg_string_new(8); // あえて少ない領域を用意
        pg_string_format(value, "This is a sample string No.%d", i);
        pg_string_list_add(string_list, value);
    }

    for (int i = 0; i < pg_string_list_size(string_list); i++)
    {
        char expect[512];
        snprintf(expect, sizeof(expect), "This is a sample string No.%d", i + 1);

        PGString *value = pg_string_list_get(string_list, i);
        ret |= assert_cmp(expect, pg_string_get(value),
                          "pg_string_format check");
    }

    pg_string_list_free(string_list);

    return ret;
}

int string_test_06()
{
    int ret = 0;

    PGString *value1 = pg_string_new(8);
    PGString *value2 = pg_string_new(8);

    pg_string_format(value1, "%2.4f", 1.2);
    ret |= assert_cmp("1.2000", pg_string_get(value1),
                      "pg_string_format check");

    pg_string_format(value2, "%d", 1200);
    ret |= assert_cmp("1200", pg_string_get(value2),
                      "pg_string_format check");

    pg_string_trim_trailing_zeros(value1);
    ret |= assert_cmp("1.2", pg_string_get(value1),
                      "pg_string_trim_trailing_zeros check");

    pg_string_trim_trailing_zeros(value2);
    ret |= assert_cmp("1200", pg_string_get(value2),
                      "pg_string_trim_trailing_zeros check");

    pg_string_free(value1);
    pg_string_free(value2);

    return ret;
}

int string_test_07()
{
    int ret = 0;

    PGString *value = pg_string_new(8);
    pg_string_set(value, "This string contains UPPERCASE side characters.");

    pg_string_to_upper(value);
    ret |= assert_cmp("THIS STRING CONTAINS UPPERCASE SIDE CHARACTERS.", pg_string_get(value),
                      "pg_string_to_upper check");
    pg_string_to_lower(value);
    ret |= assert_cmp("this string contains uppercase side characters.", pg_string_get(value),
                      "pg_string_to_lowerr check");

    PGString *mid_string = pg_string_mid(value, 21, 9);
    ret |= assert_cmp("uppercase", pg_string_get(mid_string),
                      "pg_string_mid check");
    pg_string_free(mid_string);

    PGString *left_string = pg_string_left(value, 11);
    ret |= assert_cmp("this string", pg_string_get(left_string),
                      "pg_string_left check");
    pg_string_free(left_string);

    PGString *right_string = pg_string_right(value, 11);
    ret |= assert_cmp("characters.", pg_string_get(right_string),
                      "pg_string_right check");
    pg_string_free(right_string);

    int count = pg_string_reverse_find(value, "side");
    ret |= assert_value(31, count, "pg_string_reverse_find");

    pg_string_free(value);

    return ret;
}

int string_test_08()
{
    int ret = 0;

    PGString *value = pg_string_new(128);
    pg_string_set(value, "1,\"val2\",3,'val4'");
    PGStringList *parts = pg_string_split(value, ',');
    PGString *part;
    int response;

    part = pg_string_list_get(parts, 0);
    response = pg_string_unquote(part, '"');
    ret |= assert_value(0, response, "pg_string_unquote response");
    ret |= assert_cmp("1", pg_string_get(pg_string_list_get(parts, 0)),
                      "pg_string_split #1 check");

    part = pg_string_list_get(parts, 1);
    response = pg_string_unquote(part, '"');
    ret |= assert_value(1, response, "pg_string_unquote response");
    ret |= assert_cmp("val2", pg_string_get(pg_string_list_get(parts, 1)),
                      "pg_string_split #2 check");

    part = pg_string_list_get(parts, 2);
    response = pg_string_unquote(part, '"');
    ret |= assert_value(0, response, "pg_string_unquote response");
    ret |= assert_cmp("3", pg_string_get(pg_string_list_get(parts, 2)),
                      "pg_string_split #3 check");

    part = pg_string_list_get(parts, 3);
    response = pg_string_unquote(part, '\'');
    ret |= assert_value(1, response, "pg_string_unquote response");
    ret |= assert_cmp("val4", pg_string_get(pg_string_list_get(parts, 3)),
                      "pg_string_split #4 check");

    pg_string_list_free(parts);
    pg_string_free(value);

    return ret;
}

int string_test_09()
{
    int ret = 0;

    PGString *value = pg_string_new(128);
    pg_string_set(value, "1,,3,");
    PGStringList *parts = pg_string_split(value, ',');

    ret |= assert_cmp("1", pg_string_get(pg_string_list_get(parts, 0)),
                      "pg_string_split #1 check");
    ret |= assert_cmp("", pg_string_get(pg_string_list_get(parts, 1)),
                      "pg_string_split #2 check");
    ret |= assert_cmp("3", pg_string_get(pg_string_list_get(parts, 2)),
                      "pg_string_split #3 check");
    ret |= assert_cmp("", pg_string_get(pg_string_list_get(parts, 3)),
                      "pg_string_split #4 check");

    pg_string_list_free(parts);
    pg_string_free(value);

    return ret;
}

int string_test_10()
{
    int ret = 0;
    bool result = false;

    PGString *match1 = pg_string_new(12);
    pg_string_set(match1, "0123456789");
    
    result = pg_string_starts_with(match1, "0123");
    ret |= assert_value(true, result, "pg_string_starts_with(true) #1");
    
    result = pg_string_ends_with(match1, "6789");
    ret |= assert_value(true, result, "pg_string_ends_with(true) #1");

    pg_string_free(match1);

    PGString *match2 = pg_string_new(12);
    pg_string_set(match2, "1234567890");
    
    result = pg_string_starts_with(match2, "0123");
    ret |= assert_value(false, result, "pg_string_starts_with(true) #2");
    
    result = pg_string_ends_with(match2, "6789");
    ret |= assert_value(false, result, "pg_string_ends_with(true) #2");

    pg_string_free(match2);
    
    return ret;
}

int main(void)
{
    int rc = 0;

    rc |= assert_value(SUCCESS, string_test_01(), "string_test_01");
    rc |= assert_value(SUCCESS, string_test_02(), "string_test_02");
    rc |= assert_value(SUCCESS, string_test_03(), "string_test_03");
    rc |= assert_value(SUCCESS, string_test_04(), "string_test_04");
    rc |= assert_value(SUCCESS, string_test_05(), "string_test_05");
    rc |= assert_value(SUCCESS, string_test_06(), "string_test_06");
    rc |= assert_value(SUCCESS, string_test_07(), "string_test_07");
    rc |= assert_value(SUCCESS, string_test_08(), "string_test_08");
    rc |= assert_value(SUCCESS, string_test_09(), "string_test_09");
    rc |= assert_value(SUCCESS, string_test_10(), "string_test_10");

    return rc;
}
