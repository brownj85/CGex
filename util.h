#ifndef UTIL_H

#define UTIL_H

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

#endif
