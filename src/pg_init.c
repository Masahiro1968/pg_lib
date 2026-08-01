/**
 * @file    pg_init.c
 * @brief   PostgreSQL 初期化関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ライブラリの初期化関数をここに集約します。
 */

#include "pg_lib.h"

static int initialized;

void pg_init(void)
{
    if (initialized)
        return;

    initialized = 1;
}