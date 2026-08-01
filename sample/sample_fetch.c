/**
 * @file    sample_fetch.c
 * @brief   各フィールドの型に合わせた変数に値を格納するサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - SELECT文の各列の型に応じた変数を用意します。
 *   - データはその型に変換して値を格納します。
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "pg_lib.h"
#include "pg_utility.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_fetch.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    PG_LOG_DEBUG("start main()");

    pg_init();

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    int id;
    int int_value;
    int64_t bigint_value;
    char numeric_value[20];
    float real_value;
    double double_value;
    char char_value[11];
    char varchar_value[101];
    char text_value[64];
    char date_value[11];
    char timestamp_value[20];
    bool bool_value;

    PGField fields[] = {
        PG_DEFINE_INT(id),
        PG_DEFINE_INT(int_value),
        PG_DEFINE_INT64(bigint_value),
        PG_DEFINE_NUMERIC(numeric_value),
        PG_DEFINE_FLOAT(real_value),
        PG_DEFINE_DOUBLE(double_value),
        PG_DEFINE_CHAR(char_value),
        PG_DEFINE_TEXT(varchar_value),
        PG_DEFINE_TEXT(text_value),
        PG_DEFINE_DATE(date_value),
        PG_DEFINE_TIMESTAMP(timestamp_value),
        PG_DEFINE_BOOL(bool_value)};

    sql =
        "SELECT "
        "id, int_value, bigint_value, numeric_value, real_value, double_value, "
        "char_value, varchar_value, text_value, date_value, timestamp_value, bool_value "
        "FROM sample_table ORDER BY id";
    PG_LOG_INFO("SQL=%s", sql);
    res = pg_query(ctx, sql);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    for (int row = 0; row < pg_rows(res); row++)
    {
        pg_fetch_row(res, row, fields, ARRAY_SIZE(fields));
        PG_LOG_INFO("%3d %4d %9ld %-10s %f %f %s %s %s %s %s %d",
                    id, int_value, bigint_value, numeric_value, real_value, double_value,
                    char_value, varchar_value, text_value, date_value, timestamp_value, bool_value);
    }

cleanup:
    pg_result_free(res);
    pg_disconnect(ctx);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}