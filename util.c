#ifndef UTIL_C

#define UTIL_C

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "util.h"

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
#endif
