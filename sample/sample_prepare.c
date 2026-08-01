/**
 * @file    sample_prepare.c
 * @brief   SQL文を事前準備して、パラメータでSQLを実行するサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - パラメータ付きSELECT文を事前に準備します。
 *   - パラメータの値を渡してSQLを実行します。
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
    const char *log_file = "./sample_prepare.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    PG_LOG_DEBUG("start main()");

    pg_init();

    PGContext *ctx = NULL;
    PGStmt *stmt = NULL;
    PGresult *res = NULL;
    char *sql = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    sql =
        "SELECT "
        "id, int_value, bigint_value, numeric_value, real_value, double_value, "
        "char_value, varchar_value, text_value, date_value, timestamp_value, bool_value "
        "FROM sample_table WHERE id < $1";
    PG_LOG_INFO("SQL=%s", sql);
    stmt = pg_prepare(ctx, "pickup data", sql);
    if (!stmt)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    const char *param[] = {"10"};
    res = pg_execute(stmt, 1, param);
    if (pg_rows(res) > 0 && pg_cols(res) > 0)
    {
        char line[4096] = "";
        for (int col = 0; col < pg_cols(res); col++)
        {
            char tmp[128];
            snprintf(tmp, sizeof(tmp), "%s\t", pg_col_name(res, col));
            strcat(line, tmp);
        }
        PG_LOG_INFO(line);

        memset(line, 0, sizeof(line));
        for (int col = 0; col < pg_cols(res); col++)
        {
            char tmp[128];
            snprintf(tmp, sizeof(tmp), "%s\t", pg_col_type_name(res, col));
            strcat(line, tmp);
        }
        PG_LOG_INFO(line);
        PG_LOG_INFO("================================================================");

        for (int row = 0; row < pg_rows(res); row++)
        {
            char line[4096] = "";
            for (int col = 0; col < pg_cols(res); col++)
            {
                char tmp[128];
                snprintf(tmp, sizeof(tmp), "%s\t", pg_value(res, row, col));
                strcat(line, tmp);
            }
            PG_LOG_INFO(line);
        }
    }

cleanup:
    pg_stmt_free(stmt);
    pg_result_free(res);
    pg_disconnect(ctx);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}