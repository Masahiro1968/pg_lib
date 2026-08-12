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
#include "pg_statement.h"
#include "pg_utility.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"
#include "pg_string.h"

#define USE_LOG_FILE 1

const char *SQL_DROP_TABLE =
    "DROP TABLE IF EXISTS sample_table_%05d;";
const char *SQL_CREATE_TABLE =
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
const char *SQL_INSERT =
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
    "$1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12);";

bool create_dummy_tables(int num_of_tables, int num_of_rows)
{
    PG_LOG_DEBUG("begin create_dummy_tables(%d,%d)", num_of_tables, num_of_rows);

    bool ret = false;
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

    const int buffer_size = 512;
    sql = malloc(buffer_size);
    for (int i = 1; i <= num_of_tables; i++)
    {
        PG_LOG_DEBUG("Making No.%d", i);

        snprintf(sql, buffer_size, SQL_DROP_TABLE, i);
        PG_LOG_INFO("SQL=%s", sql);
        ret = pg_exec(ctx, sql);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            goto cleanup;
        }

        snprintf(sql, buffer_size, SQL_CREATE_TABLE, i);
        PG_LOG_INFO("SQL=%s", sql);
        ret = pg_exec(ctx, sql);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            goto cleanup;
        }

        snprintf(sql, buffer_size, SQL_INSERT, i);
        PG_LOG_INFO("SQL=%s", sql);
        stmt = pg_prepare(ctx, "insert data", sql);
        if (!stmt)
        {
            PG_LOG_ERROR(pg_error(ctx));
            ret = false;
            goto cleanup2;
        }

        for (int j = 1; j <= num_of_rows; j++)
        {
            PG_LOG_DEBUG("Making Row %d.", j);

            char id_value[32];
            char int_value[32];
            char bigint_value[32];
            char numeric_value[32];
            char real_value[32];
            char double_value[32];
            char char_value[32];
            char varchar_value[32];
            char text_value[32];
            char date_value[32];
            char timestamp_value[32];
            char bool_value[8];
            const char *param[12];

            snprintf(id_value, sizeof(id_value), "%d", j);
            snprintf(int_value, sizeof(int_value), "%d", j);
            snprintf(bigint_value, sizeof(bigint_value), "%lld", (long long)j * 1000000LL);
            snprintf(numeric_value, sizeof(numeric_value), "%.5f", j * 0.12345);
            snprintf(real_value, sizeof(real_value), "%.2f", j * 1.25f);
            snprintf(double_value, sizeof(double_value), "%.10f", j * 123.456789);
            snprintf(char_value, sizeof(char_value), "C%04d", j);
            snprintf(varchar_value, sizeof(varchar_value), "VARCHAR-%04d", j);
            snprintf(text_value, sizeof(text_value), "TEXT-%04d", j);
            snprintf(date_value, sizeof(date_value), "2026-07-%02d", (j % 28) + 1);
            snprintf(timestamp_value, sizeof(timestamp_value), "2026-07-%02d %02d:%02d:%02d",
                     (j % 28) + 1, j % 24, j % 60, (j * 3) % 60);
            snprintf(bool_value, sizeof(bool_value), "%s", (j % 2) ? "TRUE" : "FALSE");

            param[0] = id_value;

            if (j % 7 != 0)
            {
                param[1] = int_value;
                param[2] = bigint_value;
                param[3] = numeric_value;
                param[4] = real_value;
                param[5] = double_value;
            }
            else
            {
                param[1] = NULL;
                param[2] = NULL;
                param[3] = NULL;
                param[4] = NULL;
                param[5] = NULL;
            }

            if (j % 8 != 0)
            {
                param[6] = char_value;
                param[7] = varchar_value;
                param[8] = text_value;
            }
            else
            {
                param[6] = NULL;
                param[7] = NULL;
                param[8] = NULL;
            }

            if (j % 9 != 0)
            {
                param[9] = date_value;
                param[10] = timestamp_value;
                param[11] = bool_value;
            }
            else
            {
                param[9] = NULL;
                param[10] = NULL;
                param[11] = NULL;
            }

            res = pg_execute(stmt, 12, param);
            if (!pg_ok(res))
            {
                PG_LOG_ERROR(pg_error(ctx));
                ret = false;
                goto cleanup3;
            }
            pg_result_free(res);
        }

        pg_stmt_free(stmt);
    }

    ret = true;
    goto cleanup;

cleanup3:
    pg_result_free(res);

cleanup2:
    pg_stmt_free(stmt);

cleanup:
    pg_disconnect(ctx);

    free(sql);

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
        const char *delimiter = "\t";
        const char *blacket = "'";
        const char *eol = "\n";
        PGStringList *field_name_list = pg_get_field_names(res);
        PGString *field_names = pg_make_data(field_name_list, delimiter, blacket, eol);
        fputs(pg_string_get(field_names), fp);
        pg_string_free(field_names);
        pg_string_list_free(field_name_list);

        for (int row = 0; row < pg_rows(res); row++)
        {
            PGStringList *field_data_list = pg_get_row(res, row);
            PGString *field_data = pg_make_data(field_data_list, delimiter, blacket, eol);
            fputs(pg_string_get(field_data), fp);
            pg_string_free(field_data);
            pg_string_list_free(field_data_list);
        }
    }

cleanup2:
    pg_result_free(res);

cleanup1:
    pg_disconnect(ctx);

    free(sql);

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

#ifdef MAKE_DATA
    bool ret = create_dummy_tables(2000, 1000);
    if (!ret)
    {
        return -1;
    }
#endif

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

            // #pragma omp for
            #pragma omp for schedule(dynamic, 1)
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