/**
 * @file    sample_make_data.c
 * @brief   扱う型を定義したテーブルを作成し、データを格納するサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - このライブラリで扱う型を定義したテーブルを作成します。
 *   - テーブルにダミーデータを生成して格納します。
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_make_data.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    PG_LOG_DEBUG("start main()");

    pg_init();

    bool ret;
    PGContext *ctx = NULL;
    char *sql1 = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    sql1 = "DROP TABLE IF EXISTS sample_table;";
    PG_LOG_INFO("SQL1=%s", sql1);
    ret = pg_exec(ctx, sql1);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    sql1 =
        "CREATE TABLE sample_table ("
        "   id              INTEGER PRIMARY KEY,"
        "   int_value       INTEGER,"
        "   bigint_value    BIGINT,"
        "   numeric_value   NUMERIC(8,5),"
        "   real_value      REAL,"
        "   double_value    DOUBLE PRECISION,"
        "   char_value      CHAR(10),"
        "   varchar_value   VARCHAR(100),"
        "   text_value      TEXT,"
        "   date_value      DATE,"
        "   timestamp_value TIMESTAMP,"
        "   bool_value      BOOLEAN"
        ");";
    PG_LOG_INFO("SQL1=%s", sql1);
    ret = pg_exec(ctx, sql1);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    char sql2[1024];

    for (int i = 1; i <= 100; i++)
    {
        snprintf(
            sql2,
            sizeof(sql2),
            "INSERT INTO sample_table ("
            "id,"
            "int_value,"
            "bigint_value,"
            "numeric_value,"
            "real_value,"
            "double_value,"
            "char_value,"
            "varchar_value,"
            "text_value,"
            "date_value,"
            "timestamp_value,"
            "bool_value"
            ") VALUES ("
            "%d,"
            "%d,"
            "%lld,"
            "%.5f,"
            "%.2f,"
            "%.10f,"
            "'C%02d',"
            "'VARCHAR-%03d',"
            "'TEXT-%03d',"
            "'2026-07-%02d',"
            "'2026-07-%02d %02d:%02d:%02d',"
            "%s"
            ");",
            i,                         /* id */
            i * 10,                    /* int_value */
            (long long)i * 1000000LL,  /* bigint_value */
            i * 0.12345,               /* numeric_value */
            i * 1.25f,                 /* real_value */
            i * 123.456789,            /* double_value */
            i,                         /* char_value */
            i,                         /* varchar_value */
            i,                         /* text_value */
            (i % 28) + 1,              /* date_value */
            (i % 28) + 1,              /* timestamp day */
            i % 24,                    /* hour */
            i % 60,                    /* minute */
            (i * 3) % 60,              /* second */
            (i % 2) ? "TRUE" : "FALSE" /* bool_value */
        );
        PG_LOG_INFO("SQL2=%s", sql2);

        ret = pg_exec(ctx, sql2);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            goto cleanup;
        }
    }

cleanup:
    pg_disconnect(ctx);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}