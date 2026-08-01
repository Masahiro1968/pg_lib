/**
 * @file    pg_file.c
 * @brief   ファイル出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ファイルの操作に関する構造体や関数を集約します。
 */

#include "pg_file.h"

static bool is_exist(const char *file_name)
{
    struct stat st;
    int response = stat(file_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("stat failed for '%s', errno=%d (%s)", file_name, errno, error);
        return false;
    }
    return true;
}

bool is_exist_file(const char *file_name)
{
    if (!file_name)
    {
        PG_LOG_DEBUG("parameter 'file_name' is null");
        return false;
    }

    struct stat st;
    int response = stat(file_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("stat failed for '%s', errno=%d (%s)", file_name, errno, error);
        return false;
    }
    else
    {
        int res = S_ISREG(st.st_mode);
        if (!res)
        {
            PG_LOG_DEBUG("%s is not a file.", file_name);
            return false;
        }
    }

    return true;
}

bool is_exist_directory(const char *directory_name)
{
    if (!directory_name)
    {
        PG_LOG_DEBUG("parameter 'directory_name' is null");
        return false;
    }

    struct stat st;
    int response = stat(directory_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("stat failed for '%s', errno=%d (%s)", directory_name, errno, error);
        return false;
    }
    else
    {
        int res = S_ISDIR(st.st_mode);
        if (!res)
        {
            PG_LOG_DEBUG("%s is not a directory.", directory_name);
            return false;
        }
    }

    return true;
}

bool remove_file(const char *file_name)
{
    if (!file_name)
    {
        PG_LOG_DEBUG("parameter 'file_name' is null");
        return false;
    }

    if (!is_exist(file_name))
    {
        PG_LOG_DEBUG("%s is still removed.", file_name);
        return true;
    }

    int ret = remove(file_name);
    if (ret)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("failed to remove %s. errno=%d (%s)", file_name, errno, error);
        return false;
    }

    return true;
}

bool rename_file(const char *file_name, const char *new_file_name)
{
    if (!file_name || !new_file_name)
    {
        if (!file_name)
            PG_LOG_DEBUG("parameter 'file_name' is null");
        if (!new_file_name)
            PG_LOG_DEBUG("parameter 'new_file_name' is null");
        return false;
    }

    if (!is_exist(file_name))
    {
        PG_LOG_DEBUG("%s is not exist.", file_name);
        return false;
    }

    if (is_exist(new_file_name))
    {
        PG_LOG_DEBUG("%s is still exist.", new_file_name);
        return false;
    }

    int ret = rename(file_name, new_file_name);
    if (ret)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("failed to rename file. errno=%d (%s)", errno, error);
        return false;
    }

    return true;
}

bool copy_file(const char *file_name, const char *target_file_name)
{
    if (!file_name || !target_file_name)
    {
        if (!file_name)
            PG_LOG_DEBUG("parameter 'file_name' is null");
        if (!target_file_name)
            PG_LOG_DEBUG("parameter 'target_file_name' is null");
        return false;
    }

    if (!is_exist_file(file_name))
    {
        PG_LOG_DEBUG("%s is not exist.", file_name);
        return false;
    }

    if (is_exist_file(target_file_name))
    {
        PG_LOG_DEBUG("%s is still exist.", target_file_name);
        return false;
    }

    FILE *src = fopen(file_name, "rb");
    if (!src)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("failed to open file %s. errno=%d (%s)",
                     file_name, errno, error);
        return false;
    }

    FILE *dst = fopen(target_file_name, "wb");
    if (!dst)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("failed to open file %s. errno=%d (%s)",
                     target_file_name, errno, error);
        fclose(src);
        return false;
    }

    char buffer[8192];
    size_t bytes;
    bool success = true;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        if (fwrite(buffer, 1, bytes, dst) != bytes)
        {
            char *error = strerror(errno);
            PG_LOG_DEBUG("failed to write file %s. errno=%d (%s)",
                         target_file_name, errno, error);
            success = false;
            break;
        }
    }

    fclose(src);
    fclose(dst);

    if (!success)
    {
        PG_LOG_DEBUG("remove partical file %s.", target_file_name);
        remove(target_file_name);
    }

    return success;
}

bool move_file(const char *file_name, const char *target_file_name)
{
    if (!file_name || !target_file_name)
    {
        if (!file_name)
            PG_LOG_DEBUG("parameter 'file_name' is null");
        if (!target_file_name)
            PG_LOG_DEBUG("parameter 'target_file_name' is null");
        return false;
    }

    if (!is_exist_file(file_name))
    {
        PG_LOG_DEBUG("%s is not exist.", file_name);
        return false;
    }

    if (is_exist_file(target_file_name))
    {
        PG_LOG_DEBUG("%s is still exist.", target_file_name);
        return false;
    }

    // 同一ファイルシステム内なら rename で一瞬で終わる
    if (rename_file(file_name, target_file_name))
    {
        return true;
    }

    // パーティションを跨ぐ移動などで rename が失敗した場合のバックアップ処理
    if (copy_file(file_name, target_file_name))
    {
        return remove_file(file_name);
    }

    return false;
}

bool create_directory(const char *directory_name)
{
    if (!directory_name)
    {
        PG_LOG_DEBUG("parameter 'directory_name' is null");
        return false;
    }
    if (is_exist_directory(directory_name))
    {
        PG_LOG_DEBUG("parameter %s is still exist", directory_name);
        return false;
    }

    // 0755: 所有者は全権限、グループ・その他は読み取りと実行権限
    int ret = mkdir(directory_name, 0755);
    if (ret)
    {
        char *error = strerror(errno);
        PG_LOG_DEBUG("failed to create directory. errno=%d (%s)", errno, error);
        return false;
    }

    return true;
}

bool copy_directory(const char *directory_name, const char *target_directory_name)
{
    // C言語でフォルダのコピーをやる場合、dirent.h を使って
    // ディレクトリ内のファイル／サブフォルダを再帰的に走査（opendir / readdir）する必要があります。
    // 現時点では未実装スケルトンとして false を返しておきます。
    return false;
}

bool move_directory(const char *directory_name, const char *target_directory_name)
{
    if (!directory_name || !target_directory_name)
    {
        if (!directory_name)
            PG_LOG_DEBUG("parameter 'directory_name' is null");
        if (!target_directory_name)
            PG_LOG_DEBUG("parameter 'target_directory_name' is null");
        return false;
    }

    if (!is_exist_file(directory_name))
    {
        PG_LOG_DEBUG("%s is not exist.", directory_name);
        return false;
    }

    if (is_exist_file(target_directory_name))
    {
        PG_LOG_DEBUG("%s is still exist.", target_directory_name);
        return false;
    }

    // 同一ファイルシステム間であれば rename でフォルダごと移動可能
    if (rename_file(directory_name, target_directory_name))
    {
        return true;
    }

    // 別ドライブ間の場合は copy_folder() -> remove_folder() の再帰処理が必要
    return false;
}
