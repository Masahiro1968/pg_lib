/**
 * @file    pg_logger.h
 * @brief   ログ出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ログに関する構造体や関数を集約します。
 */

#ifndef PG_LOGGER_H
#define PG_LOGGER_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

/**
 * @brief ログのレベル列挙体
 */
typedef enum
{
    PG_LEVEL_DEBUG, ///< DEBUGレベル
    PG_LEVEL_INFO,  ///< INFOレベル
    PG_LEVEL_WARN,  ///< WARNレベル
    PG_LEVEL_ERROR  ///< ERRORレベル

} PGLogLevel;

/**
 * @brief     エラーの出力レベルを設定します。
 * @param[in] aboveLogLevel 出力制限レベル
 * @details   aboveLogLevel以上を出力対象に設定します。
 */
void pg_log_set_level(PGLogLevel aboveLogLevel);

/**
 * @brief     ログの出力先を設定します。
 * @param[in] fp ログの出力先
 */
void pg_log_set_stream(FILE *fp);

/**
 * @brief ログを出力します。
 * @param[in] level 出力するログレベル
 * @param[in] fmt   出力文字のフォーマット
 * @param[in] ...   可変引数の出力内容
 */
void pg_log(PGLogLevel level, const char *fmt, ...);

/** @brief DEBUGログを出力します。 */
#define PG_LOG_DEBUG(...) pg_log(PG_LEVEL_DEBUG, __VA_ARGS__)

/** @brief INFOログを出力します。 */
#define PG_LOG_INFO(...) pg_log(PG_LEVEL_INFO, __VA_ARGS__)

/** @brief WARNログを出力します。 */
#define PG_LOG_WARN(...) pg_log(PG_LEVEL_WARN, __VA_ARGS__)

/** @brief ERRORログを出力します。 */
#define PG_LOG_ERROR(...) pg_log(PG_LEVEL_ERROR, __VA_ARGS__)

#endif