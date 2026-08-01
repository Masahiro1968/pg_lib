/**
 * @file    pg_context.c
 * @brief   PostgreSQL 接続、切断関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLへの接続、切断を行う関数をここに集約します。
 */

#include <stdlib.h>
#include "pg_lib.h"
#include "pg_error.h"

PGContext *pg_connect(const char *conninfo)
{
    PGContext *ctx;

    ctx = malloc(sizeof(PGContext));
    if (!ctx)
        return NULL;

    pg_error_clear(ctx);
    ctx->conn = PQconnectdb(conninfo);
    if (PQstatus(ctx->conn) != CONNECTION_OK)
    {
        pg_error_set(ctx, NULL, NULL);
    }

    return ctx;
}

void pg_disconnect(PGContext *ctx)
{
    pg_error_clear(ctx);

    if (!ctx)
        return;

    if (ctx->conn)
        PQfinish(ctx->conn);

    free(ctx);

    ctx = NULL;
}
