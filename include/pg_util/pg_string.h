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
#include <stdarg.h>

/**
 * @brief 文字列情報構造体
 */
typedef struct
{
    char *data;      ///< 文字列情報
    size_t size;     ///< 文字列のサイズ
    size_t capacity; ///< 実際に確保しているサイズ
} PGString;

/**
 * @brief 文字列情報配列構造体
 */
typedef struct
{
    PGString **items; ///< 文字列情報構造体
    size_t count;     ///< 文字列情報構造体のサイズ
} PGStringList;

/**
 * @brief     文字列格納領域を確保します。
 * @param[in] size 文字列サイズ（NULL終端を含まない）
 * @return    PGString
 * @retval    NULL 失敗
 * @retval    PGString 成功
 */
PGString *pg_string_new(size_t size);

/**
 * @brief     文字列バッファの容量を確保します。
 * @param[in] string PGString 領域を確保する対象
 * @param[in] capacity バッファサイズ（NULL終端を含む）
 * @return    実行結果
 * @retval    0 成功
 * @retval    -1 失敗
 */
int pg_string_reserve(PGString *string, size_t capacity);

/**
 * @brief     文字列情報を開放します。
 * @param[in] string PGString 領域を開放する対象
 */
void pg_string_free(PGString *string);

/**
 * @brief      文字列を文字列情報に設定します。
 * @param[out] string PGString 文字列を設定する対象
 * @param[in]  text 設定する文字列
 * @return     設定された文字列サイズ
 * @details    -1の場合は、エラー発生
 */
int pg_string_set(PGString *string, const char *text);

/**
 * @brief      文字列情報をフォーマットで構築します。
 * @param[out] string PGString 文字列を構築する対象
 * @param[in]  format printf()に準拠したフォーマット文字列
 * @return     実際に構築された文字列長
 * @retval     -1 失敗
 */
int pg_string_format(PGString *string, const char *format, ...);

/**
 * @brief     設定された文字列を文字列情報から取得します。
 * @param[in] string PGString 文字列を取得する対象
 * @return    設定された文字列
 */
const char *pg_string_get(const PGString *string);

/**
 * @brief     設定された文字列のサイズを取得します。
 * @param[in] string PGString 文字列のサイズを取得する対象
 * @return    設定された文字列のサイズ
 */
int pg_string_size(const PGString *string);

/**
 * @brief         文字列情報を連結します。
 * @param[in,out] base_string PGString 元になる文字列情報
 * @param[in]     append_string PGString 追加する文字列情報
 * @return        base_stringの文字列サイズ
 */
int pg_string_join(PGString *base_string, const PGString *append_string);

/**
 * @brief         文字列の左側の余白を削除します。
 * @param[in,out] string PGString 余白を削除する対象
 */
void pg_string_trim_left(PGString *string);

/**
 * @brief         文字列の右側の余白を削除します。
 * @param[in,out] string PGString 余白を削除する対象
 */
void pg_string_trim_right(PGString *string);

/**
 * @brief         文字列の左右の余白を削除します。
 * @param[in,out] string PGString 余白を削除する対象
 */
void pg_string_trim(PGString *string);

/**
 * @brief         小数点以下の数字に０が続く場合に削除します。
 * @param[in,out] string PGString 浮動小数点の文字列
 */
void pg_string_trim_trailing_zeros(PGString *string);

/**
 * @brief     文字列情報の文字列の中から、デリミタを検出して配列に分割します。
 * @param[in] string PGString デリミタで分割する対象
 * @param[in] delimiter 分割単位のデリミタ文字
 * @return    PGStringList 文字列情報配列
 */
PGStringList *pg_string_split(const PGString *string, char delimiter);

/**
 * @brief  文字列情報配列を生成します。
 * @return 文字列情報配列
 */
PGStringList *pg_string_list_new();

/**
 * @brief     文字列情報配列を開放します。
 * @param[in] list PGStringList 領域を開放する対象
 */
void pg_string_list_free(PGStringList *list);

/**
 * @brief     文字列情報から指定の文字列を検索します。
 * @param[in] string PGString 文字列を検索する対象
 * @param[in] needle 検索する文字列
 * @return    条件に一致した文字列の先頭からの位置<br>
 *            見つからない場合は、-1を返します。
 */
