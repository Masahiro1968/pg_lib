/**
 * @file    pg_utility.h
 * @brief   PostgreSQL ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLに関連するユーティリティを集約します。
 */

#ifndef PG_UTILITY_H
#define PG_UTILITY_H

#include "pg_lib.h"

/**
 * @brief     使う予定のない引数への警告を抑制させます。
 * @param[in] x 警告抑制させる引数名
 */
#define UNUSED(x) (void)(x)

/**
 * @brief     配列の件数を取得します。
 * @param[in] a 配列の変数名
 */
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/**
 * @brief Postgresの'NULL'を定義します。
 */
#define PG_NULL ((char *)0)

/**
 * @brief 日付と時刻を集約した構造体
 */
typedef struct
{
    int year;  ///< 年
    int month; ///< 月
    int day;   ///< 日

    int hour;   ///< 時
    int minute; ///< 分
    int second; ///< 秒

} PGDateTime;

/**
 * @brief     日付文字列を日付と時刻を集約した構造体に分解します。
 * @param[in] src 日時文字列
 * @param[in] dst PGDateTime
 * @return    実行結果
 * @retval    1 成功
 */
int pg_datetime_parse(const char *src, PGDateTime *dst);

/**
 * @brief     テーブル一覧を取得します。
 * @param[in] ctx PGcontext
 * @return    PGresult
 */
PGresult *pg_tables(PGContext *ctx);

#endif