/**
 * @file    pg_logger.c
 * @brief   ログ出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ログに関する構造体や関数を集約します。
 */

#include "pg_logger.h"

static FILE *g_logger = NULL;
static PGLogLevel g_suppressLogLevel = PG_LEVEL_INFO;

void pg_log_set_level(PGLogLevel aboveLogLevel)
{
    g_suppressLogLevel = aboveLogLevel;
}

void pg_log_set_stream(FILE *fp)
{
    g_logger = fp;
}

void pg_log(PGLogLevel level, const char *fmt, ...)
{
    if (g_logger == NULL)
    {
        g_logger = stdout;
    }

    if (level < g_suppressLogLevel)
        return;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    struct tm lt;
    localtime_r(&ts.tv_sec, &lt);

    char datetime[32];
    strftime(datetime, sizeof(datetime),
             "%Y-%m-%d %H:%M:%S", &lt);

    const char *name;

    switch (level)
    {
    case PG_LEVEL_DEBUG:
        name = "DEBUG";
        break;

    case PG_LEVEL_INFO:
        name = "INFO ";
        break;

    case PG_LEVEL_WARN:
        name = "WARN ";
        break;

    case PG_LEVEL_ERROR:
        name = "ERROR";
        break;

    default:
        name = "UNKNOWN";
        break;
    }

    fprintf(g_logger,
            "[%s.%03ld][%s] ",
            datetime,
            ts.tv_nsec / 1000000,
            name);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(g_logger, fmt, ap);
    va_end(ap);

    fputc('\n', g_logger);
    fflush(g_logger);
}