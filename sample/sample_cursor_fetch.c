/**
 * @file    sample_cursor_fetch.c
 * @brief   カーソル関数を使用して、10件ずつFETCHするサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLへの接続後、SELECT文(パラメータ付き)でカーソルを作成します。
 *   - 10件ずつFETCHして、情報を取得します。
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_statement.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

int main()
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_cursor_fetch.log";
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
        goto cleanup1;
    }

    if (!pg_begin(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    sql =
        "SELECT "
        "id, int_value, bigint_value, numeric_value, real_value, double_value, "
        "char_value, varchar_value, text_value, date_value, timestamp_value, bool_value "
        "FROM sample_table "
        "WHERE id < $1 "
        "ORDER BY id";
    PG_LOG_INFO("SQL=%s", sql);
    const char *params[] = {"10"};
    ret = pg_open_cursor(ctx, "sample_cursor", sql, 1, params);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    PG_LOG_INFO("================================================");

    for (;;)
    {
        res = pg_query(ctx, "FETCH 10 FROM sample_Cursor");
        if (!pg_ok(res))
        {
            PG_LOG_ERROR(pg_error(ctx));
            //! PGresultを必ず開放する。
            pg_result_free(res);
            goto cleanup2;
        }

        int rows = pg_rows(res);
        if (rows == 0)
        {
            PG_LOG_DEBUG("reached end of data.");
            //! PGresultを必ず開放する。
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

        //! 次のデータをPGresultに格納する前に必ず開放する。
        pg_result_free(res);
    }

    ret = pg_exec(ctx, "CLOSE sample_cursor");
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }

    ret = pg_commit(ctx);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }

    goto cleanup1;

cleanup2:
    pg_rollback(ctx);

cleanup1:
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