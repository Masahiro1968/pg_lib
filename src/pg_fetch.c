/**
 * @file    pg_fetch.c
 * @brief   PostgreSQL FETCH関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PGresultから型定義した変数にデータを格納します。
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "pg_lib.h"
#include "pg_type.h"

int pg_fetch_row(PGresult *res, int row, PGField *fields, int count)
{
    for (int i = 0; i < count; i++)
    {
        char *value;

        if (PQgetisnull(res, row, i))
            continue;

        value = PQgetvalue(res, row, i);

        switch (fields[i].type)
        {
        case PG_TYPE_INT:
            *(int *)fields[i].addr = atoi(value);
            break;

        case PG_TYPE_INT64:
            *(int64_t *)fields[i].addr = atoll(value);
            break;

        case PG_TYPE_FLOAT:
            *(float *)fields[i].addr = (float)atof(value);
            break;

        case PG_TYPE_DOUBLE:
            *(double *)fields[i].addr = atof(value);
            break;

        case PG_TYPE_BOOL:
            *(bool *)fields[i].addr =
                (value[0] == 't' || value[0] == 'T' || value[0] == '1');
            break;

        case PG_TYPE_CHAR:
        case PG_TYPE_TEXT:
        case PG_TYPE_DATE:
        case PG_TYPE_TIME:
        case PG_TYPE_TIMESTAMP:
        case PG_TYPE_NUMERIC:
            snprintf((char *)fields[i].addr, fields[i].size, "%s", value);
            break;

        default:
            break;
        }
    }

    return 1;
}