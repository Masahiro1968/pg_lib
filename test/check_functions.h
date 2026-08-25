#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FAILURE -1
#define SUCCESS 0

int assert_true(bool cond, const char *msg)
{
    if (!cond)
    {
        fprintf(stderr, "FAIL: %s\n", msg);
        return FAILURE;
    }

    return SUCCESS;
}

int assert_cmp(const char *expect, const char *actual, const char *msg)
{
    if (strcmp(expect, actual))
    {
        fprintf(stderr, "expect:%s, actual:%s, msg:%s\n", expect, actual, msg);
        return FAILURE;
    }

    return SUCCESS;
}

int assert_value(const int expect, const int actual, const char *msg)
{
    if (expect != actual)
    {
        fprintf(stderr, "expect:%d, actual:%d, msg:%s\n", expect, actual, msg);
        return FAILURE;
    }

    return SUCCESS;
}
