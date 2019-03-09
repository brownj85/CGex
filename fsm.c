#include "fsm.h"

#include <stdlib.h>

static inline void insert_transition(fsm *f, size_t node_id,
        enum transitionRule rule, w_char_t min_ch, w_char_t max_ch,
        size_t exit_nd){

    fsmTransition t = {rule, min_ch, max_ch, exit_nd};

    bucketList_insert(&f->data, node_id, &t);
}

fsmTransition fsmTransition_make(
        enum transitionRule rule, 
        w_char_t min_ch, w_char_t max_ch,
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

fsm *make_string_fsm(w_char_t *str){
    return NULL;
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

void fsm_match(
        fsm *f, w_char_t str,
        struct uint_tuple *match_tups,
        size_t max_matches){
}
