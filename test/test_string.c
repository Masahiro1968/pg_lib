#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pg_string.h"
#include "pg_logger.h"
#include "check_functions.h"

/**
 * @brief 最初の領域に対して、pg_string_relplace()が大きく拡張が必要な場合のテスト
 */
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

int main(void)
{
    int rc = 0;

    rc |= assert_value(SUCCESS, string_test_01(), "string_test_01");
    rc |= assert_value(SUCCESS, string_test_02(), "string_test_02");

    return rc;
}
