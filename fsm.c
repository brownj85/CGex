#include "fsm.h"
#include "array_list/array_list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

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

static size_t test_transition(fsmTransition t, wchar_t ch){

    if(t.rule < EQ){
        return t.exit_nd;
    }else if(t.rule == EQ){
        if(t.min_ch <= ch && t.max_ch >= ch)
            return t.exit_nd;
        else
            return 0;

    }else if(t.rule == N_EQ){
        if(t.min_ch > ch || t.max_ch < ch)
            return t.exit_nd;
        else
            return 0;
    }

    assert(0);
    return 0;
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
    size_t nd;
    size_t strt;
    size_t end;
};

void print_states(arrayList *active_states){
    struct state_tuple curr_st;
    for(int i = 0; i < active_states->len; i++){
        arrayList_get_cpy(active_states, i, &curr_st);
        printf("(%ld, %ld, %ld)\n", curr_st.nd, curr_st.strt, curr_st.end);
    }
    printf("--------------------\n");
}


int fsm_match(
        fsm *f, wchar_t *str,
        struct uint_tuple *match_tups,
        size_t max_matches){

    arrayList stack_a = arrayList_make(sizeof(struct state_tuple));
    arrayList *active_states = &stack_a;

    arrayList stack_b = arrayList_make(sizeof(struct state_tuple));
    arrayList *next_states = &stack_b;

    struct state_tuple curr_st = {2, 0, 0};
    size_t curr_matches = 0;

    arrayList_append(active_states, &curr_st);

    size_t i = 0;

    while(str[i] != '\0' && curr_matches < max_matches){
        print_states(active_states);
        while(active_states->len > 0){
            arrayList_pop(active_states, &curr_st);
            
            struct bucketHeader nd_hdr = 
                bucketList_get(&f->data, curr_st.nd);

            fsmTransition *transition = (fsmTransition *) nd_hdr.elem;

            for(int i = 0; i < nd_hdr.len; i++){

                if(transition->rule == ACCEPT){
                    match_tups[curr_matches] = 
                        (struct uint_tuple) {curr_st.strt, curr_st.end};
                    curr_matches++;
                }else{
                    size_t exit_nd = test_transition(*transition, str[i]);

                    if(exit_nd){                    
                        struct state_tuple next_state = 
                            {exit_nd, curr_st.strt, curr_st.end + 1};
                        
                        arrayList_append(next_states, &next_state);
                    }
                }

                transition++;
            }
        }

        struct state_tuple next = {2, i + 1, i + 1};

        arrayList_append(next_states, &next);
        i++;

        arrayList *temp = next_states;
        next_states = active_states;
        active_states = temp;
    }


    for(int i = 0; i < active_states->len; i++){
        if(curr_matches == max_matches)
            break;

        struct state_tuple temp;
        arrayList_pop(active_states, &temp);

        if(temp.nd == 1){
            match_tups[curr_matches] = 
                (struct uint_tuple) {temp.strt, temp.end};
            curr_matches++;
        }
    }

    arrayList_free(&stack_a);
    arrayList_free(&stack_b);

    return curr_matches;
}
