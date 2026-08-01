/**
 * @file    pg_error.c
 * @brief   PostgreSQL エラー情報関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLで発生したエラー情報に関する定義や関数をここに集約します。
 */

#include <string.h>
#include "pg_error.h"

static const char *pg_status_name(ExecStatusType status);

const char *pg_error(PGContext *ctx)
{
    if (!ctx)
        return "invalid context";

    return ctx->last_error;
}

void pg_error_clear(PGContext *ctx)
{
    ctx->last_error[0] = '\0';
    ctx->last_sqlstate[0] = '\0';
    ctx->last_sql[0] = '\0';
    ctx->last_message[0] = '\0';
    ctx->last_detail[0] = '\0';
    ctx->last_hint[0] = '\0';
    ctx->last_status = PGRES_COMMAND_OK;
}

void pg_error_set(PGContext *ctx, const char *sql, PGresult *res)
{
    const char *msg;
    const char *sqlstate;
    const char *last_message;
    const char *last_detail;
    const char *last_hint;

    if (!ctx)
        return;

    if (sql)
    {
        strncpy(ctx->last_sql, sql, sizeof(ctx->last_sql) - 1);
        ctx->last_sql[sizeof(ctx->last_sql) - 1] = '\0';
    }
    else
    {
        ctx->last_sql[0] = '\0';
    }

    if (!res)
    {
        msg = PQerrorMessage(ctx->conn);
        strncpy(ctx->last_error, msg ? msg : "", sizeof(ctx->last_error) - 1);
        ctx->last_error[sizeof(ctx->last_error) - 1] = '\0';
        ctx->last_sqlstate[0] = '\0';
        ctx->last_status = PGRES_FATAL_ERROR;

        return;
    }

    ctx->last_status = PQresultStatus(res);
    msg = PQresultErrorMessage(res);

    strncpy(ctx->last_error, msg ? msg : "", sizeof(ctx->last_error) - 1);
    ctx->last_error[sizeof(ctx->last_error) - 1] = '\0';
    sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);
    if (sqlstate)
    {
        // ANSI SQL の仕様上、必ず５文字。
        strncpy(ctx->last_sqlstate, sqlstate, 5);
        ctx->last_sqlstate[5] = '\0';
    }
    else
    {
        ctx->last_sqlstate[0] = '\0';
    }

    last_message = PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY);
    if (last_message)
    {
        strncpy(ctx->last_message, last_message, sizeof(ctx->last_message) - 1);
        ctx->last_message[sizeof(ctx->last_message) - 1] = '\0';
    }
    else
    {
        ctx->last_message[0] = '\0';
    }

    last_detail = PQresultErrorField(res, PG_DIAG_MESSAGE_DETAIL);
    if (last_detail)
    {
        strncpy(ctx->last_detail, last_detail, sizeof(ctx->last_detail) - 1);
        ctx->last_detail[sizeof(ctx->last_detail) - 1] = '\0';
    }
    else
    {
        ctx->last_detail[0] = '\0';
    }

    last_hint = PQresultErrorField(res, PG_DIAG_MESSAGE_HINT);
    if (last_hint)
    {
        strncpy(ctx->last_hint, last_hint, sizeof(ctx->last_hint) - 1);
        ctx->last_hint[sizeof(ctx->last_hint) - 1] = '\0';
    }
    else
    {
        ctx->last_hint[0] = '\0';
    }
}

void pg_error_dump(FILE *fp, PGContext *ctx)
{
    fprintf(fp, "========================================\n");
    fprintf(fp, "SQLSTATE : %s\n", ctx->last_sqlstate);
    fprintf(fp, "STATUS   : %s\n", pg_status_name(ctx->last_status));
    fprintf(fp, "SQL      : %s\n", ctx->last_sql);
    fprintf(fp, "MESSAGE  : %s\n", ctx->last_error);
    fprintf(fp, "DETAIL   : %s\n", ctx->last_detail);
    fprintf(fp, "HINT     : %s\n", ctx->last_hint);
    fprintf(fp, "========================================\n");
}

static const char *pg_status_name(ExecStatusType status)
{
    switch (status)
    {
    case PGRES_EMPTY_QUERY:
        return "PGRES_EMPTY_QUERY";

    case PGRES_COMMAND_OK:
        return "PGRES_COMMAND_OK";

    case PGRES_TUPLES_OK:
        return "PGRES_TUPLES_OK";

    case PGRES_COPY_OUT:
        return "PGRES_COPY_OUT";

    case PGRES_COPY_IN:
        return "PGRES_COPY_IN";

    case PGRES_BAD_RESPONSE:
        return "PGRES_BAD_RESPONSE";

    case PGRES_NONFATAL_ERROR:
        return "PGRES_NONFATAL_ERROR";

    case PGRES_FATAL_ERROR:
        return "PGRES_FATAL_ERROR";

    case PGRES_COPY_BOTH:
        return "PGRES_COPY_BOTH";

    case PGRES_SINGLE_TUPLE:
        return "PGRES_SINGLE_TUPLE";

    case PGRES_PIPELINE_SYNC:
        return "PGRES_PIPELINE_SYNC";

    case PGRES_PIPELINE_ABORTED:
        return "PGRES_PIPELINE_ABORTED";

    case PGRES_TUPLES_CHUNK:
        return "PGRES_TUPLES_CHUNK";
    }

    return "UNKNOWN";
}
