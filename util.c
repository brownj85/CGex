#ifndef UTIL_C

#define UTIL_C

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"

void print_tup(struct uint_tuple tup){
    printf("(%ld, %ld)\n", tup.a, tup.b);
}

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

static inline char search_chset(wchar_t ch, wchar_t *chset){
    for(int i = 0; chset[i] != L'\0'; i++){
        if(ch == chset[i])
            return 1;
    }

    return 0;
}

int srch_wcstr(wchar_t *str, wchar_t *chset, wchar_t *esc){

    int i = 0;
    while(str[i] != '\0'){
        
        if(!search_chset(str[i], esc)){
            if(search_chset(str[i], chset))
                return i;
        }
        
        i++;
        
    }

    return -1;
}


wchar_t *freadline(wchar_t *prompt, FILE *stream){
    wchar_t *line = check_malloc(sizeof(wchar_t) * 1024, "malloc in freadline");
    size_t cap = 1024;

    printf("%ls\n", prompt);
    int i = 0;
    wchar_t ch = fgetwc(stream);
    while(ch != WEOF && ch != L'\n'){
        line[i] = ch;
        i++;

        if(i == cap){
            cap += 1024;
            line = check_realloc(line, sizeof(wchar_t) * cap, "realloc in freadline");
        }

        ch = fgetwc(stream);
    }

    line[i] = L'\0';
    i++;
    line = check_realloc(line, sizeof(wchar_t) * i, "realloc in freadline");
    return line;
}

wchar_t *readline(wchar_t *prompt){
    return freadline(prompt, stdin);
}


#endif
