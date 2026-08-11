/**
 * @file    sample_thread_read.c
 * @brief   マルチスレッド動作によるサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - OpenMPを使用してテーブル一覧を取得後、マルチスレッドで各テーブルのデータを出力します。
 */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "pg_lib.h"
#include "pg_utility.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 0

bool create_dummy_tables(int num_of_tables, int num_of_rows)
{
    PG_LOG_DEBUG("begin create_dummy_tables(%d,%d)", num_of_tables, num_of_rows);

    bool ret = false;
    PGContext *ctx = NULL;
    char *sql = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    char *template = NULL;
    const int buffer_size = 512;
    sql = malloc(buffer_size);
    for (int i = 1; i <= num_of_tables; i++)
    {
        PG_LOG_DEBUG("Making No.%d", i);

        template = "DROP TABLE IF EXISTS sample_table_%05d;";
        snprintf(sql, buffer_size, template, i);
        // PG_LOG_INFO("SQL=%s", sql);
        ret = pg_exec(ctx, sql);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            goto cleanup;
        }

        template =
            "CREATE TABLE sample_table_%05d ("
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
        snprintf(sql, buffer_size, template, i);
        // PG_LOG_INFO("SQL=%s", sql);
        ret = pg_exec(ctx, sql);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            goto cleanup;
        }

        template =
            "INSERT INTO sample_table_%05d ("
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
            ");";
        for (int j = 1; j <= num_of_rows; j++)
        {
            snprintf(
                sql,
                buffer_size,
                template,
                i,                         /* table_name_index */
                j,                         /* id */
                j * 10,                    /* int_value */
                (long long)j * 1000000LL,  /* bigint_value */
                j * 0.12345,               /* numeric_value */
                j * 1.25f,                 /* real_value */
                j * 123.456789,            /* double_value */
                j,                         /* char_value */
                j,                         /* varchar_value */
                j,                         /* text_value */
                (j % 28) + 1,              /* date_value */
                (j % 28) + 1,              /* timestamp day */
                j % 24,                    /* hour */
                j % 60,                    /* minute */
                (j * 3) % 60,              /* second */
                (j % 2) ? "TRUE" : "FALSE" /* bool_value */
            );
            // PG_LOG_INFO("SQL=%s", sql);

            ret = pg_exec(ctx, sql);
            if (!ret)
            {
                PG_LOG_ERROR(pg_error(ctx));
                goto cleanup;
            }
        }
    }

    free(sql);

    ret = true;

cleanup:
    pg_disconnect(ctx);

    PG_LOG_DEBUG("end create_dummy_tables()");
    return ret;
}

void dump_table(const char *table_name)
{
    int thread_no = omp_get_thread_num();
    PG_LOG_DEBUG("begin[%d] dump_table(%s)", thread_no, table_name);

    char file_name[256];
    snprintf(file_name, sizeof(file_name), "./%s.csv", table_name);
    FILE *fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        PG_LOG_ERROR("failed fopen(%s)", file_name);
        return;
    }

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;
    char *template = NULL;

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    template = "SELECT * FROM %s;";
    const int buffer_size = 512;
    sql = malloc(buffer_size);
    snprintf(sql, buffer_size, template, table_name);
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    res = pg_query(ctx, sql);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }
    else
    {
        for (int row = 0; row < pg_rows(res); row++)
        {
            char line[4096];
            int pos = 0;

            for (int col = 0; col < pg_cols(res); col++)
            {
                int n = snprintf(
                    line + pos,
                    sizeof(line) - pos,
                    col == 0 ? "'%s'" : ",'%s'",
                    pg_value(res, row, col));

                if (n < 0 || (size_t)n >= sizeof(line) - pos)
                {
                    PG_LOG_ERROR("CSV line is too long.");
                    break;
                }

                pos += n;
            }

            if (pos < sizeof(line) - 1)
                line[pos++] = '\n';

            line[pos] = '\0';

            // PG_LOG_INFO("%s", line);
            fputs(line, fp);
        }
    }

    free(sql);

cleanup2:
    pg_result_free(res);
cleanup1:
    pg_disconnect(ctx);

    fclose(fp);

    PG_LOG_DEBUG("end dump_table()");
}

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_thread_read.log";
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

    bool ret = create_dummy_tables(1000, 100);
    if (!ret)
    {
        return -1;
    }

    ctx = pg_connect("host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    res = pg_tables(ctx);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }
    else
    {
        int table_count = pg_rows(res);
        PG_LOG_DEBUG("table_count is %d", table_count);
        char **table_names = calloc(table_count, sizeof(char *));
        for (int i = 0; i < table_count; i++)
        {
            table_names[i] = strdup(pg_value(res, i, 0));
            PG_LOG_DEBUG("search table %d:%s", i, table_names[i]);
        }

        #pragma omp parallel
        {
            #pragma omp single
            {
                PG_LOG_INFO("OpenMP threads = %d", omp_get_num_threads());
            }
            
            #pragma omp for
            for (int i = 0; i < table_count; i++)
            {
                dump_table(table_names[i]);
            }
        }

        for (int i = 0; i < table_count; i++)
        {
            free(table_names[i]);
        }
        free(table_names);
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