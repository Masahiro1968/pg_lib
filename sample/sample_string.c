/**
 * @file    sample_string.c
 * @brief   文字列操作に関するサンプルコード
 * @author  Masahiro1968
 * @date    2026-08-01
 */

#include "pg_string.h"
#include "pg_logger.h"

int main(int argc, char **argv)
{
    pg_log_set_level(PG_LEVEL_DEBUG);

    PGString *string1 = pg_string_new(0);
    PGString *string2 = pg_string_new(256);

    pg_string_set(string1, "   ABCDEFGHIJKLMN   ");
    pg_string_set(string2, "   abcdefghijklmn   ");

    const char *test1 = pg_string_get(string1);
    const int test1_len = pg_string_size(string1);
    PG_LOG_INFO("string1 value is %s, size is %d", test1, test1_len);

    pg_string_trim_right(string1);
    const char *test1T = pg_string_get(string1);
    const int test1_lenT = pg_string_size(string1);
    PG_LOG_INFO("string1(trimed) value is %s, size is %d", test1T, test1_lenT);

    const char *test2 = pg_string_get(string2);
    const int test2_len = pg_string_size(string2);
    PG_LOG_INFO("string2 value is %s, size is %d", test2, test2_len);

    pg_string_trim_left(string2);
    const char *test2T = pg_string_get(string2);
    const int test2_lenT = pg_string_size(string2);
    PG_LOG_INFO("string2(trimed) value is %s, size is %d", test2T, test2_lenT);

    const int join_size = pg_string_join(string1, string2);
    const char *join = pg_string_get(string1);
    const int join_len = pg_string_size(string1);
    PG_LOG_INFO("string1 value is %s, size is %d", join, join_len);

    pg_string_trim(string1);
    const char *joinT = pg_string_get(string1);
    const int join_lenT = pg_string_size(string1);
    PG_LOG_INFO("string1(join and trimed) value is %s, size is %d", joinT, join_lenT);

    pg_string_free(string2);
    pg_string_free(string1);

    {
        PGString *path = pg_string_new(0);
        pg_string_set(path, "/usr/local/bin/postgres");
        PGStringList parts = pg_string_split(path, '/');
        for (size_t i = 0; i < parts.count; i++)
        {
            PG_LOG_INFO("[%zu] %s", i, pg_string_get(parts.items[i]));
        }
        pg_string_list_free(&parts);
        pg_string_free(path);
    }

    {
        PGString *data = pg_string_new(0);
        pg_string_set(data, "val1, val2, val3, val4, val5, val6");
        PGStringList parts = pg_string_split(data, ',');
        for (size_t i = 0; i < parts.count; i++)
        {
            PGString *v = parts.items[i];
            pg_string_trim(v);
            PG_LOG_INFO("[%zu] %s", i, pg_string_get(v));
        }
        pg_string_list_free(&parts);
        pg_string_free(data);
    }

    {
        PGString *base = pg_string_new(0);
        pg_string_set(base, "This is a sample string.");
        int pos = pg_string_find(base, "sample");
        PG_LOG_INFO("%d", pos);

        pg_string_replace(base, "sample", "test");
        PG_LOG_INFO("%s", pg_string_get(base));

        pg_string_free(base);
    }
}