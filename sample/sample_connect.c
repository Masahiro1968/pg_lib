/**
 * @file    sample_connect.c
 * @brief   接続方法に関するサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLへの接続と切断に関するサンプルコードとなります。
 *   - 接続後、テーブルアクセスを伴わない簡単なSQLを実行し、結果を取得します。
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_connect.log";
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
        goto cleanup1;
    }

    sql = "SELECT 1, 'way', 3.14, current_database(), current_user, clock_timestamp()";
    PG_LOG_INFO("SQL=%s", sql);
    res = pg_query(ctx, sql);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }
    else
    {
        PG_LOG_INFO("rows=%d", pg_rows(res));
        PG_LOG_INFO("cols=%d", pg_cols(res));
        char line[4096] = "";
        for (int col = 0; col < pg_cols(res); col++)
        {
            char tmp[128];
            snprintf(tmp, sizeof(tmp),
                     "%s [%d](%s)\t",
                     pg_col_name(res, col),
                     pg_col_type(res, col),
                     pg_col_type_name(res, col));
            strcat(line, tmp);
        }
        PG_LOG_INFO(line);
        PG_LOG_INFO("%s,%s,%s,%s,%s,%s",
                    pg_value(res, 0, 0), pg_value(res, 0, 1), pg_value(res, 0, 2),
                    pg_value(res, 0, 3), pg_value(res, 0, 4), pg_value(res, 0, 5));
    }

cleanup2:
    pg_result_free(res);
cleanup1:
    pg_disconnect(ctx);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}