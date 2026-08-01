/**
 * @file    pg_statement.h
 * @brief   PostgreSQL ステートメント情報
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLのSQL事前実行に関する定義や関数を集約します。
 */

#ifndef PG_STATEMENT_H
#define PG_STATEMENT_H

#include "pg_lib.h"

/**
 * @brief SQLの事前実行情報を保持する構造体
 */
typedef struct
{
    PGContext *ctx;  ///< PGContext情報
    char name[64];   ///< ステートメント名
    int param_count; ///< パラメータ件数
} PGStmt;

/**
 * @brief          SQLの事前実行情報を設定します
 * @param[in] ctx  PGContext
 * @param[in] name 事前実行上に設定する名称
 * @param[in] sql  事前実行させるSQL
 * @return         PGStmt
 */
PGStmt *pg_prepare(PGContext *ctx, const char *name, const char *sql);

/**
 * @brief             SQLの事前実行情報を元にSQLを実行する
 * @param[in] stmt    PGStmt
 * @param[in] nparams パラメータの件数
 * @param[in] params  パラメータ
 * @return            PGresult
 */
PGresult *pg_execute(PGStmt *stmt, int nparams, const char **params);

/**
 * @brief     SQLの事前実行情報を開放します。
 * @param[in] stmt PGStmt
 */
void pg_stmt_free(PGStmt *stmt);

/**
 * @brief     パラメータ付きSQLをカーソルでオープンします。
 * @param[in] ctx PGContext
 * @param[in] cursor_name カーソル名
 * @param[in] sql 実行するSQL
 * @param[in] nparams パラメータの件数
 * @param[in] params パラメータ
 */
bool pg_open_cursor(
    PGContext *ctx, const char *cursor_name, const char *sql,
    int nparams, const char **params);

#endif