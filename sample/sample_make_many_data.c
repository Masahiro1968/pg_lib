/**
 * @file    sample_make_many_data.c
 * @brief   扱う型を定義したテーブルを作成し、データを格納するサンプル２
 * @author  Masahiro1968
 * @date    2026-08-22
 * @details
 *   - データベース、スキーマを指定してサンプルデータを作成します。
 */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <omp.h>
#include "pg_lib.h"
#include "pg_statement.h"
#include "pg_utility.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"
#include "pg_string.h"

#define USE_LOG_FILE 1

const char *SQL_DROP_SCHEMA =
    "DROP SCHEMA IF EXISTS %s CASCADE;";
const char *SQL_CREATE_SCHEMA =
    "CREATE SCHEMA %s;";
const char *SQL_CREATE_TABLE =
    "CREATE TABLE sample_table_%05d ("
    "   id              INTEGER PRIMARY KEY,"
    "   int_value       INTEGER,"
    "   bigint_value    BIGINT,"
    "   numeric_value   NUMERIC(9,5),"
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

bool create_dummy_data(
    PGString *connection_string,
    const char *schema_name,
    const int table_number,
    const int num_of_records)
{
    int thread_no = omp_get_thread_num();
    PG_LOG_DEBUG("begin[%d] create_dummy_data(%d)", thread_no, table_number);

    bool ret = false;
    PGContext *ctx = NULL;
    PGStmt *stmt = NULL;
    PGresult *res = NULL;
    char *sql = NULL;

    ctx = pg_connect(pg_string_get(connection_string));
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup_t_1;
    }

    const int buffer_size = 512;
    sql = malloc(buffer_size);

    snprintf(sql, buffer_size, "SET search_path TO %s;", schema_name);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup_t_1;
    }

    snprintf(sql, buffer_size, SQL_CREATE_TABLE, table_number);
    PG_LOG_INFO("SQL=%s", sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup_t_1;
    }

    snprintf(sql, buffer_size, SQL_INSERT, table_number);
    PG_LOG_INFO("SQL=%s", sql);
    stmt = pg_prepare(ctx, "insert data", sql);
    if (!stmt)
    {
        PG_LOG_ERROR(pg_error(ctx));
        ret = false;
        goto cleanup_t_2;
    }

    for (int j = 1; j <= num_of_records; j++)
    {
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
            goto cleanup_t_3;
        }

        pg_result_free(res);
    }

    pg_stmt_free(stmt);

    ret = true;
    goto cleanup_t_1;

cleanup_t_3:
    pg_result_free(res);

cleanup_t_2:
    pg_stmt_free(stmt);

cleanup_t_1:
    pg_disconnect(ctx);

    free(sql);

    return ret;
}

void print_usage(char *argv0)
{
    printf("Usage:\n");
    printf("%s <parameters>\n", argv0);
    printf(" parameters are ...\n");
    printf("  -H or --host    <database server name>\n");
    printf("  -P or --port    <port number>\n");
    printf("  -D or --dbname  <database name>\n");
    printf("  -S or --schema  <schema name>\n");
    printf("  -u or --user    <login user name>\n");
    printf("  -p or --pass    <login password>\n");
    printf("  -s or --service <service name>\n");
    printf("  -t or --tables  <create table count>\n");
    printf("  -r or --records <create record count>\n");

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_make_many_data.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    if (argc == 1)
        print_usage(argv[0]);

#ifdef DEBUG
    pg_log_set_level(PG_LEVEL_DEBUG);
#else
    pg_log_set_level(PG_LEVEL_INFO);
#endif

    char *host = NULL;
    char *port = NULL;
    char *dbname = NULL;
    char *schema = "public";
    char *user = NULL;
    char *pass = NULL;
    char *service = NULL;
    char *tables = NULL;
    char *records = NULL;
    int num_of_tables = -1;
    int num_of_records = -1;

    static struct option parameters[] = {
        {"host", required_argument, 0, 'H'},
        {"port", required_argument, 0, 'P'},
        {"dbname", required_argument, 0, 'D'},
        {"schema", required_argument, 0, 'S'},
        {"user", required_argument, 0, 'u'},
        {"pass", required_argument, 0, 'p'},
        {"service", required_argument, 0, 's'},
        {"tables", required_argument, 0, 't'},
        {"records", required_argument, 0, 'r'},
        {0, 0, 0, 0} // EOL
    };

    int opt;
    int option_index;
    while ((opt = getopt_long(
                argc, argv, "H:P:D:S:u:p:s:t:r:", parameters, &option_index)) != -1)
    {
        switch (opt)
        {
        case 'H':
            host = optarg;
            break;
        case 'P':
            port = optarg;
            break;
        case 'D':
            dbname = optarg;
            break;
        case 'S':
            schema = optarg;
            break;
        case 'u':
            user = optarg;
            break;
        case 'p':
            pass = optarg;
            break;
        case 's':
            service = optarg;
            break;
        case 't':
            tables = optarg;
            break;
        case 'r':
            records = optarg;
            break;
        default:
            PG_LOG_ERROR("Invalid argment %c.", opt);
            exit(EXIT_FAILURE);
        }
    }

    PG_LOG_INFO(
        "host=%s port=%s dbname=%s schema=%s user=%s password=%s service=%s tables=%s records=%s",
        host, port, dbname, schema, user, pass, service, tables, records);

    if (tables)
    {
        num_of_tables = atoi(tables);
        if (num_of_tables <= 0)
        {
            PG_LOG_ERROR("parameter 'tables' is invalid(%s).", tables);
            exit(EXIT_FAILURE);
        }
    }

    if (records)
    {
        num_of_records = atoi(records);
        if (num_of_records <= 0)
        {
            PG_LOG_ERROR("parameter 'records' is invalid(%s).", records);
            exit(EXIT_FAILURE);
        }
    }

    PGString *connection_string = pg_string_new(128);
    if (!pg_build_connection_string(connection_string, host, port, dbname, user, pass, service))
    {
        PG_LOG_ERROR("cannot build connection_string.");
        pg_string_free(connection_string);
        return EXIT_FAILURE;
    }
    else
        PG_LOG_INFO("connection_string:%s", pg_string_get(connection_string));

    pg_init();

    bool ret = false;
    PGContext *ctx = NULL;
    char *sql = NULL;

    ctx = pg_connect(pg_string_get(connection_string));
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    const int buffer_size = 512;
    sql = malloc(buffer_size);

    snprintf(sql, buffer_size, SQL_DROP_SCHEMA, schema);
    PG_LOG_INFO("SQL=%s", sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    snprintf(sql, buffer_size, SQL_CREATE_SCHEMA, schema);
    PG_LOG_INFO("SQL=%s", sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup;
    }

    #pragma omp parallel
    {
        #pragma omp single
        {
            PG_LOG_INFO("OpenMP threads = %d", omp_get_num_threads());
        }

        #pragma omp for
        for (int i = 1; i <= num_of_tables; i++)
        {
            ret = create_dummy_data(connection_string, schema, i, num_of_records);
        }
    }

    ret = true;

cleanup:
    pg_disconnect(ctx);

    free(sql);
    pg_string_free(connection_string);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return ret;
}