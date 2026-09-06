/**
 * @file    pg_file.h
 * @brief   ファイル出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ファイルの操作に関する構造体や関数を集約します。
 */

#ifndef PG_FILE_H
#define PG_FILE_H

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "pg_logger.h"
#include "pg_string.h"

/**
 * @brief     ファイルの存在を確認します。
 * @param[in] file_name 存在可否を確認するファイル名
 * @return    実行結果
 * @retval    true ファイルは存在します。
 * @retval    false ファイルは存在しません。
 * @details   ファイルの確認用で、ディレクトリの場合は
 *            is_exist_directory()を使用します。
 */
bool is_exist_file(const char *file_name);

/**
 * @brief     ディレクトリの存在を確認します。
 * @param[in] directory_name 存在可否を確認するディレクトリ名
 * @return    実行結果
 * @retval    true  ディレクトリは存在します。
 * @retval    false  ディレクトリは存在しません。
 * @details   ディレクトリの確認用で、ファイルの場合は
 *            is_exist_file()を使用します。
 */
bool is_exist_directory(const char *directory_name);

/**
 * @brief     ファイルを削除します。
 * @param[in] file_name 削除するファイル名
 * @return    実行結果
 * @retval    true ファイルの削除成功
 * @retval    false ファイルの削除失敗
 * @details   ファイルの削除に使用します。
 */
bool remove_file(const char *file_name);

/**
 * @brief ディレクトリを削除します。
 * @param[in] directory_name 削除するディレクトリ名
 * @return    実行結果
 * @retval    true ディレクトリの削除成功
 * @retval    false ディレクトリの削除失敗
 * @details   ディレクトリの削除に使用します。
 */
bool remove_directory(const char *directory_name);

/**
 * @brief     ファイル名を変更します。
 * @param[in] file_name 現在のファイル名
 * @param[in] new_file_name 新しいファイル名
 * @return    実行結果
 * @retval    true ファイル名の変更成功
 * @retval    false ファイル名の変更失敗
 * @details   ディレクトリ、ファイルの両方に対応します。
 */
bool rename_file(const char *file_name, const char *new_file_name);

/**
 * @brief     ファイルをコピーします。
 * @param[in] file_name コピー元ファイル名
 * @param[in] target_file_name コピー先ファイル名
 * @return    実行結果
 * @retval    true ファイルのコピー成功
 * @retval    false ファイルのコピー失敗
 * @details   ファイル単体のコピーに使用します。
 */
bool copy_file(const char *file_name, const char *target_file_name);

/**
 * @brief     ファイルを移動します。
 * @param[in] file_name 移動元ファイル名
 * @param[in] target_file_name 移動先ファイル名
 * @return    実行結果
 * @retval    true ファイルの移動成功
 * @retval    false ファイルの移動失敗
 * @details   ファイル単体の移動時に使用します。
 */
bool move_file(const char *file_name, const char *target_file_name);

/**
 * @brief     ディレクトリを新規に作成します。
 * @param[in] directory_name 新規に作成するディレクトリ名
 * @return    実行結果
 * @retval    true ディレクトリの作成成功
 * @retval    false ディレクトリの作成失敗
 */
bool create_directory(const char *directory_name);

/**
 * @brief     ディレクトリをコピーします。
 * @param[in] directory_name コピーするディレクトリ名
 * @param[in] target_directory_name コピー先のディレクトリ名
 * @return    実行結果
 * @retval    true ディレクトリのコピー成功
 * @retval    false ディレクトリのコピー失敗
 * @details   ディレクトリのコピーに使用します。
 * @note      まだ実装ができていません。
 */
bool copy_directory(const char *directory_name, const char *target_directory_name);

/**
 * @brief     ディレクトリを移動します。
 * @param[in] directory_name 移動するディレクトリ名
 * @param[in] target_directory_name 移動先のディレクトリ名
 * @return    実行結果
 * @retval    true ディレクトリの移動成功
 * @retval    false ディレクトリの移動失敗
 * @details   ディレクトリの移動に使用します。
 * @note      ファイルシステムをまたぐ場合の処理が未実装です。
 */
bool move_directory(const char *directory_name, const char *target_directory_name);

/**
 * @brief     ファイル一覧を取得します。
 * @param[in] path 取得対象のパス
 * @param[in] is_file 取得対象がファイルならtrue, ディレクトリならfalse
 * @return    PGStringList ファイル一覧
 * @retval    NULL 取得失敗
 */
PGStringList *get_file_entry_list(const char *path, bool is_file);

/**
 * @brief     相対パスをrealpath()で絶対パスに変換します。
 * @param[in] path 相対パス
 * @return    PGString 変換した絶対パス
 * @retval    NULL 取得失敗
 */
PGString *get_real_path(const char *path);

/**
 * @brief     Linuxコマンドを実行し、結果をPGStringに格納します。
 * @param[in] command Linuxコマンド
 * @return    PGString 実行結果
 * @retval    NULL 取得失敗
 */
PGString *execute_command(const char *command);

/**
 * @brief 行末を検知するまで、バッファを拡張して読み込みます。
 * @param[in] fp FILE*
 * @param[in] initial_size バッファの初期サイズ
 * @param[in] keep_newline falseにすると、行末のCRやLFを除去します。
 * @return    PGString 実行結果
 * @retval    NULL 読み込み失敗
 */
PGString *read_line(FILE *fp, size_t initial_size, bool keep_newline);

#endif