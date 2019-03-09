#ifndef UTIL_H

#define UTIL_H

void *check_malloc(size_t, char*);

void *check_realloc(void *ptr, size_t, char *);

size_t min(size_t, size_t);

size_t max(size_t, size_t);

#endif
