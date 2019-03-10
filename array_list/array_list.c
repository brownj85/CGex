#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../util.h"
#include "array_list.h"

arrayList arrayList_make(size_t elem_size){
    arrayList arr;

    arr._elem = calloc(ARRAYLIST_ITER * elem_size, 1);
    arr._cap = ARRAYLIST_ITER;
    arr._elem_size = elem_size;

    arr.len = 0;

    return arr;
}

void arrayList_free(arrayList *arr){
    free(arr->_elem);
}

void *arrayList_get_ptr(arrayList *arr, size_t i){
    return (void *) &arr->_elem[i * arr->_elem_size];
}

void arrayList_get_cpy(arrayList *arr, size_t i, void *buf){
    memcpy(buf, &arr->_elem[i * arr->_elem_size], arr->_elem_size);
}

void arrayList_set(arrayList *arr, size_t i, void *elem){
    if(i >= arr->len){
        fprintf(stderr, "index error in arrayList: %ld\n", i);
        exit(1);
    }

    memcpy(&arr->_elem[i * arr->_elem_size], elem, arr->_elem_size);
}

static void arrayList_resize(arrayList *arr){
  
    arr->_elem = realloc(arr->_elem, (arr->_cap + ARRAYLIST_ITER) * arr->_elem_size);

    memset(&arr->_elem[arr->_cap * arr->_elem_size], 0, ARRAYLIST_ITER * arr->_elem_size);

    arr->_cap += ARRAYLIST_ITER;
}

void arrayList_append(arrayList *arr, void *elem){
    arr->len++;

    if(arr->len == arr->_cap)
        arrayList_resize(arr);

    arrayList_set(arr, arr->len - 1, elem);
}

void arrayList_pop(arrayList *arr, void *buf){
    if(arr->len == 0){
        fprintf(stderr, "popping from empty list");
        exit(1);
    }
    arr->len--;
    arrayList_get_cpy(arr, arr->len, buf);
}   
