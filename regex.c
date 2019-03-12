#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>

#include "regex.h"

static inline void err_not_impl(int mchar_idx){
    fprintf(stderr, "Matching for %s is not yet implemented\n", metaChar_repr[mchar_idx]);
    exit(1);
}

static inline void err_expr_invalid(wchar_t *pattern, size_t start, size_t end){
    fwprintf(stderr, L"Invalid expression: %.*s\n", end - start, pattern + start);
    exit(1);
}

arrayList find_sets(wchar_t *pattern){
    arrayList acc = arrayList_make(sizeof(struct uint_tuple), true);

    int i = 0;

    struct uint_tuple curr;
    wchar_t chset_l[] = {metaChars[LSET], '\0'};
    wchar_t chset_r[] = {metaChars[RSET], '\0'};

    bool found_l = false;
    int l_idx = -1;
    int r_idx = -1;
    while(i < wcslen(pattern)){
        if(!found_l){
            l_idx = find_char(&pattern[i], chset_l, metaChars[ESC]) + i;
            if(l_idx == -1 + i){
                break;
            }
            found_l = true;
            i = l_idx + 1;
        }else{
            r_idx = find_char(&pattern[i], chset_r, metaChars[ESC]) + i;
            if(r_idx == -1 + i){
                err_expr_invalid(pattern, 0, wcslen(pattern));
            }

            struct uint_tuple tup = {l_idx, r_idx};
            arrayList_append(&acc, &tup);
            found_l = false;
            i = r_idx + 1;
        }
    }

    if(found_l)
        err_expr_invalid(pattern, 0, wcslen(pattern));

    return acc;
}
                        

int find_groupings_rec(wchar_t *pattern, 
        wchar_t ldelim, wchar_t rdelim,
        int min, arrayList *acc){

    wchar_t chset_l[] = {ldelim, '\0'};
    int l_idx = find_char(&pattern[min], chset_l, metaChars[ESC]) + min;
    
    int r_min = l_idx + 1;
    if(pattern[r_min] == '\0' || l_idx == -1 + min)
        return -1;

    wchar_t chset_lr[] = {ldelim, rdelim, '\0'};

    while(pattern[r_min] != '\0'){
        int i = find_char(&pattern[r_min], chset_lr, metaChars[ESC]) + r_min;
        
        if(i == -1 + r_min)
            err_expr_invalid(pattern, l_idx, wcslen(pattern));
        else if(pattern[i] == ldelim){
            r_min = find_groupings_rec(pattern, ldelim, rdelim, i, acc);
            if(r_min == -1){
                return -1;
            }
        }else if(pattern[i] == rdelim){
            struct uint_tuple sub = {l_idx, i + 1};
            arrayList_append(acc, &sub);
            return i + 1;
        }else{
            assert(false);
        }
    }

    return -1;
}

arrayList find_groupings(wchar_t *pattern, wchar_t ldelim, wchar_t rdelim){
    arrayList acc = arrayList_make(sizeof(struct uint_tuple), true);

    int i = 0;
    while(i < wcslen(pattern)){
        i = find_groupings_rec(pattern, ldelim, rdelim, i, &acc);
        
        if(i == -1)
            break;
    }

    return acc;
}

arrayList find_special_char(wchar_t *pattern, wchar_t ch){
    arrayList acc = arrayList_make(sizeof(int), true);
    int i = 0;
    bool escaped = false;
    while(pattern[i] != '\0'){
        if(pattern[i] == metaChars[ESC] && !escaped){
            escaped = true;
        }else if(!escaped){
            if(pattern[i] == ch){
                arrayList_append(&acc, &i);
            }
        }else{
            escaped = false;
        }
        i++; 
    }

    return acc;
}

int parse_set(wchar_t *str, fsm **res_ptr){
    fsm *f = fsm_make();

    size_t id = fsm_add_node(f);

    bool escaped = false;
    int i = 0;
    while(str[i] != '\0'){
        if(!escaped && str[i] == metaChars[RSET]){
            *res_ptr = f;
            return i + 2;
        }else if(!escaped && str[i] == metaChars[ESC]){
            escaped = true;
        }else{
            escaped = false;
            
            fsmTransition t = {EQ, str[i], str[i], 1};
            fsm_insert_transition(f, 2, &t);
        }
        i++;
    }

    *res_ptr = NULL;
    return 0;
}



int parse_group(wchar_t *grp_start, fsm **res_ptr){
    int i = 0;

    wchar_t *str = grp_start;
    
    fsm *acc = NULL;
    fsm *alt = NULL;
    
    bool escaped = false;
    while(str[i] != '\0'){
        fsm *curr = NULL;
        if(str[i] == metaChars[ESC] && !escaped){
            escaped = true;
            i++;
        }else if(str[i] == metaChars[LSET] && !escaped){
            i += parse_set(&str[i + 1], &curr);
            assert(curr != NULL);
        }else if(str[i] == metaChars[LGRP] && !escaped){
            i += parse_group(&str[i + 1], &curr);
            assert(curr != NULL);
        }else if(str[i] == metaChars[RGRP] && !escaped){
            *res_ptr = acc;
            return i + 2;
        }else{
            curr = fsm_make();
            fsmTransition t = {EQ, str[i], str[i], 1};

            fsm_add_node(curr);
            fsm_insert_transition(curr, 2, &t);
            i++;
        }

        if(curr != NULL){
            escaped = false;
            if(str[i] == metaChars[KSTAR]){
                fsm *g = fsm_k_star(curr);
                
                memswap(&g, &curr, sizeof(fsm *));
                fsm_free(g);
                i++;
            }

            if(alt != NULL){
                fsm *g = fsm_union(alt, curr);

                memswap(&g, &curr, sizeof(fsm *));
                fsm_free(g);
                fsm_free(alt);

                alt = NULL;
            }

            if(str[i] == metaChars[ALT]){
                assert(alt == NULL);
                alt = curr;
                i++;
            }else if(acc == NULL){
                acc = curr;
            }else{
                fsm *g = fsm_concat(acc, curr);
                
                memswap(&g, &acc, sizeof(fsm *));
                fsm_free(g);
                fsm_free(curr);

            }   
        }
    }

    assert(alt == NULL);
    *res_ptr = acc;

    return i;
}

fsm *parse_regex(wchar_t *pattern){

    fsm *f = NULL;
    
    parse_group(pattern, &f);

    return f;
}



