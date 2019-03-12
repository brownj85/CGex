#ifndef UTIL_H
#define UTIL_H

#include <wchar.h>
#include <stdio.h>
typedef struct uint_tuple{
    size_t a;
    size_t b;
}uint_tuple;


void print_tup(struct uint_tuple tup);


void memswap(void *a, void *b, size_t data_size);

void *check_malloc(size_t, char*);

void *check_realloc(void *ptr, size_t, char *);

size_t min(size_t, size_t);

size_t max(size_t, size_t);

//returns the first index of any of the characters
//contained in chset in str, that are not in esc
//all arguments must be null terminated wc strings
//
//returns -1 on failure
int srch_wcstr(wchar_t *str, wchar_t *chset, wchar_t *esc);

#define READLINE_STOP 10000
//prompts the user for input then reads from
//stdtin up to EOF or first newline
//returns a dynamically allocated string
//stripped of newlines
wchar_t *readline(wchar_t *prompt);

wchar_t *freadline(wchar_t *prompt, FILE *stream);

wchar_t *get_substring(wchar_t *str, uint_tuple idx);

#endif
