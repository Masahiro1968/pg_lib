/**
 * @file    sample_error_check.c
 * @brief   SQLエラーに対するエラー出力の確認用サンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 */

#include <stdio.h>
#include <string.h>
#include "pg_lib.h"
#include "pg_error.h"
#include "pg_type.h"
#include "pg_logger.h"
#include "pg_file.h"
#include "pg_utility.h"
#include "pg_string.h"

#define USE_LOG_FILE 1

PGString *make_connection_string(
    const char *hostname,
    const char *port,
    const char *dbname,
    const char *user,
    const char *password,
    const char *service)
{
    const char *my_hostname = hostname ? hostname : "localhost";
    const char *my_port = port ? port : "5432";
    const char *my_dbname = dbname ? dbname : "postgres";
    const char *my_user = user ? user : "postgres";
    const char *my_pass = password ? password : "postgres";
    const char *my_service = service ? service : NULL;

    PGString *connection_string = pg_string_new(1024);
    if (pg_build_connection_string(
            connection_string, my_hostname, my_port, my_dbname, my_user, my_pass, my_service))
    {
        return connection_string;
    }
    else
    {
        pg_string_free(connection_string);
        return NULL;
    }
}

void connection_string_error(PGString *connection_string, FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_error()");

    PGContext *ctx = pg_connect(pg_string_get(connection_string));
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end connection_string_error()");
}

void connection_string_host_error(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_host_error()");
    PGString *conn_str = make_connection_string("somehost", NULL, NULL, NULL, NULL, NULL);
    connection_string_error(conn_str, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end connection_string_host_error()");
}

void connection_string_port_error(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_port_error()");
    PGString *conn_str = make_connection_string(NULL, "1234", NULL, NULL, NULL, NULL);
    connection_string_error(conn_str, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end connection_string_port_error()");
}

void connection_string_dbname_error(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_dbname_error()");
    PGString *conn_str = make_connection_string(NULL, NULL, "database", NULL, NULL, NULL);
    connection_string_error(conn_str, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end connection_string_dbname_error()");
}

void connection_string_user_error(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_user_error()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, "SQLite3", NULL, NULL);
    connection_string_error(conn_str, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end connection_string_user_error()");
}

void connection_string_password_error(FILE *out)
{
    PG_LOG_DEBUG("begin connection_string_password_error()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, "1234567890", NULL);
    connection_string_error(conn_str, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end connection_string_password_error()");
}

void sql_error(const char *connection_string, const char *valid_sql, const char *invalid_sql, FILE *out)
{
    PG_LOG_DEBUG("begin sql_error()");

    PGContext *ctx = pg_connect(connection_string);
    if (!pg_connected(ctx))
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    if (valid_sql)
    {
        PG_LOG_INFO("SQL=%s", valid_sql);
        bool ret = pg_exec(ctx, valid_sql);
        if (!ret)
        {
            PG_LOG_ERROR(pg_error(ctx));
            pg_error_dump(out, ctx);
        }
    }

    PG_LOG_INFO("SQL=%s", invalid_sql);
    bool ret = pg_exec(ctx, invalid_sql);
    if (!ret)
    {
        PG_LOG_ERROR(pg_error(ctx));
        pg_error_dump(out, ctx);
    }

    pg_disconnect(ctx);

    PG_LOG_DEBUG("end sql_error()");
}

void sql_error_invalid_command(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_invalid_command()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);
    sql_error(pg_string_get(conn_str), NULL, "selectX 1", out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_invalid_command()");
}

void sql_error_oracle_word(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_oracle_word()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);
    sql_error(pg_string_get(conn_str), NULL, "select * from dual", out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_oracle_word()");
}

void sql_error_invalid_statement(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_invalid_statement()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);

    const char *valid_sql =
        "CREATE TEMP TABLE tmp_table ("
        "  id SERIAL,"
        "  word TEXT,"
        "  meaning TEXT"
        ");";

    const char *invalid_sql = "insert int tmp_table values(1,2,3,4,5)";

    sql_error(pg_string_get(conn_str), valid_sql, invalid_sql, out);

    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_invalid_statement()");
}

void sql_error_too_few_values(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_too_few_values()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);

    const char *valid_sql =
        "CREATE TEMP TABLE tmp_table ("
        "  id SERIAL,"
        "  word TEXT,"
        "  meaning TEXT"
        ");";

    const char *invalid_sql = "insert into tmp_table (id, word, meaning) values(1,'word_1')";

    sql_error(pg_string_get(conn_str), valid_sql, invalid_sql, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_too_few_values()");
}

void sql_error_too_many_values(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_too_many_values()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);

    const char *valid_sql =
        "CREATE TEMP TABLE tmp_table ("
        "  id SERIAL,"
        "  word TEXT,"
        "  meaning TEXT"
        ");";

    const char *invalid_sql = "insert into tmp_table values(1,'word_1','mean_1',4,5)";

    sql_error(pg_string_get(conn_str), valid_sql, invalid_sql, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_too_many_values()");
}

void sql_error_invalid_field(FILE *out)
{
    PG_LOG_DEBUG("begin sql_error_invalid_field()");
    PGString *conn_str = make_connection_string(NULL, NULL, NULL, NULL, NULL, NULL);

    const char *valid_sql =
        "CREATE TEMP TABLE tmp_table ("
        "  id SERIAL,"
        "  word TEXT,"
        "  meaning TEXT"
        ");";

    const char *invalid_sql = "update tmp_table set context = 'context'";

    sql_error(pg_string_get(conn_str), valid_sql, invalid_sql, out);
    pg_string_free(conn_str);
    PG_LOG_DEBUG("end sql_error_invalid_field()");
}

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./sample_error_check.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#else
    FILE *fp = stdout;
#endif

#ifdef DEBUG
    pg_log_set_level(PG_LEVEL_DEBUG);
#else
    pg_log_set_level(PG_LEVEL_INFO);
#endif

    PG_LOG_DEBUG("start main()");

    pg_init();

    connection_string_host_error(fp);
    connection_string_port_error(fp);
    connection_string_dbname_error(fp);
    connection_string_user_error(fp);
    connection_string_password_error(fp);

    sql_error_invalid_command(fp);
    sql_error_oracle_word(fp);
    sql_error_invalid_statement(fp);
    sql_error_too_few_values(fp);
    sql_error_too_many_values(fp);
    sql_error_invalid_field(fp);

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return 0;
}