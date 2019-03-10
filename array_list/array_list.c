#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../util.h"
#include "array_list.h"


//  == Utility Functions ==

static inline void *inc_by_bytes(void *ptr, size_t nbytes){
    return (void *) (((char *) ptr) + nbytes);
}

static void arrayList_resize(arrayList *arr){
  
    arr->_elem = realloc(arr->_elem, (arr->_cap + ARRAYLIST_ITER) * arr->_elem_size);

    memset(&arr->_elem[arr->_cap * arr->_elem_size], 0, ARRAYLIST_ITER * arr->_elem_size);

    arr->_cap += ARRAYLIST_ITER;
}

//  ==

//  == Iterator Functions ==

void *aL_first(arrayList *arr){
    return (void *) arr->_elem;
}

void *aL_next(arrayList *arr, void *iter){
    return inc_by_bytes(iter, arr->_elem_size);
}

void *aL_done(arrayList *arr){
    return inc_by_bytes(arr->_elem, arr->len * arr->_elem_size);
}

//  ==
 
arrayList arrayList_make(size_t elem_size, bool alloc){
    arrayList arr;

    if(alloc){
        arr._elem = calloc(ARRAYLIST_ITER * elem_size, 1);
        arr._cap = ARRAYLIST_ITER;
    }else{
        arr._elem = NULL;
        arr._cap = 0;
    }
    
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
        fprintf(stderr, "indexError:  %ld\n", i);
        exit(1);
    }

    memcpy(&arr->_elem[i * arr->_elem_size], elem, arr->_elem_size);
}


size_t arrayList_append(arrayList *arr, void *elem){
    arr->len = arr->len + 1;

    if(arr->len == arr->_cap)
        arrayList_resize(arr);

    arrayList_set(arr, arr->len - 1, elem);

    return arr->len;
}

size_t arrayList_pop(arrayList *arr, void *buf){
    if(arr->len == 0){
        fprintf(stderr, "indexError: pop from empty list\n");
        exit(1);
    }
    arr->len--;
    arrayList_get_cpy(arr, arr->len, buf);

    return arr->len;
}   
