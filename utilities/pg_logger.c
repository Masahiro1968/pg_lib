/**
 * @file    pg_logger.c
 * @brief   ログ出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ログに関する構造体や関数を集約します。
 */

#define _DEFAULT_SOURCE

#include <string.h>
#include <stdlib.h>
#include "pg_logger.h"

static FILE *g_logger = NULL;
static PGLogLevel g_suppress_log_level = PG_LEVEL_INFO;

void pg_log_set_level(PGLogLevel above_log_level)
{
    g_suppress_log_level = above_log_level;
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

    if (level < g_suppress_log_level)
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

void pg_log_memory(PGLogLevel level)
{
    FILE *fp = fopen("/proc/self/status", "r");
    if (!fp)
    {
        return;
    }

    char line[256];

    long vmpeak_kb = 0;
    long vmsize_kb = 0;
    long vmrss_kb = 0;

    int checked = 0;

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "VmSize:", 7) == 0)
        {
            sscanf(line, "VmSize: %ld kB", &vmsize_kb);
            checked++;
        }
        else if (strncmp(line, "VmRSS:", 6) == 0)
        {
            sscanf(line, "VmRSS: %ld kB", &vmrss_kb);
            checked++;
        }
        else if (strncmp(line, "VmPeak:", 7) == 0)
        {
            sscanf(line, "VmPeak: %ld kB", &vmpeak_kb);
            checked++;
        }

        if (checked == 3)
            break;
    }

    pg_log(level, "VmPeak:%'ld MB, VmSize:%'ld MB, VmRSS:%'ld MB",
           (vmpeak_kb / 1024), (vmsize_kb / 1024), (vmrss_kb / 1024));

    fclose(fp);
}