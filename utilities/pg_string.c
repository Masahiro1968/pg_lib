/**
 * @file    pg_string.c
 * @brief   文字列操作ユーティリティ
 * @author  Masahiro1968
 * @date    2026-08-01
 * @details
 *   - 文字列操作に関する構造体や関数を集約します。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "pg_string.h"

PGString *pg_string_new(size_t size)
{
    PGString *v = malloc(sizeof(PGString));
    if (!v)
        return NULL;

    const size_t reserve = size + 1; // ヌル終端分を確保

    v->data = malloc(reserve);
    if (!v->data)
    {
        free(v);
        return NULL;
    }

    v->data[0] = '\0';
    v->size = 0;
    v->capacity = reserve;

    return v;
}

int pg_string_reserve(PGString *string, size_t capacity)
{
    if (!string)
        return -1;

    if (capacity <= string->capacity)
        return 0;

    char *data = realloc(string->data, capacity);

    if (!data)
        return -1;

    string->data = data;
    string->capacity = capacity;

    return 0;
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

    if (pg_string_reserve(string, len + 1) != 0)
        return -1;

    memcpy(string->data, text, len + 1);

    string->size = len;

    return (int)len;
}

int pg_string_format(PGString *string, const char *format, ...)
{
    if (!string || !format)
        return -1;

    va_list ap;

    va_start(ap, format);
    int size = vsnprintf(NULL, 0, format, ap);
    va_end(ap);

    if (size < 0)
        return -1;

    if (pg_string_reserve(string, (size_t)size + 1) != 0)
        return -1;

    va_start(ap, format);
    int ret = vsnprintf(string->data, string->capacity, format, ap);
    va_end(ap);

    if (ret < 0)
        return -1;

    string->size = (size_t)ret;

    return ret;
}

const char *pg_string_get(const PGString *string)
{
    if (!string)
        return NULL;

    return string->data;
}

int pg_string_size(const PGString *string)
{
    if (!string)
        return -1;

    return string->size;
}

int pg_string_join(PGString *base, const PGString *append)
{
    if (!base || !append)
        return -1;

    size_t new_size = base->size + append->size;

    if (pg_string_reserve(base, new_size + 1) != 0)
        return -1;

    memcpy(
        base->data + base->size,
        append->data,
        append->size + 1);

    base->size = new_size;

    return (int)new_size;
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

void pg_string_trim_trailing_zeros(PGString *string)
{
    // 小数点が含まれている場合のみ処理（"1000" などの整数を削らないため）
    if (strchr(string->data, '.') != NULL)
    {
        // 末尾から '0' を削る
        while (string->size > 0 && string->data[string->size - 1] == '0')
        {
            string->size--;
        }

        // '0' を削った結果、末尾が '.' になったら '.' も削る ("1." -> "1")
        if (string->size > 0 && string->data[string->size - 1] == '.')
        {
            string->size--;
        }

        string->data[string->size] = '\0'; // 終端文字を打つ
    }
}

PGStringList *pg_string_split(const PGString *s, char delimiter)
{
    PGStringList *list = pg_string_list_new();
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

            list->items = realloc(list->items, sizeof(PGString *) * (list->count + 1));
            list->items[list->count++] = part;
            start = i + 1;
        }
    }

    return list;
}

PGStringList *pg_string_list_new()
{
    PGStringList *response = malloc(sizeof(PGStringList));
    response->count = 0;
    response->items = NULL;
    return response;
}

void pg_string_list_free(PGStringList *list)
{
    for (size_t i = 0; i < list->count; i++)
        pg_string_free(list->items[i]);

    free(list->items);
    list->items = NULL;
    list->count = 0;
    free(list);
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

bool pg_string_starts_with(const PGString *string, const char *prefix)
{
    if (!string || !prefix)
        return false;

    size_t prefix_len = strlen(prefix);

    if (string->size < prefix_len)
        return false;

    return memcmp(string->data, prefix, prefix_len) == 0;
}

bool pg_string_ends_with(const PGString *string, const char *suffix)
{
    if (!string || !suffix)
        return false;

    size_t suffix_len = strlen(suffix);

    if (string->size < suffix_len)
        return false;

    return memcmp(
               string->data + string->size - suffix_len,
               suffix,
               suffix_len) == 0;
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

    return string->size;
}

int pg_string_list_size(PGStringList *list)
{
    if (!list)
        return -1;

    return list->count;
}

PGString *pg_string_list_get(PGStringList *list, int count)
{
    if (!list || count > list->count)
        return NULL;

    return list->items[count];
}

int pg_string_list_add(PGStringList *list, PGString *string)
{
    if (!list || !string)
        return 0;

    PGString **items = realloc(
        list->items,
        sizeof(PGString *) * (list->count + 1));

    if (!items)
        return 0;

    list->items = items;
    list->items[list->count++] = string;

    return list->count;
}

void pg_string_to_upper(PGString *string)
{
    if (!string || !string->data)
        return;

    for (size_t i = 0; i < string->size; i++)
    {
        string->data[i] = (char)toupper((unsigned char)string->data[i]);
    }
}

void pg_string_to_lower(PGString *string)
{
    if (!string || !string->data)
        return;

    for (size_t i = 0; i < string->size; i++)
    {
        string->data[i] = (char)tolower((unsigned char)string->data[i]);
    }
}

PGString *pg_string_mid(const PGString *string, size_t start, size_t count)
{
    if (!string || !string->data || start >= string->size)
        return NULL;

    // 抽出可能な文字数の調整
    if (start + count > string->size)
    {
        count = string->size - start;
    }

    PGString *result = pg_string_new(count);
    if (!result)
        return NULL;

    memcpy(result->data, string->data + start, count);
    result->data[count] = '\0';
    result->size = count;

    return result;
}

PGString *pg_string_left(const PGString *string, size_t count)
{
    return pg_string_mid(string, 0, count);
}

PGString *pg_string_right(const PGString *string, size_t count)
{
    if (!string || !string->data)
        return NULL;

    if (count > string->size)
    {
        count = string->size;
    }

    size_t start = string->size - count;
    return pg_string_mid(string, start, count);
}

int pg_string_reverse_find(const PGString *string, const char *needle)
{
    if (!string || !string->data || !needle)
        return -1;

    size_t needle_len = strlen(needle);
    if (needle_len == 0 || needle_len > string->size)
        return -1;

    // 末尾側から順に一致チェック
    for (size_t i = string->size - needle_len + 1; i > 0; i--)
    {
        size_t idx = i - 1;
        if (memcmp(string->data + idx, needle, needle_len) == 0)
        {
            return (int)idx;
        }
    }

    return -1;
}

int pg_string_unquote(PGString *string, char quote_char)
{
    if (!string || string->size < 2)
        return 0; // 2文字未満なら囲まれている可能性はない

    // 先頭と末尾がともに quote_char の場合のみ外す
    if (string->data[0] == quote_char && string->data[string->size - 1] == quote_char)
    {
        // 先頭の1文字を詰める
        memmove(string->data, string->data + 1, string->size - 2);
        string->size -= 2;
        string->data[string->size] = '\0';
        return 1;
    }

    return 0;
}