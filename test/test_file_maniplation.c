#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "pg_string.h"
#include "pg_file.h"
#include "check_functions.h"

int main(void)
{
    int rc = 0;
    bool ret;

    /* is_exist_file() */
    ret = is_exist_file(NULL);
    rc |= assert_false(ret, "is_exist_file(NULL)");

    ret = is_exist_file("/var/lib");
    rc |= assert_false(ret, "is_exist_file(/var/lib)");

    ret = is_exist_file("./test_file_maniplation");
    rc |= assert_true(ret, "is_exist_file(./sample_file_maniplation)");

    /* is_exist_directory() */
    ret = is_exist_directory(NULL);
    rc |= assert_false(ret, "is_exist_directory(NULL)");

    ret = is_exist_directory("/var/org");
    rc |= assert_false(ret, "is_exist_directory(/var/org)");

    ret = is_exist_directory("/var/lib");
    rc |= assert_true(ret, "is_exist_directory(/var/lib)");

    ret = is_exist_directory("./test_file_maniplation");
    rc |= assert_false(ret, "is_exist_directory(./test_file_maniplation)");

    /* create_directory() */
    ret = create_directory("./SampleFolder");
    rc |= assert_true(ret, "create_directory(./SampleFolder)");
    ret = is_exist_directory("./SampleFolder");
    rc |= assert_true(ret, "is_exist_directory(./SampleFolder)");

    /* rename_file() */
    ret = rename_file("./SampleFolder", "./SampleDirectory");
    rc |= assert_true(ret, "rename_directory(./SampleFolder, ./SampleDirectory)");
    ret = is_exist_directory("./SampleDirectory");
    rc |= assert_true(ret, "is_exist_directory(./SampleDirectory)");

    FILE *fp = fopen("./SampleFile01.txt", "w");
    if (!fp)
    {
        fprintf(stderr, "Failed to create file 'SampleFile01.txt");
        return FAILURE;
    }
    fputs("This is a sample text.", fp);
    fclose(fp);

    /* copy_file() */
    ret = copy_file("./SampleFile01.txt", "./SampleDirectory/Sample01.txt");
    rc |= assert_true(ret, "copy_file(./SampleFile01.txt, ./SampleDirectory/Sample01.txt)");

    /* move_file() */
    ret = move_file("./SampleFile01.txt", "./SampleDirectory/Sample02.txt");
    rc |= assert_true(ret, "move_file(./SampleFile01.txt, ./SampleDirectory/Sample02.txt)");

    /* remove_file() */
    ret = remove_file("./SampleFile01.txt");
    rc |= assert_true(ret, "remove_file(./SampleFile01.txt)");
    ret = is_exist_file("./SampleFile01.txt");
    rc |= assert_false(ret, "is_exist_file(./SampleFile01.txt)");

    /* get_file_entry_list() */
    PGStringList *file_list = get_file_entry_list("./SampleDirectory", true);
    rc |= assert_value(2, pg_string_list_size(file_list), "get_file_entry_list() size");

    PGString *file01 = pg_string_list_get(file_list, 0);
    rc |= assert_cmp("Sample01.txt", pg_string_get(file01), "get_file_entry_list(0)");

    PGString *file02 = pg_string_list_get(file_list, 1);
    rc |= assert_cmp("Sample02.txt", pg_string_get(file02), "get_file_entry_list(1)");

    pg_string_list_free(file_list);

    /* remove_directory() */
    ret = remove_directory("./SampleDirectory");
    rc |= assert_true(ret, "remove_directory(./SampleDirectory)");
    ret = is_exist_directory("./SampleDirectory");
    rc |= assert_false(ret, "is_exist_directory(./SampleDirectory)");

    /* get_real_path() */
    PGString *check_path01 = get_real_path("/proc");
    rc |= assert_cmp("/proc", pg_string_get(check_path01), "get_real_path(/proc)");
    pg_string_free(check_path01);

    PGString *check_path02 = get_real_path(".");
    PGString *expect02 = execute_command("pwd");
    rc |= assert_cmp(pg_string_get(expect02), pg_string_get(check_path02), "get_real_path(.)");
    pg_string_free(expect02);
    pg_string_free(check_path02);

    /* read_line() */
    fp = fopen("./read_file_test.txt", "w");
    if (!fp)
    {
        fprintf(stderr, "Failed to create file 'read_file_test.txt");
        return FAILURE;
    }

    const char *data = "---------1---------2---------3---------4---------5---------6\r\n";
    const char *expect = "---------1---------2---------3---------4---------5---------6";
    for (int i = 0; i < 10; i++)
    {
        fputs(data, fp);
    }

    fclose(fp);

    fp = fopen("./read_file_test.txt", "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to open file 'read_file_test.txt");
        return FAILURE;
    }

    PGString *line;
    while (line = read_line(fp, 16, false))
    {
        rc |= assert_cmp(expect, pg_string_get(line), "read_line()");
        pg_string_free(line);
    }

    fclose(fp);

    return rc;
}
