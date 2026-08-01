/**
 * @file    pg_string.c
 * @brief   文字列操作ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - 文字列操作に関する構造体や関数を集約します。
 */

#include <stdlib.h>
#include <string.h>
#include "pg_string.h"

PGString *pg_string_new(size_t size)
{
    PGString *v = malloc(sizeof(PGString));
    if (!v)
        return NULL;

    v->data = malloc(size + 1); // ヌル終端分を確保
    if (!v->data)
    {
        free(v);
        return NULL;
    }

    v->data[0] = '\0';
    v->size = 0;

    return v;
}

void pg_string_free(PGString *string)
{
    if (!string)
        return;

    free(string->data);
    free(string);
}

int pg_string_set(PGString *string, const char *text)
{
    if (!string || !text)
        return -1;

    size_t len = strlen(text);

    char *newdata = malloc(len + 1);
    if (!newdata)
        return -1;

    memcpy(newdata, text, len);
    newdata[len] = '\0';

    free(string->data);
    string->data = newdata;
    string->size = len;

    return len;
}

const char *pg_string_get(PGString *string)
{
    if (!string)
        return NULL;

    return string->data;
}

int pg_string_size(PGString *string)
{
    if (!string)
        return -1;

    return string->size;
}

int pg_string_join(PGString *base, PGString *append)
{
    size_t new_size = base->size + append->size;

    char *newdata = malloc(new_size + 1);
    if (!newdata)
        return -1;

    memcpy(newdata, base->data, base->size);
    memcpy(newdata + base->size, append->data, append->size);
    newdata[new_size] = '\0';

    free(base->data);
    base->data = newdata;
    base->size = new_size;

    return new_size;
}

void pg_string_trim_left(PGString *string)
{
    if (!string || string->size == 0)
        return;

    size_t i = 0;
    while (i < string->size && (string->data[i] == ' ' || string->data[i] == '\t'))
        i++;

    if (i == 0)
        return; // 何も削る必要なし

    size_t new_size = string->size - i;

    memmove(string->data, string->data + i, new_size);
    string->data[new_size] = '\0';
    string->size = new_size;
}

void pg_string_trim_right(PGString *string)
{
    if (!string || string->size == 0)
        return;

    size_t i = string->size;

    while (i > 0 && (string->data[i - 1] == ' ' || string->data[i - 1] == '\t'))
        i--;

    string->data[i] = '\0';
    string->size = i;
}

void pg_string_trim(PGString *string)
{
    pg_string_trim_right(string);
    pg_string_trim_left(string);
}

PGStringList pg_string_split(PGString *s, char delimiter)
{
    PGStringList list = {0};
    size_t start = 0;

    // UNIX パスの先頭 "/" を無視する
    if (s->data[0] == delimiter)
        start = 1;

    for (size_t i = start; i <= s->size; i++)
    {
        if (s->data[i] == delimiter || s->data[i] == '\0')
        {
            size_t len = i - start;

            PGString *part = pg_string_new(len + 1);
            memcpy(part->data, s->data + start, len);
            part->data[len] = '\0';
            part->size = len;

            list.items = realloc(list.items, sizeof(PGString *) * (list.count + 1));
            list.items[list.count++] = part;
            start = i + 1;
        }
    }

    return list;
}

void pg_string_list_free(PGStringList *list)
{
    for (size_t i = 0; i < list->count; i++)
        pg_string_free(list->items[i]);

    free(list->items);
    list->items = NULL;
    list->count = 0;
}

int pg_string_find(PGString *string, const char *needle)
{
    if (!string || !needle)
        return -1;

    char *p = strstr(string->data, needle);
    if (!p)
        return -1;

    return (int)(p - string->data);
}

int pg_string_replace(PGString *string, const char *from, const char *to)
{
    int pos = pg_string_find(string, from);
    if (pos < 0)
        return -1;

    size_t from_len = strlen(from);
    size_t to_len = strlen(to);

    size_t new_size = string->size - from_len + to_len;

    char *newdata = malloc(new_size + 1);
    if (!newdata)
        return -1;

    // 前半
    memcpy(newdata, string->data, pos);

    // 置換部分
    memcpy(newdata + pos, to, to_len);

    // 後半
    memcpy(newdata + pos + to_len, string->data + pos + from_len, string->size - pos - from_len);

    newdata[new_size] = '\0';

    free(string->data);
    string->data = newdata;
    string->size = new_size;

    return pos;
}
