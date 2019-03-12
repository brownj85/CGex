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

static inline int is_metachar(wchar_t ch){
    for(int i = 1; i < NUM_MCH; i++){
        if(ch == metaChars[i]){
            return ch;
        }
    }

    return 0;
}

static inline void check_implemented(int mchar_idx){
    if(!metaChar_impl[mchar_idx]){
        err_not_impl(mchar_idx);
    }
}

static fsm *build_set_fsm(wchar_t *sub, size_t *end){
    fsm *f = fsm_make();

    size_t i = 0;
    bool escaped = false;

    size_t id = fsm_add_node(f);
    assert(id == 2);

    bool sent = true;
    while(sent){
        if(sub[i] == '\0'){
            err_expr_invalid(sub, 0, i);
        }

        wchar_t mch = 0;
        if(!escaped)
            mch = is_metachar(sub[i]);

        if(mch == 0){
            fsmTransition t = {EQ, sub[i], sub[i], 1};
            fsm_insert_transition(f, 2, &t);
        }else if(mch == '\\' ){
            escaped = true;
        }else if(mch == '^'){
            err_not_impl(mch);
        }else if(mch == '$'){
            err_not_impl(mch); 
        }else if(mch == '.'){
            fsmTransition t = {N_EQ, '\n', '\n', 1};
        }else if(mch == ']'){
            sent = false;
            *end = i + 1;
        }else{
            err_expr_invalid(sub, 0, i);
        }
    }

    return f;
}

static fsm *build_string_fsm(wchar_t *sub, size_t *end){
    fsm *f = fsm_make();
    
    size_t i = 0;
    bool escaped = false;

    fsmTransition prev;
    bool prev_valid = false;

    bool sent = true;
    while(sent){
        fsmTransition curr;
        bool curr_valid = false;

        wchar_t mch = 0;
        if(!escaped)
            mch = is_metachar(sub[i]);

        if(sub[i] == '\0'){
            if(escaped)
                err_expr_invalid(sub, 0, i);
            sent = false;
        }

        else if(mch){
            if(mch == metaChars[ESC]){
                escaped = true;
            }else if(mch == '+'){
                err_not_impl(mch);
            }else if(mch == '?'){
                err_not_impl(mch); 
            }else if(mch == '.'){
                curr  = (fsmTransition) {N_EQ, '\n', '\n', 0};
                curr_valid = true;
            }else if(mch == ')' || mch == ']' || mch == '}'){
                err_expr_invalid(sub, 0, i);
            }else{
                *end = i;
                sent = false;  
            }

        }else{
            curr = (fsmTransition) {EQ, sub[i], sub[i], 0};
            curr_valid = true;
        }

        if(prev_valid){
            size_t id = fsm_add_node(f);
            
            if(!sent)
                prev.exit_nd = 1;
            else
                prev.exit_nd = id + 1;
            
            fsm_insert_transition(f, id, &prev);

        }

        if(curr_valid){
            prev_valid = true;
            prev = curr;
        }else{
            prev_valid = false;
        }
 
        i++;
        
    }

    return f;
}


static inline fsm *grouping_search_fsm(wchar_t l_delim, wchar_t r_delim){
    fsm *f = fsm_make();

    size_t start = fsm_add_node(f);
    size_t start_esc = fsm_add_node(f);
    
    size_t inter = fsm_add_node(f);
    size_t inter_esc = fsm_add_node(f);
    
    fsmTransition l_b = {EQ, l_delim, l_delim, inter};
    fsm_insert_transition(f, start, &l_b);

    fsmTransition start_esc_in = {EQ, '\\', '\\', start_esc};
    fsm_insert_transition(f, start, &start_esc_in);

    fsmTransition start_esc_out = {EQ, WCHAR_MIN, WCHAR_MAX, start};
    fsm_insert_transition(f, start_esc, &start_esc_out);
       
    fsmTransition inter_esc_in = {EQ, '\\', '\\', inter_esc};
    fsm_insert_transition(f, inter, &inter_esc_in);

    fsmTransition inter_esc_out = {EQ, WCHAR_MIN, WCHAR_MAX, inter};
    fsm_insert_transition(f, inter_esc, &inter_esc_out);

    size_t breaks[] = {metaChars[ESC], l_delim, r_delim};
    sort_uint(breaks, 3);

    fsmTransition lower = {EQ, WCHAR_MIN, breaks[0] - 1, inter};
    fsm_insert_transition(f, inter, &lower);

    for(int i = 0; i < 2; i++){
        fsmTransition t = {EQ, breaks[i] + 1, breaks[i + 1] - 1, inter};
        fsm_insert_transition(f, inter, &t);
    }

    fsmTransition upper = {EQ, breaks[2] + 1, WCHAR_MAX, inter};
    fsm_insert_transition(f, inter, &upper);

    fsmTransition succ = {EQ, r_delim, r_delim, 1};
    fsm_insert_transition(f, inter, &succ);

    return f;
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

fsm *parse_regex(wchar_t *pattern){
    
    arrayList sets = find_groupings(pattern, '[', ']');  
   
    arrayList special = find_special_char(pattern, '*');
    
    
    struct uint_tuple *idx;
    for(idx = aL_first(&sets); idx != aL_done(&sets); idx = aL_next(&sets, idx)){
        print_tup(*idx);
    }

    int *ip;
    for(ip = aL_first(&special); ip != aL_done(&special); ip = aL_next(&special, ip)){
        printf("%d\n", *ip);
    }

 
    fsm *f = NULL;
    size_t i;
    fsm *union_first = NULL;
    fsm *prev = NULL;

    while(false){
        if(pattern[i] == '*'){
            if(prev == NULL)
                err_expr_invalid(pattern, i, i + 2);
            
            fsm *g = fsm_k_star(prev);
            fsm_free(prev);
            prev = g; 
            i++;
        }else if(pattern[i] == '|'){
            if(prev == NULL || union_first != NULL)
                //err_invalid_expr(pattern, i, i + 2);

            union_first = prev;
            prev = NULL;
            i++;
        }else{
            fsm *g = NULL;
            if(union_first != NULL && prev != NULL){
                g = fsm_union(union_first, prev);
                fsm_free(union_first);
                fsm_free(prev);

                union_first = NULL;
                prev = g;

            }else if(prev != NULL){
                if(f == NULL){
                    f = prev;
                }else{
                    fsm *g = fsm_concat(f, prev);
                    fsm_free(f);
                    fsm_free(prev);

                    prev = NULL;
                    f = g;
                }
            }else{
                prev = NULL;
            }
        }
    }

    return prev;
}



