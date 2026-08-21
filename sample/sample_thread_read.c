/**
 * @file    sample_thread_read.c
 * @brief   マルチスレッド動作によるサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - OpenMPを使用してテーブル一覧を取得後、マルチスレッドで各テーブルのデータを出力します。
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

#define USE_LOG_FILE 0

void dump_table(
    const char *connection_string,
    const char *schema_name,
    const char *table_name,
    const char *output)
{
    int thread_no = omp_get_thread_num();
    PG_LOG_DEBUG("begin[%d] dump_table(%s)", thread_no, table_name);

    bool ret;
    char file_name[PATH_MAX];

    if (output)
        snprintf(file_name, sizeof(file_name), "%s/%s.csv", output, table_name);
    else
        snprintf(file_name, sizeof(file_name), "./%s.csv", table_name);

    FILE *fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        PG_LOG_ERROR("failed fopen(%s)", file_name);
        return;
    }

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;
    char *template = NULL;

    ctx = pg_connect(connection_string);
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    const int buffer_size = 512;
    sql = malloc(buffer_size);

    snprintf(sql, buffer_size, "SET search_path TO %s;", schema_name);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    template = "SELECT * FROM %s;";
    snprintf(sql, buffer_size, template, table_name);
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    res = pg_query(ctx, sql);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }
    else
    {
        const char *delimiter = ",";
        const char *blacket = "\"";
        const char *eol = "\n";
        PGStringList *field_name_list = pg_get_field_names(res);
        PGString *field_names = pg_make_data(field_name_list, delimiter, blacket, eol);
        fputs(pg_string_get(field_names), fp);
        pg_string_free(field_names);
        pg_string_list_free(field_name_list);

        for (int row = 0; row < pg_rows(res); row++)
        {
            PGStringList *field_data_list = pg_get_row(res, row);
            PGString *field_data = pg_make_data(field_data_list, delimiter, blacket, eol);
            fputs(pg_string_get(field_data), fp);
            pg_string_free(field_data);
            pg_string_list_free(field_data_list);
        }
    }

cleanup2:
    pg_result_free(res);

cleanup1:
    pg_disconnect(ctx);

    free(sql);

    fclose(fp);

    PG_LOG_DEBUG("end dump_table()");
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
    printf("  -s or --service <service key>\n");
    printf("  -o or --output  <output directory> default is '.'\n");

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_thread_read.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

    pg_log_set_level(PG_LEVEL_DEBUG);

    if (argc == 1)
        print_usage(argv[0]);

    PG_LOG_DEBUG("start main()");

    char *host = NULL;
    char *port = NULL;
    char *dbname = NULL;
    char *schema = "public";
    char *user = NULL;
    char *pass = NULL;
    char *service = NULL;
    char *output = ".";

    static struct option parameters[] = {
        {"host", required_argument, 0, 'H'},
        {"port", required_argument, 0, 'P'},
        {"dbname", required_argument, 0, 'D'},
        {"schema", required_argument, 0, 'S'},
        {"user", required_argument, 0, 'u'},
        {"pass", required_argument, 0, 'p'},
        {"service", required_argument, 0, 's'},
        {"output", required_argument, 0, 'o'},
        {0, 0, 0, 0} // EOL
    };

    int opt;
    int option_index;
    while ((opt = getopt_long(
                argc, argv, "H:P:D:S:u:p:s:o", parameters, &option_index)) != -1)
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
        case 'o':
            output = optarg;
            break;
        default:
            PG_LOG_ERROR("Invalid argment %c.", opt);
            exit(EXIT_FAILURE);
        }
    }

    PG_LOG_DEBUG(
        "host=%s port=%s dbname=%s schema=%s user=%s password=%s service=%s output=%s",
        host, port, dbname, schema, user, pass, service, output);

    if (!is_exist_directory(output))
    {
        PG_LOG_ERROR("output '%s' is not exist.", output);
        exit(EXIT_FAILURE);
    }

    PGString *connection = pg_string_new(PATH_MAX);
    if (!pg_build_connection_string(connection, host, port, dbname, user, pass, service))
    {
        PG_LOG_ERROR("cannot build connection_string.");
        pg_string_free(connection);
        return EXIT_FAILURE;
    }
    else
        PG_LOG_INFO("connection_string:%s", pg_string_get(connection));

    pg_init();

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;

    ctx = pg_connect(pg_string_get(connection));
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    res = pg_tables(ctx, schema);
    if (!pg_ok(res))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup2;
    }
    else
    {
        int table_count = pg_rows(res);
        PG_LOG_DEBUG("table_count is %d", table_count);
        char **table_names = calloc(table_count, sizeof(char *));
        for (int i = 0; i < table_count; i++)
        {
            table_names[i] = strdup(pg_value(res, i, 0));
            PG_LOG_DEBUG("search table %d:%s", i, table_names[i]);
        }

#pragma omp parallel
        {
#pragma omp single
            {
                PG_LOG_INFO("OpenMP threads = %d", omp_get_num_threads());
            }

#pragma omp for schedule(dynamic, 1)
            for (int i = 0; i < table_count; i++)
            {
                dump_table(pg_string_get(
                               connection),
                           schema, table_names[i], output);
            }
        }

        for (int i = 0; i < table_count; i++)
        {
            free(table_names[i]);
        }
        free(table_names);
    }

cleanup2:
    pg_result_free(res);

cleanup1:
    pg_disconnect(ctx);

    pg_string_free(connection);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}