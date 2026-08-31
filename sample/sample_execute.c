/**
 * @file    sample_execute.c
 * @brief   ファイルからのSQL供給とその連続時実行に関するサンプル
 * @author  Masahiro1968
 * @date    2026-08-22
 * @details
 *   - ファイルから供給されるSQLを順次実行していきます。
 *   - データベースへの接続と切断は＠から始まるコマンドとして処理します。
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"
#include "pg_string.h"

#define USE_LOG_FILE 1

PGStringList *parse_sql_string(const char *file_name)
{
    PG_LOG_DEBUG("parse_sql_string(%s) begin", file_name);

    FILE *fp = fopen(file_name, "r");
    if (!fp)
    {
        PG_LOG_ERROR("cannot open file %s. %s", file_name, strerror(errno));
        return NULL;
    }

    PGStringList *command_list = pg_string_list_new();
    PGString *command = pg_string_new(4096);
    PGString *buffer = pg_string_new(256);
    char line[4096];
    while (fgets(line, sizeof(line), fp))
    {
        if (line[0] == '\n' || line[0] == '\r')
        {
            continue;
        }
        else
        {
            pg_string_format(buffer, "%s ", line);
            pg_string_join(command, buffer);
            if (strstr(line, ";") || line[0] == '@')
            {
                pg_string_replace(command, "\n", "");
                pg_string_list_add(command_list, command);
                command = pg_string_new(4096);
            }
        }
    }

    pg_string_free(buffer);
    pg_string_free(command);
    fclose(fp);

    PG_LOG_DEBUG("parse_sql_string() end");
    return command_list;
}

int main(int argc, char **argv)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_execute.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

#ifdef DEBUG
    pg_log_set_level(PG_LEVEL_DEBUG);
#else
    pg_log_set_level(PG_LEVEL_INFO);
#endif

    PG_LOG_DEBUG("start main(%s)", argv[1]);

    pg_init();

    PGContext *ctx = NULL;
    PGresult *res = NULL;
    char *sql = NULL;
    bool ret;

    PGStringList *commands = parse_sql_string(argv[1]);
    for (int i = 0; i < pg_string_list_size(commands); i++)
    {
        PGString *line = pg_string_list_get(commands, i);
        PG_LOG_DEBUG("command:%s", pg_string_get(line));

        if (pg_string_find(line, "@connect") != -1)
        {
            PG_LOG_DEBUG("@connect:%s", pg_string_get(line));
            pg_string_replace(line, "@connect ", "");
            PG_LOG_DEBUG("@connect:%s", pg_string_get(line));
            ctx = pg_connect(pg_string_get(line));
            if (!pg_connected(ctx))
            {
                PG_LOG_ERROR(pg_error(ctx));
                PG_LOG_ERROR("Command failed %s", pg_string_get(line));
                goto cleanup;
            }
            PG_LOG_INFO("Command success %s", pg_string_get(line));
        }
        else if (pg_string_find(line, "@disconnect") != -1)
        {
            PG_LOG_DEBUG("@disconnect");
            pg_disconnect(ctx);
            ctx = NULL;
            PG_LOG_INFO("Command success %s", pg_string_get(line));
        }
        else
        {
            PG_LOG_DEBUG("execute command:%s", pg_string_get(line));
            ret = pg_exec(ctx, pg_string_get(line));
            if (!ret)
            {
                PG_LOG_ERROR(pg_error(ctx));
                PG_LOG_ERROR("Command failed %s", pg_string_get(line));
                goto cleanup;
            }
            PG_LOG_INFO("Command success %s", pg_string_get(line));
        }
    }

cleanup:
    pg_disconnect(ctx);
    pg_string_list_free(commands);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}