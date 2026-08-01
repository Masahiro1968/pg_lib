/**
 * @file    pg_string.h
 * @brief   文字列操作ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - 文字列操作に関する構造体や関数を集約します。
 */

#ifndef PG_STRING_H
#define PG_STRING_H

#include <stddef.h>

/**
 * @brief 文字列情報の構造体
 */
typedef struct
{
    char *data;  ///< 文字列情報
    size_t size; ///< 文字列のサイズ
} PGString;

/**
 * @brief 文字列情報の配列情報
 */
typedef struct
{
    PGString **items; ///< 文字列情報の構造体
    size_t count;     ///< 構造体のサイズ
} PGStringList;

/**
 * @brief     文字列情報を生成します。
 * @param[in] size 文字列サイズ
 * @return    PGString
 */
PGString *pg_string_new(size_t size);

/**
 * @brief 文字列情報を開放します。
 * @param[in] string 開放する文字列情報
 */
void pg_string_free(PGString *string);

/**
 * @brief     文字列を文字列情報に設定します。
 * @param[in] string 設定する文字列情報
 * @param[in] text 設定する文字列
 * @return    設定された文字列サイズ
 */
int pg_string_set(PGString *string, const char *text);

/**
 * @brief     設定された文字列を文字列情報から取得します。
 * @param[in] string PGString
 * @return    設定された文字列
 */
const char *pg_string_get(PGString *string);

/**
 * @brief     設定された文字列のサイズを取得します。
 * @param[in] string PGString
 * @return    設定された文字列のサイズ
 */
int pg_string_size(PGString *string);

/**
 * @brief     文字列情報を連結します。
 * @param[in] base_string 元になる文字列情報
 * @param[in] append_string 追加する文字列情報
 * @return    base_stringの文字列サイズ
 */
int pg_string_join(PGString *base_string, PGString *append_string);

/**
 * @brief     文字列の左側の余白を削除します。
 * @param[in] string 文字列情報
 */
void pg_string_trim_left(PGString *string);

/**
 * @brief     文字列の右側の余白を削除します。
 * @param[in] string 文字列情報
 */
void pg_string_trim_right(PGString *string);

/**
 * @brief     文字列の左右の余白を削除します。
 * @param[in] string 文字列情報
 */
void pg_string_trim(PGString *string);

/**
 * @brief     文字列情報の文字列の中から、デリミタを検出して配列に分割します。
 * @param[in] string PGString
 * @param[in] delimiter 分割単位のデリミタ文字
 * @return    文字列情報配列
 */
PGStringList pg_string_split(PGString *string, char delimiter);

/**
 * @brief     文字列情報配列を開放します。
 * @param[in] list PGStringList
 */
void pg_string_list_free(PGStringList *list);

/**
 * @brief     文字列情報から指定の文字列を検索します。
 * @param[in] string 文字列情報
 * @param[in] needle 検索する文字列
 * @return    条件に一致した文字列の先頭からの位置
 */
int pg_string_find(PGString *string, const char *needle);

/**
 * @brief     文字列情報から文字列を置換します。
 * @param[in] string 文字列情報
 * @param[in] from 検索文字列
 * @param[in] to 置換文字列
 */
int pg_string_replace(PGString *string, const char *from, const char *to);

#endif