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
#include "pg_string.h"

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
 * @brief      日付文字列を日付と時刻を集約した構造体に分解します。
 * @param[in]  src 日時文字列
 * @param[out] dst PGDateTime
 * @return     実行結果
 * @retval     1 成功
 * @retval     0 失敗
 */
int pg_datetime_parse(const char *src, PGDateTime *dst);

/**
 * @brief     テーブル一覧を取得します。
 * @param[in] ctx PGcontext
 * @param[in] schema_name 対象のスキーマ名
 * @return    PGresult
 * @details   'tablename'でテーブル名を返却します。<br>
 *            schema_nameがNULLの場合、"public"を検索します。
 */
PGresult *pg_tables(PGContext *ctx, const char *schema_name);

/**
 * @brief     テーブルから１行単位でデータを取得します。
 * @param[in] res PGresult
 * @param[in] row 行番号
 * @return    PGStringList
 * @retval    NULL 失敗
 * @retval    PGStringList 成功
 * @details   取得したPGStringListは、pg_string_list_free()で開放してください。
 */
PGStringList *pg_get_row(PGresult *res, int row);

/**
 * @brief     テーブルからフィールド名一覧を取得します。
 * @param[in] res PGresult
 * @return    PGStringList
 * @retval    NULL 失敗
 * @retval    PGStringList 成功
 * @details   取得したPGStringListは、pg_string_list_free()で開放してください。
 */
PGStringList *pg_get_field_names(PGresult *res);

/**
 * @brief     データ一覧からブラケット、デリミタをつけて出力用データを作成します。
 * @param[in] row_data PGStringList
 * @param[in] delimiter データ単位のデリミタ文字
 * @param[in] blacket   データ単位の囲い文字
 * @param[in] eol       行単位のデリミタ文字
 * @return    PGString 行単位の文字列
 */
PGString *pg_make_data(PGStringList *row_data, const char *delimiter, const char *blacket, const char *eol);

/**
 * @brief 接続文字列を作成します。
 * @param[out] connection_string 作成した接続文字列
 * @param[in]  host ホスト名
 * @param[in]  port ポート番号
 * @param[in]  dbname データベース名
 * @param[in]  user ユーザー名
 * @param[in]  pass パスワード
 * @param[in]  service サービス名
 * @return     実行結果
 * @retval     true 成功
 * @retval     false 失敗
 * @details    各設定値に対応するキーワードをイコールでつないで文字列を構成します。
 */
bool pg_build_connection_string(
    PGString *connection_string,
    char *host,
    char *port,
    char *dbname,
    char *user,
    char *pass,
    char *service);

#endif