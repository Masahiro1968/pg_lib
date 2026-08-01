/**
 * @file    pg_result.c
 * @brief   PostgreSQL 取得結果に関する関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLからのデータを取得する関数をここに集約します。
 */

#include "pg_lib.h"

int pg_rows(PGresult *res)
{
    if (!res)
        return -1;

    return PQntuples(res);
}

int pg_cols(PGresult *res)
{
    if (!res)
        return -1;

    return PQnfields(res);
}

const char *pg_col_name(PGresult *res, int col)
{
    if (!res)
        return NULL;

    return PQfname(res, col);
}

const char *pg_value(PGresult *res, int row, int col)
{
    if (!res)
        return NULL;

    return PQgetvalue(res, row, col);
}

void pg_result_free(PGresult *res)
{
    if (res)
        PQclear(res);
}

int pg_is_null(PGresult *res, int row, int col)
{
    return PQgetisnull(res, row, col);
}