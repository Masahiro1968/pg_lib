/**
 * @file    pg_file.c
 * @brief   ファイル出力ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - ファイルの操作に関する構造体や関数を集約します。
 */

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdlib.h>
#include "pg_file.h"

static bool is_exist(const char *file_name)
{
    struct stat st;
    int response = stat(file_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        return false;
    }
    return true;
}

bool is_exist_file(const char *file_name)
{
    if (!file_name)
    {
        return false;
    }

    struct stat st;
    int response = stat(file_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        return false;
    }
    else
    {
        int res = S_ISREG(st.st_mode);
        if (!res)
        {
            return false;
        }
    }

    return true;
}

bool is_exist_directory(const char *directory_name)
{
    if (!directory_name)
    {
        return false;
    }

    struct stat st;
    int response = stat(directory_name, &st);
    if (response != 0)
    {
        char *error = strerror(errno);
        return false;
    }
    else
    {
        int res = S_ISDIR(st.st_mode);
        if (!res)
        {
            return false;
        }
    }

    return true;
}

bool remove_file(const char *file_name)
{
    if (!file_name)
    {
        return false;
    }

    if (!is_exist_file(file_name))
    {
        return true;
    }

    int ret = remove(file_name);
    if (ret)
    {
        char *error = strerror(errno);
        return false;
    }

    return true;
}

bool remove_directory(const char *directory_name)
{
    if (!directory_name)
    {
        return false;
    }

    if (!is_exist_directory(directory_name))
    {
        return true;
    }

    PGStringList *files = get_file_entry_list(directory_name, true);
    for (int i = 0; i < pg_string_list_size(files); i++)
    {
        PGString *file_name = pg_string_list_get(files, i);
        PGString *file_path = pg_string_new(PATH_MAX);
        pg_string_format(file_path, "%s/%s", directory_name, pg_string_get(file_name));
        int ret = remove_file(pg_string_get(file_path));
        if (ret == false)
        {
            char *error = strerror(errno);
            pg_string_free(file_path);
            pg_string_list_free(files);
            return false;
        }
        pg_string_free(file_path);
    }
    pg_string_list_free(files);

    int ret = remove(directory_name);
    if (ret)
    {
        char *error = strerror(errno);
        return false;
    }

    return true;
}

bool rename_file(const char *file_name, const char *new_file_name)
{
    if (!file_name || !new_file_name)
        return false;

    if (!is_exist(file_name))
        return false;

    if (is_exist(new_file_name))
        return false;

    int ret = rename(file_name, new_file_name);
    if (ret)
    {
        char *error = strerror(errno);
        return false;
    }

    return true;
}

bool copy_file(const char *file_name, const char *target_file_name)
{
    if (!file_name || !target_file_name)
        return false;

    if (!is_exist_file(file_name))
        return false;

    if (is_exist_file(target_file_name))
        return false;

    FILE *src = fopen(file_name, "rb");
    if (!src)
    {
        char *error = strerror(errno);
        return false;
    }

    FILE *dst = fopen(target_file_name, "wb");
    if (!dst)
    {
        char *error = strerror(errno);
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
            success = false;
            break;
        }
    }

    fclose(src);
    fclose(dst);

    if (!success)
        remove(target_file_name);

    return success;
}

bool move_file(const char *file_name, const char *target_file_name)
{
    if (!file_name || !target_file_name)
        return false;

    if (!is_exist_file(file_name))
        return false;

    if (is_exist_file(target_file_name))
        return false;

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
        return false;

    if (is_exist_directory(directory_name))
        return false;

    // 0755: 所有者は全権限、グループ・その他は読み取りと実行権限
    int ret = mkdir(directory_name, 0755);
    if (ret)
    {
        char *error = strerror(errno);
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
        return false;

    if (!is_exist_directory(directory_name))
        return false;

    if (is_exist_directory(target_directory_name))
        return false;

    // 同一ファイルシステム間であれば rename でフォルダごと移動可能
    if (rename_file(directory_name, target_directory_name))
    {
        return true;
    }

    // 別ドライブ間の場合は copy_folder() -> remove_folder() の再帰処理が必要
    return false;
}

PGStringList *get_file_entry_list(const char *path, bool is_file)
{
    if (!path)
        return NULL;

    DIR *dir = opendir(path);
    if (!dir)
        return NULL;

    PGStringList *list = pg_string_list_new();
    if (!list)
    {
        closedir(dir);
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_REG && !is_file)
        {
            continue;
        }
        else if (entry->d_type == DT_DIR && is_file)
        {
            continue;
        }

        PGString *name = pg_string_new(0);
        if (!name)
            goto error;

        if (pg_string_set(name, entry->d_name) < 0)
        {
            pg_string_free(name);
            goto error;
        }

        if (!pg_string_list_add(list, name))
        {
            pg_string_free(name);
            goto error;
        }
    }

    closedir(dir);
    return list;

error:
    closedir(dir);
    pg_string_list_free(list);
    return NULL;
}

PGString *get_real_path(const char *path)
{
    if (!path)
        return NULL;

    PGString *response = pg_string_new(PATH_MAX);

    if (path[0] == '/')
    {
        // 既に絶対パス
        pg_string_set(response, path);
    }
    else
    {
        char *real_path = realpath(path, NULL);
        if (!real_path)
        {
            pg_string_free(response);
            response = NULL;
        }
        else
        {
            pg_string_set(response, real_path);
            free(real_path);
        }
    }

    return response;
}

PGString *execute_command(const char *command)
{
    FILE *fp = popen(command, "r");
    if (!fp)
        return NULL;

    char *buffer = calloc(512, sizeof(char));
    fgets(buffer, 512, fp);
    pclose(fp);

    buffer[strcspn(buffer, "\n")] = '\0';

    PGString *response = pg_string_new(512);
    pg_string_set(response, buffer);
    free(buffer);
    
    return response;
}