#ifndef UTIL_C

#define UTIL_C

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
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

void sort_uint(size_t *arr, size_t len){
    for(size_t i = 1; i < len; i++){

        size_t j = i;
        while(j > 0  && arr[j-1] > arr[j]){
            size_t temp = arr[j];
            
            arr[j] = arr[j-1];
            arr[j-1] = temp;

            j--;
        }
    }
}


void print_tup(struct uint_tuple tup){
    printf("(%ld, %ld)\n", tup.a, tup.b);
}


int find_char(wchar_t *str, wchar_t *chset, wchar_t esc){

    int i = 0;
    while(str[i] != '\0'){
        
            if(str[i] == esc){
                i++;
                if(str[i] == '\0')
                    return -1;
            }else{
                for(int j = 0; chset[j] != '\0'; j++){
                    if(str[i] == chset[j])
                        return i;
                }
            }
 
        i++;
    }

    return -1;
}

#endif