int pg_string_find(PGString *string, const char *needle);

/**
 * @brief         文字列情報から文字列を置換します。
 * @param[in,out] string PGString 文字列を置換する対象
 * @param[in]     from 検索文字列
 * @param[in]     to 置換文字列
 * @return        置換後のstringのサイズ
 */
int pg_string_replace(PGString *string, const char *from, const char *to);

/**
 * @brief     PGStringListにある、PGStringの件数を取得します。
 * @param[in] list PGStringList 件数を数える対象
 * @return    PGStringListにある、PGStringの件数
 */
int pg_string_list_size(PGStringList *list);

/**
 * @brief     PGStringListから、PGStringを取得します。
 * @param[in] list PGStringList PGStringを取得する対象
 * @param[in] count 先頭(0)からの件数
 * @return    listのcountにあるPGString
 */
PGString *pg_string_list_get(PGStringList *list, int count);

/**
 * @brief     PGStringListに、PGStringを追加します。
 * @param[in] list PGStringList PGStringを追加する対象
 * @param[in] string PGString 追加する文字列情報
 * @return    追加後のPGStringListの件数
 * @note      stringは、この関数でポインタ保持するので、
 *            stringの削除は、pg_string_list_free()で
 *            行います。個別の削除は不要です。
 */
int pg_string_list_add(PGStringList *list, PGString *string);

/**
 * @brief         文字列全体を大文字に変換します。
 * @param[in,out] string PGString 変換する文字列
 * @details       入力された文字列そのものを変換するので、破壊されたくない場合<br>
 *                文字列のコピーを使用してください。
 */
void pg_string_to_upper(PGString *string);

/**
 * @brief         文字列全体を小文字に変換します。
 * @param[in,out] string PGString 変換する文字列
 * @details       入力された文字列そのものを変換するので、破壊されたくない場合<br>
 *                文字列のコピーを使用してください。
 */
void pg_string_to_lower(PGString *string);

/**
 * @brief     指定した位置から指定文字数分の部分文字列を生成して返します。
 * @param[in] string PGString 対象の文字列
 * @param[in] start 開始インデックス (0オリジン)
 * @param[in] count 抽出する文字数
 * @return    新しく生成された PGString* (失敗または範囲外の場合は NULL または空文字列)
 * @details   生成された文字列は、最後にpg_string_free()で開放してください。
 */
PGString *pg_string_mid(const PGString *string, size_t start, size_t count);

/**
 * @brief     先頭から指定文字数分の部分文字列を生成して返します。
 * @param[in] string PGString 対象の文字列
 * @param[in] count 抽出する文字数
 * @return    新しく生成された PGString* (失敗または範囲外の場合は NULL または空文字列)
 * @details   生成された文字列は、最後にpg_string_free()で開放してください。
 */
PGString *pg_string_left(const PGString *string, size_t count);

/**
 * @brief     末尾から指定文字数分の部分文字列を生成して返します。
 * @param[in] string PGString 対象の文字列
 * @param[in] count 抽出する文字数
 * @return    新しく生成された PGString* (失敗または範囲外の場合は NULL または空文字列)
 * @details   生成された文字列は、最後にpg_string_free()で開放してください。
 */
PGString *pg_string_right(const PGString *string, size_t count);

/**
 * @brief     文字列を末尾から検索し、最初に見つかったインデックスを返します。
 * @param[in] string PGString 検索対象となる文字列
 * @param[in] needle 検索する文字列
 * @return    見つかったインデックス（見つからない、またはエラー時は -1）
 */
int pg_string_reverse_find(const PGString *string, const char *needle);

/**
 * @brief     先頭と末尾が指定した引用符（ダブルクォート等）で囲まれている場合のみ、それを取り除きます。
 * @param[in] string PGString 引用符を除去する文字列
 * @param[in] quote_char 取り除きたい引用符文字 ('"' や '\'' など)
 * @return    実際に取り除いた場合は 1、変化がなかった場合は 0、エラーは -1
 */
int pg_string_unquote(PGString *string, char quote_char);

#endif