/**
 * @file    sample_thread_read.c
 * @brief   マルチスレッド動作によるCSV出力のサンプル
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

#define USE_LOG_FILE 1

bool dump_table_fetch(
    const PGString *connection_string,
    const char *schema_name,
    const PGString *table_name,
    const char *delimiter,
    const char *bracket,
    bool null_bracket,
    const char *eol,
    bool need_field_name,
    const char *output)
{
    int thread_no = omp_get_thread_num();
    PG_LOG_DEBUG("begin[%d] dump_table_fetch(%s)", thread_no, pg_string_get(table_name));

    bool response = false;
    PGString *file_name = pg_string_new(PATH_MAX);

    if (output)
        pg_string_format(file_name, "%s/%s.csv", output, pg_string_get(table_name));
    else
        pg_string_format(file_name, "./%s.csv", pg_string_get(table_name));

    FILE *fp = fopen(pg_string_get(file_name), "w");
    if (fp == NULL)
    {
        PG_LOG_ERROR("failed fopen(%s)", pg_string_get(file_name));
        pg_string_free(file_name);
        return response;
    }

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;
    char *template = NULL;
    bool ret;

    ctx = pg_connect(pg_string_get(connection_string));
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    const int buffer_size = 512;
    sql = malloc(buffer_size);

    snprintf(sql, buffer_size, "SET search_path TO %s;", schema_name);
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

    template = "SELECT * FROM %s;";
    snprintf(sql, buffer_size, template, pg_string_get(table_name));
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    ret = pg_open_cursor(ctx, "dump_cursor", sql, 0, NULL);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }
    else
    {
        bool field_name_out = false;
        for (;;)
        {
            res = pg_read_cursor(ctx, "dump_cursor", 100);
            if (!pg_ok(res))
            {
                PG_LOG_ERROR(pg_error(ctx));
                pg_close_cursor(ctx, "dump_cursor");
                goto cleanup2;
            }
            if (pg_rows(res) == 0)
            {
                PG_LOG_DEBUG("dump finished. %s", pg_string_get(table_name));
                pg_close_cursor(ctx, "dump_cursor");
                break;
            }

            if (need_field_name && !field_name_out)
            {
                PGStringList *field_name_list = pg_get_field_names(res);
                PGString *field_names = pg_make_data(field_name_list, delimiter, bracket, eol, null_bracket);
                fputs(pg_string_get(field_names), fp);
                pg_string_free(field_names);
                pg_string_list_free(field_name_list);
                field_name_out = true;
            }

            for (int row = 0; row < pg_rows(res); row++)
            {
                PGStringList *field_data_list = pg_get_row(res, row);
                PGString *field_data = pg_make_data(field_data_list, delimiter, bracket, eol, null_bracket);
                fputs(pg_string_get(field_data), fp);
                pg_string_free(field_data);
                pg_string_list_free(field_data_list);
            }

            pg_result_free(res);
        }
    }

    response = true;

cleanup2:
    pg_result_free(res);

cleanup1:
    pg_disconnect(ctx);
    free(sql);
    fclose(fp);
    pg_string_free(file_name);
    PG_LOG_DEBUG("end dump_table_fetch()");

    return response;
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

#ifdef DEBUG
    pg_log_set_level(PG_LEVEL_DEBUG);
#else
    pg_log_set_level(PG_LEVEL_INFO);
#endif

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
                argc, argv, "H:P:D:S:u:p:s:o:", parameters, &option_index)) != -1)
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
        return EXIT_FAILURE;
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
        PGStringList *table_names = pg_string_list_new();
        for (int i = 0; i < table_count; i++)
        {
            PGString *table_name = pg_string_new(30);
            pg_string_set(table_name, pg_value(res, i, 0));
            PG_LOG_DEBUG("search table %d:%s", i, pg_string_get(table_name));
            pg_string_list_add(table_names, table_name);
        }

//
// You need to set environment variable "OMP_CANCELLATION=true"
// Then you can stop the error thread immediately.
//
        #pragma omp parallel
        {
            #pragma omp single
            {
                PG_LOG_INFO("OpenMP threads = %d", omp_get_num_threads());
            }

            #pragma omp for schedule(dynamic, 1)
            for (int i = 0; i < table_count; i++)
            {
                bool ret = dump_table_fetch(
                    connection,                         // connection_string
                    schema,                             // schema_name
                    pg_string_list_get(table_names, i), // table_name
                    ",",                                // delimiter
                    "'",                                // bracket
                    true,                               // null_bracket
                    "\n",                               // eol
                    false,                              // need_field_name
                    output);                            // output directory
                if (!ret)
                {
                    PG_LOG_ERROR("failed dump_table_fetch()");
                    #pragma omp cancel for
                }

                #pragma omp cancellation point for
            }
        }

        pg_string_list_free(table_names);
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