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



