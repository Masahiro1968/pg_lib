/**
 * @file    pg_utility.c
 * @brief   PostgreSQL ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLに関連するユーティリティを集約します。
 */

#include <string.h>
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

PGStringList *pg_get_row(PGresult *res, int row)
{
    PGStringList *list = pg_string_list_new();

    for (int col = 0; col < pg_cols(res); col++)
    {
        const char *value = pg_value(res, row, col);
        PGString *str = pg_string_new(strlen(value));
        if (!str)
        {
            pg_string_list_free(list);
            return NULL;
        }

        pg_string_set(str, value);

        if (!pg_string_list_add(list, str))
        {
            pg_string_free(str);
            pg_string_list_free(list);
            return NULL;
        }
    }

    return list;
}

PGStringList *pg_get_field_names(PGresult *res)
{
    PGStringList *list = pg_string_list_new();

    for (int col = 0; col < pg_cols(res); col++)
    {
        const char *value = pg_col_name(res, col);
        PGString *str = pg_string_new(strlen(value));
        if (!str)
        {
            pg_string_list_free(list);
            return NULL;
        }

        pg_string_set(str, value);

        if (!pg_string_list_add(list, str))
        {
            pg_string_free(str);
            pg_string_list_free(list);
            return NULL;
        }
    }

    return list;
}

PGString *pg_make_data(PGStringList *row_data, const char *delimiter, const char *blacket, const char *EOL)
{
    const int buffer_size = 1024;
    PGString *response = pg_string_new(buffer_size);

    for (int col = 0; col < pg_string_list_size(row_data); col++)
    {
        PGString *value = pg_string_list_get(row_data, col);
        const char *string = pg_string_get(value);
        PGString *temp = pg_string_new(buffer_size);
        if (col == 0)
        {
            if (blacket == NULL)
            {
                pg_string_format(temp, "%s", string);
            }
            else
            {
                pg_string_format(temp, "%s%s%s", blacket, string, blacket);
            }
        }
        else
        {
            if (blacket == NULL)
            {
                pg_string_format(temp, "%s%s", delimiter, string);
            }
            else
            {
                pg_string_format(temp, "%s%s%s%s", delimiter, blacket, string, blacket);
            }
        }
        pg_string_join(response, temp);
        pg_string_free(temp);
    }

    PGString *eol = pg_string_new(buffer_size);
    pg_string_set(eol, EOL);
    pg_string_join(response, eol);
    pg_string_free(eol);

    return response;
}