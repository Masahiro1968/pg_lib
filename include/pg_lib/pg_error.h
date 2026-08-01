/**
 * @file    pg_error.h
 * @brief   PostgreSQL エラー情報関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLで発生したエラー情報に関する定義や関数をここに集約します。
 */

#ifndef PG_ERROR_H
#define PG_ERROR_H

#include "pg_lib.h"

/**
 * @brief     エラー情報をクリアします。
 * @param[in] ctx PGContext
 */
void pg_error_clear(PGContext *ctx);

/**
 * @brief         エラー情報を設定します。
 * @param[in] ctx PGContext
 * @param[in] sql 実行したSQL
 * @param[in] res PGresult
 */
void pg_error_set(PGContext *ctx, const char *sql, PGresult *res);

/**
 * @brief     エラー情報を列挙します。
 * @param[in] fp FILE
 * @param[in] ctx PGContext
 * @details
 *   Postgresが出力する各種エラー情報を列挙します。
 *   - SQLSTATE
 *   - STATUS
 *   - SQL
 *   - MESSAGE
 *   - DETAIL
 *   - HINT
 */
void pg_error_dump(FILE *fp, PGContext *ctx);

#endif