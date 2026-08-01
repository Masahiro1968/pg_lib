/**
 * @file    pg_statement.c
 * @brief   PostgreSQL SQL実行系関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLへSQLを実行する関数をここに集約します。
 */

#include <stdlib.h>
#include <string.h>
#include "pg_statement.h"
#include "pg_error.h"

static PGresult *pg_execute_internal(PGContext *ctx, const char *sql, ExecStatusType expect);

bool pg_exec(PGContext *ctx, const char *sql)
{
    if (!ctx)
        return false;

    PGresult *res;
    res = pg_execute_internal(ctx, sql, PGRES_COMMAND_OK);
    if (!res)
        return false;

    PQclear(res);
    return true;
}

bool pg_begin(PGContext *ctx)
{
    return pg_exec(ctx, "BEGIN");
}

bool pg_commit(PGContext *ctx)
{
    return pg_exec(ctx, "COMMIT");
}

bool pg_rollback(PGContext *ctx)
{
    return pg_exec(ctx, "ROLLBACK");
}

PGresult *pg_query(PGContext *ctx, const char *sql)
{
    if (!ctx)
        return NULL;

    return pg_execute_internal(ctx, sql, PGRES_TUPLES_OK);
}

PGStmt *pg_prepare(PGContext *ctx, const char *name, const char *sql)
{
    if (!ctx)
        return NULL;

    PGStmt *stmt = malloc(sizeof(PGStmt));
    if (!stmt)
        return NULL;

    stmt->ctx = ctx;
    strncpy(stmt->name, name, sizeof(stmt->name) - 1);
    stmt->name[sizeof(stmt->name) - 1] = '\0';

    PGresult *res = PQprepare(ctx->conn, name, sql, 0, NULL);
    if (!pg_ok(res))
    {
        PQclear(res);
        free(stmt);
        return NULL;
    }

    PQclear(res);
    return stmt;
}

PGresult *pg_execute(PGStmt *stmt, int nparams, const char **params)
{
    return PQexecPrepared(stmt->ctx->conn, stmt->name, nparams, params, NULL, NULL, 0);
}

void pg_stmt_free(PGStmt *stmt)
{
    if (stmt)
        free(stmt);
}

bool pg_open_cursor(
    PGContext *ctx, const char *cursor_name, const char *sql,
    int nparams, const char **params)
{
    bool ret = true;
    char declare_sql[1024];

    snprintf(
        declare_sql, sizeof(declare_sql), "DECLARE %s CURSOR FOR %s", cursor_name, sql);

    PGresult *res = PQexecParams(
        ctx->conn, declare_sql, nparams, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_COMMAND_OK)
    {
        pg_error_set(ctx, sql, res);
        ret = false;
    }

    PQclear(res);
    return ret;
}

static PGresult *pg_execute_internal(PGContext *ctx, const char *sql, ExecStatusType expect)
{
    PGresult *res;
    ExecStatusType status;

    res = PQexec(ctx->conn, sql);
    if (!res)
    {
        pg_error_set(ctx, sql, NULL);
        return NULL;
    }

    status = PQresultStatus(res);
    if (status != expect)
    {
        pg_error_set(ctx, sql, res);
        PQclear(res);
        return NULL;
    }

    pg_error_clear(ctx);
    return res;
}