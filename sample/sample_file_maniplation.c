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

int main(int argc, char **argv)
{
    pg_log_set_level(PG_LEVEL_DEBUG);

    printf("================================\n");
    check_exist_file(NULL);
    check_exist_file("/var/lib");
    check_exist_file("./sample_file_maniplation");
    check_exist_file("./sample_file_maniplationD");

    printf("================================\n");
    check_exist_directory(NULL);
    check_exist_directory("/var/org");
    check_exist_directory("/var/lib");
    check_exist_directory("./sample_file_maniplation");
    check_exist_directory("./sample_file_maniplationD");

    printf("================================\n");
    bool ret;

    ret = create_directory("SampleFolder");
    if (!ret)
    {
        PG_LOG_ERROR("cannot create directory");
        return -1;
    }
    else
    {
        ret = is_exist_directory("SampleFolder");
        if (!ret)
        {
            PG_LOG_ERROR("SampleFolder is not exist.");
            return -1;
        }
    }
    PG_LOG_INFO("create directory 'SampleFolder'.");

    ret = rename_file("SampleFolder", "SampleDirectory");
    if (!ret)
    {
        PG_LOG_ERROR("cannot rename directory");
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
    }

    if (!copy_file("SampleFile01.txt", "SampleDirectory/TestFile01.txt"))
    {
        PG_LOG_ERROR("cannot copy_file.");
        return -1;
    }

    if (remove_file("SampleFile01.txt"))
    {
        PG_LOG_ERROR("cannot remove file %s.", "SampleFile01.txt");
    }

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
    }

    if (!move_file("SampleFile02.txt", "SampleDirectory/TestFile02.txt"))
    {
        PG_LOG_ERROR("cannot move_file.");
        return -1;
    }

    ret = remove_file("SampleDirectory");
    if (!ret)
    {
        PG_LOG_ERROR("cannot remove SampleDirectoy");
        return -1;
    }
    PG_LOG_INFO("remove directory 'SampleDirectory'.");

    PG_LOG_INFO("All test done.");
}
