/**
 * @file    sample_thread_copy.c
 * @brief   マルチスレッド動作によるCOPYのサンプル
 * @author  Masahiro1968
 * @date    2026-09-03
 * @details
 *   - ファイル一覧を取得後、OpenMPを使用してマルチスレッドで各テーブルへデータを入力します。
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

bool copy_table(
    const PGString *connection_string,
    const char *schema_name,
    const PGString *file_name,
    const char *directory)
{
    int thread_no = omp_get_thread_num();
    PG_LOG_DEBUG("begin[%d] copy_table(%s)", thread_no, pg_string_get(file_name));

    bool response = false;
    PGString *file_path = pg_string_new(PATH_MAX);

    if (directory)
        pg_string_format(file_path, "%s/%s", directory, pg_string_get(file_name));
    else
        pg_string_format(file_path, "./%s", pg_string_get(file_name));

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

    int period = pg_string_reverse_find(file_name, ".");
    PGString *table_name = pg_string_left(file_name, period);

    template = "TRUNCATE TABLE %s";
    snprintf(sql, buffer_size, template, pg_string_get(table_name));
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }

#if RUN_WITH_COPY_COMMAND
    PGString *abstract_path = get_real_path(pg_string_get(file_path));
    // template = "COPY %s FROM '%s' WITH ( FORMAT csv, HEADER true, QUOTE '''' , NULL '');";
    template = "COPY %s FROM '%s' WITH ( FORMAT csv, HEADER true, QUOTE '\"');";
    snprintf(sql, buffer_size, template, pg_string_get(table_name), pg_string_get(abstract_path));
    pg_string_free(abstract_path);
    PG_LOG_INFO("SQL[%d]=%s", thread_no, sql);
    ret = pg_exec(ctx, sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        goto cleanup1;
    }
#else
    FILE *fp = fopen(pg_string_get(file_path), "r");
    if (!fp)
    {
        PG_LOG_ERROR("Failed to open file %s.", pg_string_get(file_path));
        goto cleanup1;
    }

    PGString *line;
    while (line = read_line(fp, 1024, false))
    {
        PGString *insert_sql = pg_string_new(pg_string_size(line) + 20);
        pg_string_format(insert_sql, "INSERT INTO %s VALUES(%s);", pg_string_get(table_name), pg_string_get(line));
        while (1)
        {
            int ret = pg_string_replace(insert_sql, "''", "NULL");
            if (ret < 0)
                break;
        }
        PG_LOG_INFO("SQL[%d]=%s", thread_no, pg_string_get(insert_sql));
        ret = pg_exec(ctx, pg_string_get(insert_sql));
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            pg_string_free(line);
            pg_string_free(insert_sql);
            goto cleanup1;
        }
        pg_string_free(line);
        pg_string_free(insert_sql);
    }

    fclose(fp);
#endif

    response = true;

cleanup1:
    pg_disconnect(ctx);
    free(sql);
    pg_string_free(table_name);
    pg_string_free(file_path);
    PG_LOG_DEBUG("end[%d] copy_table()", thread_no);

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
    printf("  -i or --input   <input directory> default is '.'\n");

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_thread_copy.log";
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
    char *input = ".";

    static struct option parameters[] = {
        {"host", required_argument, 0, 'H'},
        {"port", required_argument, 0, 'P'},
        {"dbname", required_argument, 0, 'D'},
        {"schema", required_argument, 0, 'S'},
        {"user", required_argument, 0, 'u'},
        {"pass", required_argument, 0, 'p'},
        {"service", required_argument, 0, 's'},
        {"input", required_argument, 0, 'i'},
        {0, 0, 0, 0} // EOL
    };

    int opt;
    int option_index;
    while ((opt = getopt_long(
                argc, argv, "H:P:D:S:u:p:s:i:", parameters, &option_index)) != -1)
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
        case 'i':
            input = optarg;
            break;
        default:
            PG_LOG_ERROR("Invalid argment %c.", opt);
            exit(EXIT_FAILURE);
        }
    }

    PG_LOG_DEBUG(
        "host=%s port=%s dbname=%s schema=%s user=%s password=%s service=%s input=%s",
        host, port, dbname, schema, user, pass, service, input);

    if (!is_exist_directory(input))
    {
        PG_LOG_ERROR("input '%s' is not exist.", input);
        return EXIT_FAILURE;
    }

    PGStringList *file_list = get_file_entry_list(input, true);
    if (!file_list)
    {
        PG_LOG_ERROR("Failed to get directory list %s.", input);
        return EXIT_FAILURE;
    }

    int num_of_files = pg_string_list_size(file_list);
    PG_LOG_DEBUG("num of files are %d", num_of_files);
    for (int i = 0; i < num_of_files; i++)
    {
        PGString *file_name = pg_string_list_get(file_list, i);
        PG_LOG_DEBUG("target file:%s", pg_string_get(file_name));
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
        for (int i = 0; i < num_of_files; i++)
        {
            PGString *file_name = pg_string_list_get(file_list, i);
            bool ret = copy_table(
                connection, // connection_string
                schema,     // schema_name
                file_name,  // file_name
                input);     // directory
            if (!ret)
            {
                PG_LOG_ERROR("failed copy_table(%s)", pg_string_get(file_name));
                #pragma omp cancel for
            }

            #pragma omp cancellation point for
        }
    }

    pg_string_list_free(file_list);

cleanup1:
    pg_string_free(connection);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}