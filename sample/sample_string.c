/**
 * @file    sample_string.c
 * @brief   文字列操作に関するサンプルコード
 * @author  Masahiro1968
 * @date    2026-08-01
 */

#include "pg_string.h"
#include "pg_logger.h"

void sample01()
{
    PGString *string1 = pg_string_new(16);  // 少ないのですぐにrealloc()される
    PGString *string2 = pg_string_new(256); // 余裕があるので足りなくなってrealloc()される

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
}

void sample02()
{
    PGString *path = pg_string_new(1024);
    pg_string_set(path, "/usr/local/bin/postgres");
    PGStringList *parts = pg_string_split(path, '/');
    for (size_t i = 0; i < pg_string_list_size(parts); i++)
    {
        PGString *v = pg_string_list_get(parts, i);
        PG_LOG_INFO("[%zu] %s", i, pg_string_get(v));
    }
    pg_string_list_free(parts);
    pg_string_free(path);
}

void sample03()
{
    PGString *data = pg_string_new(0);
    pg_string_set(data, "val1, val2, val3, val4, val5, val6");
    PGStringList *parts = pg_string_split(data, ',');
    for (size_t i = 0; i < pg_string_list_size(parts); i++)
    {
        PGString *v = pg_string_list_get(parts, i);
        pg_string_trim(v);
        PG_LOG_INFO("[%zu] %s", i, pg_string_get(v));
    }
    pg_string_list_free(parts);
    pg_string_free(data);
}

void sample04()
{
    PGString *base = pg_string_new(0);
    pg_string_set(base, "This is a sample string.");
    int pos = pg_string_find(base, "sample");
    PG_LOG_INFO("found pos=%d", pos);

    pg_string_replace(base, "sample", "test");
    PG_LOG_INFO("replaced %s", pg_string_get(base));

    pg_string_free(base);
}

void sample05()
{
    PGStringList *string_list = pg_string_list_new();

    for (int i = 1; i < 20; i++)
    {
        PGString *value = pg_string_new(8); // あえて少ない領域を用意
        pg_string_format(value, "This is a sample string No.%d", i);
        pg_string_list_add(string_list, value);
    }

    for (int i = 0; i < pg_string_list_size(string_list); i++)
    {
        PGString *value = pg_string_list_get(string_list, i);
        PG_LOG_INFO("value:%s", pg_string_get(value));
    }

    pg_string_list_free(string_list);
}

int main(int argc, char **argv)
{
    pg_log_set_level(PG_LEVEL_DEBUG);

    sample01();
    sample02();
    sample03();
    sample04();
    sample05();
}