#ifndef UTIL_C

#define UTIL_C

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"

void memswap(void *a, void *b, size_t data_size){
    char temp[data_size];

    memcpy(temp, a, data_size);
    memcpy(a, b, data_size);
    memcpy(b, temp, data_size);
}


void *check_realloc(void *ptr, size_t bytes, char *context){
    void *ret = realloc(ptr, bytes);

    if(ret == NULL){
        perror(context);
        exit(errno);
    }

    return ret;
}


void *check_malloc(size_t bytes, char *context){
        void *ret = malloc(bytes);

            if(ret == NULL){
                        perror(context);
                                exit(errno);
            }

   return ret;
}

size_t min(size_t a, size_t b){
    if(a < b)
        return a;
    
    return b;
}

size_t max(size_t a, size_t b){
    if(a > b)
        return a;

    return b;
}

void print_tup(struct uint_tuple tup){
    printf("(%ld, %ld)\n", tup.a, tup.b);
}
#endif
