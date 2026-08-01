/**
 * @file    sample_error_check.c
 * @brief   SQLエラーに対するエラー出力の確認用サンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_error.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"

#define USE_LOG_FILE 1

void connection_string_error1(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_error1()");

    PGContext *ctx = pg_connect(
        "host=localhost1 dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end connection_string_error1()");
}

void connection_string_error2(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_error2()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=sample user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end connection_string_error2()");
}

void connection_string_error3(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_error3()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres1 password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end connection_string_error3()");
}

void connection_string_error4(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_error4()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres password=postgres1");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end connection_string_error4()");
}

void sql_error_01(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_01()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    char *sql = "selectX 1";
    PG_LOG_INFO("SQL=%s", sql);
    PGresult *res = pg_query(ctx, sql);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_result_free(res);
    pg_disconnect(ctx);

    PG_LOG_DEBUG("end sql_error_01()");
}

void sql_error_02(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_02()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    char *sql = "insert int sample_table values(1,2,3,4,5)";
    PG_LOG_INFO("SQL=%s", sql);
    bool res = pg_exec(ctx, sql);
    if (!res)
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end sql_error_02()");
}

void sql_error_03(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_03()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    char *sql = "insert into test_table values(1,2,3,4,5)";
    PG_LOG_INFO("SQL=%s", sql);
    bool res = pg_exec(ctx, sql);
    if (!res)
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end sql_error_03()");
}

void sql_error_04(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_04()");

    PGContext *ctx = pg_connect(
        "host=localhost dbname=testdb user=postgres password=postgres");
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    char *sql = "update sample_table set number_data=1000";
    PG_LOG_INFO("SQL=%s", sql);
    bool res = pg_exec(ctx, sql);
    if (!res)
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end sql_error_04()");
}

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_error_check.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#else
    FILE *fp = stdout;
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    PG_LOG_DEBUG("start main()");

    pg_init();

    connection_string_error1(fp);
    connection_string_error2(fp);
    connection_string_error3(fp);
    connection_string_error4(fp);

    sql_error_01(fp);
    sql_error_02(fp);
    sql_error_03(fp);
    sql_error_04(fp);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}