#include <stddef.h>

#ifndef ARRAYLIST_ITER
#define ARRAYLIST_ITER 128
#endif

typedef struct arrayList{
    size_t len;
    char *_elem;
    
    size_t _cap;
    size_t _elem_size;
}arrayList;

arrayList arrayList_make(size_t elem_size);

void arrayList_free(arrayList *arr);

void *arrayList_get_ptr(arrayList *arr, size_t i);

void arrayList_get_cpy(arrayList *arr, size_t i, void *buf);

void arrayList_set(arrayList *arr, size_t i, void *elem);

void arrayList_append(arrayList *arr, void *elem);

void arrayList_pop(arrayList *arr, void *buf);
