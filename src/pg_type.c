/**
 * @file    pg_type.c
 * @brief   PostgreSQL タイプ情報
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLの型に関する定義や関数を集約します。
 */

#include <libpq-fe.h>
#include "pg_type.h"

static const char *get_type_name(Oid oid);

Oid pg_col_type(PGresult *res, int col)
{
    return PQftype(res, col);
}

const char *pg_col_type_name(PGresult *res, int col)
{
    return get_type_name(pg_col_type(res, col));
}

static const char *get_type_name(Oid oid)
{
    switch (oid)
    {
    case 16:
        return "bool";
    case 19:
        return "name";
    case 20:
        return "int8";
    case 21:
        return "int2";
    case 23:
        return "int4";
    case 25:
        return "text";
    case 700:
        return "float4";
    case 701:
        return "float8";
    case 1042:
        return "char";
    case 1043:
        return "varchar";
    case 1082:
        return "date";
    case 1114:
        return "timestamp";
    case 1184:
        return "timestamptz";
    case 1700:
        return "numeric";
    default:
        return "unknown";
    }
}
