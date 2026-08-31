#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "pg_logger.h"
#include "pg_file.h"
#include "check_functions.h"

#define USE_LOG_FILE 1

int main(void)
{
#if (USE_LOG_FILE == 1)
    const char *log_file = "./test_logger.log";
    remove_file(log_file);
    FILE *fp = fopen(log_file, "w");
    pg_log_set_stream(fp);
#endif

#ifdef DEBUG
    pg_log_set_level(PG_LEVEL_DEBUG);
#else
    pg_log_set_level(PG_LEVEL_INFO);
#endif

    PG_LOG_DEBUG("start main()");

    int rc = 0;

    setlocale(LC_NUMERIC, "");

    /* pg_log_memory() */
    for (int i = 1; i <= 10; i++)
    {
        char *buff = malloc(100000 * i);
        memset(buff, 0, 100000 * i);
        pg_log_memory(PG_LEVEL_INFO);
        free(buff);
        sleep(1);
    }

    PG_LOG_DEBUG("End main()");

#if (USE_LOG_FILE == 1)
    fclose(fp);
#endif

    return rc;
}