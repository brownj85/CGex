#ifndef UTIL_H
#define UTIL_H

#include <wchar.h>

typedef struct uint_tuple{
    size_t a;
    size_t b;
}uint_tuple;

void memswap(void *a, void *b, size_t data_size);

void print_tup(struct uint_tuple tup);

void *check_malloc(size_t, char*);

void *check_realloc(void *ptr, size_t, char *);

size_t min(size_t, size_t);

size_t max(size_t, size_t);

// sort arr in ascending order
void sort_uint(size_t *arr, size_t len);

int find_char(wchar_t *str, wchar_t *chset, wchar_t esc);
#endif
