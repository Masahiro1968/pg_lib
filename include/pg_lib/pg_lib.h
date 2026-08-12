/**
 * @file    pg_lib.h
 * @brief   PostgreSQL pg_lib関数
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLの主要な関数をここに集約します。
 */

#ifndef PG_LIB_H
#define PG_LIB_H

#include <stdbool.h>
#include <libpq-fe.h>
#include "pg_type.h"

#define PG_LIB_VERSION "0.1.1"
#define PG_DEFAULT_TIMEOUT 30

/**
 * @brief   Postgres接続情報を保持する構造体
 * @details 発生したエラー情報もここに格納します。
 */
typedef struct
{
    PGconn *conn; ///< Postgres接続情報

    char last_sqlstate[6]; ///< PQresultErrorField(res, PG_DIAG_SQLSTATE)
    char last_error[1024]; ///< PQerrorMessage(ctx->conn)を格納
    char last_sql[1024];   ///< 最後に実行したSQL文字列

    char last_message[256]; ///< PQresultErrorField(res, PG_DIAG_MESSAGE_PRIMARY)
    char last_detail[256];  ///< PQresultErrorField(res, PG_DIAG_MESSAGE_DETAIL)
    char last_hint[256];    ///< PQresultErrorField(res, PG_DIAG_MESSAGE_HINT)

    ExecStatusType last_status; ///< PQresultStatus()を格納

} PGContext;

/**
 * @brief このライブラリを初期化します。
 * @note  将来対応で、現時点では機能はありません。
 */
void pg_init(void);

/**
 * @brief     Postgresに接続します。
 * @param[in] conninfo 接続文字列
 * @return    PGContext
 * @note      接続失敗でもNULLを返しません。
 *            接続確認は、pg_connected()を使用して確認します。
 */
PGContext *pg_connect(const char *conninfo);

/**
 * @brief     Postgresから切断します。
 * @param[in] ctx PGContext
 * @note      切断後はctxは使用できません。
 */
void pg_disconnect(PGContext *ctx);

/**
 * @brief     SELECT以外のSQLを実行します。
 * @param[in] ctx PGContext
 * @param[in] sql 実行するSQL
 * @return    実行結果
 * @retval    true 成功
 * @retval    false 失敗
 */
bool pg_exec(PGContext *ctx, const char *sql);

/**
 * @brief     SELECTを実行します。
 * @param[in] ctx PGContext
 * @param[in] sql 実行するSQL
 * @return    実行結果
 */
PGresult *pg_query(PGContext *ctx, const char *sql);

/**
 * @brief     トランザクションを開始します。
 * @param[in] ctx PGContext
 */
bool pg_begin(PGContext *ctx);

/**
 * @brief     トランザクションを確定します。
 * @param[in] ctx PGContext
 */
bool pg_commit(PGContext *ctx);

/**
 * @brief     トランザクションを破棄します。
 * @param[in] ctx PGContext
 */
bool pg_rollback(PGContext *ctx);

/**
 * @brief     SQL実行後の行数を取得します。
 * @param[in] res PGresult
 * @return    取得した行数
 */
int pg_rows(PGresult *res);

/**
 * @brief     SQL実行後の列数を取得します。
 * @param[in] res PGresult
 * @return    取得した列数
 */
int pg_cols(PGresult *res);

/**
 * @brief     列名を取得します。
 * @param[in] res PGresult
 * @param[in] col 列番号
 * @return    列名
 */
const char *pg_col_name(PGresult *res, int col);

/**
 * @brief     PGresultから行列情報(文字列)を取得します。
 * @param[in] res 結果セットの入っているPGresult
 * @param[in] row 行番号
 * @param[in] col 列番号
 * @return    row, colで指定したPGresultのデータ
 */
const char *pg_value(PGresult *res, int row, int col);

/**
 * @brief        PGresultから型定義した変数にデータを格納します。
 * @param[in]    res 結果セットの入っているPGresult
 * @param[in]    row 結果セットから取得する行番号
 * @param[inout] fields 結果セットから取得する列情報
 * @param[in]    count fieldsに格納した列情報数
 * @return       実行結果
 * @retval       1 成功
 * @details      現状は「成功」しか返しません。
 * @details      pg_value()は文字列でしか返せませんが、この関数は
 *               PGField情報に応じて型変換して値を返します。
 */
int pg_fetch_row(PGresult *res, int row, PGField *fields, int count);

/**
 * @brief     取得した結果セットを開放します。
 * @param[in] res PGresult
 */
void pg_result_free(PGresult *res);

/**
 * @brief     発生したエラー情報を取得します。
 * @param[in] ctx PGContext
 * @return    エラーメッセージ
 */
const char *pg_error(PGContext *ctx);

/**
 * @brief     結果セットの妥当性を確認します。
 * @param[in] res PGresult
 * @return    実行結果
 * @retval    1 成功
 * @retval    0 失敗
 */
int pg_ok(PGresult *res);

/**
 * @brief     Postgresへの接続状況を確認します。
 * @param[in] ctx PGContext
 * @return    実行結果
 * @retval    1 成功
 * @retval    0 失敗
 */
int pg_connected(PGContext *ctx);

/**
 * @brief     NULLデータの判定をします。
 * @param[in] res PGresult
 * @param[in] row 行番号
 * @param[in] col 列番号
 * @return    実行結果
 * @retval    true NULLデータ
 * @retval    false データあり
 */
bool pg_is_null(PGresult *res, int row, int col);

#endif