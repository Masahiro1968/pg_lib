/**
 * @file    sample_cursor.c
 * @brief   カーソルを定義したSQLで、10件ずつFETCHするサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - DECLARE CURSORでカーソルを作成します。
 *   - FETCH 10 で１０件ずつデータを取得します。
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

int main()
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_cursor.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    PG_LOG_DEBUG("start main()");

    pg_init();

    bool ret;
    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    if (!pg_begin(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    sql =
        "DECLARE sample_cursor CURSOR FOR "
        "SELECT "
        "id, int_value, bigint_value, numeric_value, real_value, double_value, "
        "char_value, varchar_value, text_value, date_value, timestamp_value, bool_value "
        "FROM sample_table ORDER BY id";
    PG_LOG_INFO("SQL=%s", sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto rollback;
    }

    PG_LOG_INFO("================================================");

    for (;;)
    {
        sql = "FETCH 10 FROM sample_cursor";
        PG_LOG_INFO("SQL=%s", sql);
        res = pg_query(ctx, sql);
        if (!pg_ok(res))
        {
            PG_LOG_ERROR(pg_error(ctx));
            pg_result_free(res);
            goto rollback;
        }

        int rows = pg_rows(res);
        if (rows == 0)
        {
            PG_LOG_DEBUG("reached end of data.");
            pg_result_free(res);
            break;
        }

        int cols = pg_cols(res);
        static int header = 0;
        if (!header)
        {
            char line[4096] = "";
            for (int col = 0; col < cols; col++)
            {
                char tmp[128];
                snprintf(tmp, sizeof(tmp),
                         "%s(%s)[%d]\t",
                         pg_col_name(res, col),
                         pg_col_type_name(res, col),
                         pg_col_type(res, col));
                strcat(line, tmp);
            }
            PG_LOG_INFO(line);
            PG_LOG_INFO("================================================");
            header = 1;
        }

        for (int row = 0; row < rows; row++)
        {
            char line[4096] = "";
            for (int col = 0; col < cols; col++)
            {
                char tmp[128];
                snprintf(tmp, sizeof(tmp),
                         "%s\t", pg_value(res, row, col));
                strcat(line, tmp);
            }
            PG_LOG_INFO(line);
        }

        pg_result_free(res);
    }

    sql = "CLOSE sample_cursor";
    PG_LOG_INFO("SQL=%s", sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto rollback;
    }

    pg_commit(ctx);

    goto cleanup;

rollback:
    pg_rollback(ctx);

cleanup:
    if (ctx)
    {
        pg_disconnect(ctx);
    }

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}