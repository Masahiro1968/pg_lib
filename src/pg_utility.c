/**
 * @file    pg_utility.c
 * @brief   PostgreSQL ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLに関連するユーティリティを集約します。
 */

#include <stdlib.h>
#include "pg_utility.h"

int pg_datetime_parse(const char *src, PGDateTime *dst)
{
    if (!src || !dst)
        return 0;

    sscanf(src, "%d-%d-%d %d:%d:%d",
           &dst->year,
           &dst->month,
           &dst->day,
           &dst->hour,
           &dst->minute,
           &dst->second);

    return 1;
}

int pg_ok(PGresult *res)
{
    if (!res)
        return PGRES_EMPTY_QUERY;

    ExecStatusType st;
    st = PQresultStatus(res);

    return st == PGRES_TUPLES_OK || st == PGRES_COMMAND_OK;
}

int pg_connected(PGContext *ctx)
{
    if (ctx == NULL)
        return 0;

    if (ctx->conn == NULL)
        return 0;

    return PQstatus(ctx->conn) == CONNECTION_OK;
}

PGresult *pg_tables(PGContext *ctx)
{
    return PQexec(
        ctx->conn, "select tablename from pg_tables where schemaname='public'");
}