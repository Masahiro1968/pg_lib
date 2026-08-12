/**
 * @file    pg_type.h
 * @brief   PostgreSQL タイプ情報
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - PostgreSQLの型に関する定義や関数を集約します。
 */

#ifndef PG_TYPE_H
#define PG_TYPE_H

#include <stdbool.h>
#include <stdint.h>
#include <libpq-fe.h>

/**
 * @brief Postgresで扱う型の列挙体
 */
typedef enum
{
    /* number */
    PG_TYPE_INT,    ///< int型
    PG_TYPE_INT64,  ///< int64型
    PG_TYPE_FLOAT,  ///< float型
    PG_TYPE_DOUBLE, ///< double型
    PG_TYPE_BOOL,   ///< bool型

    /* text */
    PG_TYPE_CHAR, ///< char型
    PG_TYPE_TEXT, ///< テキスト型

    /* date/time */
    PG_TYPE_DATE,      ///< 日付型
    PG_TYPE_TIME,      ///< 日時型
    PG_TYPE_TIMESTAMP, ///< タイムスタンプ型

    /* future */
    PG_TYPE_NUMERIC, ///< numeric型
    PG_TYPE_BINARY   ///< binary型

} PGDataType;

/**
 * @brief フィールド情報構造体
 */
typedef struct
{
    PGDataType type; ///< PGDataType変数
    void *addr;      ///< 変換後に格納されるデータ
    size_t size;     ///< 格納されたデータのサイズ

} PGField;

/* integer */
#define PG_DEFINE_INT(v) {PG_TYPE_INT, &(v), sizeof(v)}
#define PG_DEFINE_INT64(v) {PG_TYPE_INT64, &(v), sizeof(v)}

/* floating point */
#define PG_DEFINE_FLOAT(v) {PG_TYPE_FLOAT, &(v), sizeof(v)}
#define PG_DEFINE_DOUBLE(v) {PG_TYPE_DOUBLE, &(v), sizeof(v)}

/* boolean */
#define PG_DEFINE_BOOL(v) {PG_TYPE_BOOL, &(v), sizeof(v)}

/* character */
#define PG_DEFINE_CHAR(v) {PG_TYPE_CHAR, (v), sizeof(v)}
#define PG_DEFINE_TEXT(v) {PG_TYPE_TEXT, (v), sizeof(v)}

/* date/time */
#define PG_DEFINE_DATE(v) {PG_TYPE_DATE, (v), sizeof(v)}
#define PG_DEFINE_TIME(v) {PG_TYPE_TIME, (v), sizeof(v)}
#define PG_DEFINE_TIMESTAMP(v) {PG_TYPE_TIMESTAMP, (v), sizeof(v)}

/* future */
#define PG_DEFINE_NUMERIC(v) {PG_TYPE_NUMERIC, (v), sizeof(v)}
#define PG_DEFINE_BINARY(v) {PG_TYPE_BINARY, (v), sizeof(v)}

/**
 * @brief     PGresultから型情報(Oid)を取得します。
 * @param[in] res PGresult
 * @param[in] col 列番号
 * @return    型情報(Oid)
 */
Oid pg_col_type(PGresult *res, int col);

/**
 * @brief     PGresultから型名を取得します。
 * @param[in] res PGresult
 * @param[in] col 列番号
 * @return    型名
 * @retval    "bool"
 * @retval    "int8"
 * @retval    "int2"
 * @retval    "int4"
 * @retval    "text"
 * @retval    "float4"
 * @retval    "float8"
 * @retval    "char"
 * @retval    "varchar"
 * @retval    "date"
 * @retval    "timestamp"
 * @retval    "timestampz"
 * @retval    "numeric"
 * @retval    "name" current_database()やcurrent_user(),current_schema()などはこれを使う
 * @retval    "unknown"
 */
const char *pg_col_type_name(PGresult *res, int col);

#endif