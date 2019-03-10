#include "fsm.h"

#include <stdlib.h>

static inline void insert_transition(fsm *f, size_t node_id,
        enum transitionRule rule, wchar_t min_ch, wchar_t max_ch,
        size_t exit_nd){

    fsmTransition t = {rule, min_ch, max_ch, exit_nd};

    bucketList_insert(&f->data, node_id, &t);
}

fsmTransition fsmTransition_make(
        enum transitionRule rule, 
        wchar_t min_ch, wchar_t max_ch,
        size_t exit_nd){

    fsmTransition t;

    t.rule = rule;
    t.min_ch = min_ch;
    t.max_ch = max_ch;
    t.exit_nd = exit_nd;

    return t;
}

fsm *fsm_make(){
    fsm *f = check_malloc(sizeof(fsm), "malloc in fsm_make");

    f->data = bucketList_make(sizeof(fsmTransition), 8);
    f->num_nodes = 2;

    insert_transition(f, 0, FAIL, 0, 0, 0);
    insert_transition(f, 1, ACCEPT, 0, 0, 1);

    return f;   
}

void fsm_free(fsm *f){
    bucketList_free(&f->data);
    free(f);
}

fsm *make_string_fsm(wchar_t *str){
    fsm *f = fsm_make();

    wchar_t *curr = str;

    if(*str == '\0'){
        return NULL;
    }

    while(*(str + 1) != '\0'){
        insert_transition(f, f->num_nodes, EQ, *str, *str, f->num_nodes + 1);
        f->num_nodes++;

        str++;
    }

    insert_transition(f, f->num_nodes, EQ, *str, *str, 1);

    return f;
}

fsm *make_charset_fsm(struct uint_tuple *ranges, size_t num_ranges){
    return NULL;
}

fsm *fsm_union(fsm *a, fsm *b){
    return NULL;
}

fsm *fsm_concat(fsm *a, fsm *b){
    return NULL;
}

fsm *fsm_k_star(fsm *f){
    return NULL;
}


struct state_tuple{
    size_t node_id;
    size_t match_start;
    size_t match_end;
}


#define STATE_ITER 128

void fsm_match(
        fsm *f, wchar_t str,
        struct uint_tuple *match_tups,
        size_t max_matches){

    struct state_tuple *stack_a = checked_malloc(sizeof(state_tuple) * STATE_ITER);
    size_t a_top = 0;
    size_t a_cap = STATE_ITER;

    struct state_tuple *stack_b = checked_malloc(sizeof(state_tuple) * STATE_ITER);

}
