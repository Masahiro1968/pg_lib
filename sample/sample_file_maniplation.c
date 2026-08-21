/**
 * @file    sample_file_maniplation.c
 * @brief   ファイルユーティリティのサンプル
 * @author  Masahiro1968
 * @date    2026-08-01
 */

#include "pg_file.h"

void check_exist_file(const char *file_name)
{
    if (is_exist_file(file_name))
    {
        PG_LOG_INFO("%s is exist.", file_name);
    }
    else
    {
        PG_LOG_ERROR("%s is not exist.", file_name);
    }
}

void check_exist_directory(const char *directory_name)
{
    if (is_exist_directory(directory_name))
    {
        PG_LOG_INFO("%s is exist.", directory_name);
    }
    else
    {
        PG_LOG_ERROR("%s is not exist.", directory_name);
    }
}

void get_directory_list(const char *path)
{
    PG_LOG_INFO("path:%s", path);

    PGStringList *file_list = get_file_entry_list(path, true);
    if (!file_list)
    {
        PG_LOG_DEBUG("path has no file: %s", path);
    }
    else
    {
        for (int i = 0; i < pg_string_list_size(file_list); i++)
        {
            PG_LOG_INFO("  File:%s",
                        pg_string_get(pg_string_list_get(file_list, i)));
        }
        pg_string_list_free(file_list);
    }

    PGStringList *directory_list = get_file_entry_list(path, false);
    if (!directory_list)
    {
        PG_LOG_DEBUG("path cannot access: %s", path);
        return;
    }
    else
    {
        for (int i = 0; i < pg_string_list_size(directory_list); i++)
        {
            PG_LOG_INFO("  Directory:%s",
                        pg_string_get(pg_string_list_get(directory_list, i)));

            PGString *mid_path = pg_string_new(256);
            pg_string_format(mid_path,
                             "%s/%s", path,
                             pg_string_get(pg_string_list_get(directory_list, i)));

            get_directory_list(pg_string_get(mid_path));

            pg_string_free(mid_path);
        }
        pg_string_list_free(directory_list);
    }
}

void check_real_path(const char *path)
{
    PGString *check_path = get_real_path(path);
    if (!check_path)
    {
        PG_LOG_INFO("cannot convert real path:%s", path);
        return;
    }

    PG_LOG_INFO("path:%s -> real path:%s",
                path, pg_string_get(check_path));
    pg_string_free(check_path);
}

int main(int argc, char **argv)
{
    pg_log_set_level(PG_LEVEL_DEBUG);

    printf("=check_file=====================\n");
    check_exist_file(NULL);
    check_exist_file("/var/lib");
    check_exist_file("./sample_file_maniplation");
    check_exist_file("./sample_file_maniplationD");

    printf("=check_directory================\n");
    check_exist_directory(NULL);
    check_exist_directory("/var/org");
    check_exist_directory("/var/lib");
    check_exist_directory("./sample_file_maniplation");
    check_exist_directory("./sample_file_maniplationD");

    printf("=create_directory===============\n");
    bool ret;

    ret = create_directory("SampleFolder");
    if (!ret)
    {
        PG_LOG_ERROR("cannot create directory 'SampleFolder'");
        return -1;
    }
    else
    {
        ret = is_exist_directory("SampleFolder");
        if (!ret)
        {
            PG_LOG_ERROR("'SampleFolder' is not exist.");
            return -1;
        }
    }
    PG_LOG_INFO("create directory 'SampleFolder'.");

    printf("=rename_directory===============\n");
    ret = rename_file("SampleFolder", "SampleDirectory");
    if (!ret)
    {
        PG_LOG_ERROR("cannot rename directory 'SampleFolder'");
        return -1;
    }
    else
    {
        ret = is_exist_directory("SampleDirectory");
        if (!ret)
        {
            PG_LOG_ERROR("SampleDirectory is not exist.");
            return -1;
        }
    }
    PG_LOG_INFO("rename directory 'SampleDirectory'.");

    printf("=create_file====================\n");
    FILE *fp;
    fp = fopen("./SampleFile01.txt", "w");
    if (fp == NULL)
    {
        PG_LOG_ERROR("cannot fopen SampleFile01.txt");
        return -1;
    }
    fclose(fp);

    if (!is_exist_file("./SampleFile01.txt"))
    {
        PG_LOG_ERROR("cannot create SampleFile01.txt");
        return -1;
    }

    if (!copy_file("SampleFile01.txt", "SampleDirectory/TestFile01.txt"))
    {
        PG_LOG_ERROR("cannot copy file 'SampleFile01.txt'");
        return -1;
    }

    if (!remove_file("SampleFile01.txt"))
    {
        PG_LOG_ERROR("cannot remove file 'SampleFile01.txt'");
        return -1;
    }

    printf("=create_file====================\n");
    fp = fopen("./SampleFile02.txt", "w");
    if (fp == NULL)
    {
        PG_LOG_ERROR("cannot fopen SampleFile02.txt");
        return -1;
    }
    fclose(fp);

    if (!is_exist_file("./SampleFile02.txt"))
    {
        PG_LOG_ERROR("cannot create SampleFile02.txt");
        return -1;
    }

    if (!move_file("SampleFile02.txt", "SampleDirectory/TestFile02.txt"))
    {
        PG_LOG_ERROR("cannot move_file.");
        return -1;
    }

    printf("=remove_directory===============\n");
    ret = remove_directory("SampleDirectory");
    if (!ret)
    {
        PG_LOG_ERROR("cannot remove SampleDirectory");
        return -1;
    }
    PG_LOG_INFO("remove directory 'SampleDirectory'.");

    printf("=get_directory_list=============\n");
    // get_directory_list("/home/ec2-user/GitHub/pg_lib");

    printf("=check_real_path================\n");
    // check_real_path("/proc");
    // check_real_path(".");
    // check_real_path("./build/");

    PG_LOG_INFO("All test done.");
}
