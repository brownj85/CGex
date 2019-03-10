#include "fsm.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//  == Local Structs

struct state_tuple{
    size_t nd;
    size_t strt;
    size_t end;
};

//  == Debugging Functions

void print_states(arrayList *active_states){
    struct state_tuple curr_st;
    for(int i = 0; i < active_states->len; i++){
        arrayList_get_cpy(active_states, i, &curr_st);
        printf("(%ld, %ld, %ld)\n", curr_st.nd, curr_st.strt, curr_st.end);
    }
    printf("--------------------\n");
}

void print_transition(fsmTransition t){
    printf("([%c-%c]: -> %ld)", (char) t.min_ch, (char) t.max_ch, t.exit_nd);
}

void print_fsm(fsm *f){
    arrayList *nd;
    for(nd = aL_first(&f->data); nd != aL_done(&f->data); nd = aL_next(&f->data, nd)){
        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            print_transition(*t);
        }
        printf("\n------\n");
    }
}

// == Utility Functions
static inline void insert_transition(fsm *f, size_t node_id,
        enum transitionRule rule, wchar_t min_ch, wchar_t max_ch,
        size_t exit_nd){
 
    assert(node_id < fsm_len(f));
    
    fsmTransition t = {rule, min_ch, max_ch, exit_nd};

    arrayList_append(
            arrayList_get_ptr(&f->data, node_id),
            &t);
}

static inline size_t add_node(fsm *f){
    arrayList nd = arrayList_make(sizeof(fsmTransition), true);
    
    return arrayList_append(&f->data, &nd) - 1;
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

//  ==

fsm *fsm_make(){
    fsm *f = check_malloc(sizeof(fsm), "malloc in fsm_make");

    f->data = arrayList_make(sizeof(arrayList), true);

    add_node(f);
    insert_transition(f, 0, FAIL, 0, 0, 0);
    add_node(f);
    insert_transition(f, 1, ACCEPT, 0, 0, 1);

    return f;   
}

void fsm_free(fsm *f){
    for(int i = 0; i < f->data.len; i++){
        arrayList_free(
                (arrayList *) arrayList_get_ptr(&f->data, i));
    }

    arrayList_free(&f->data);
    free(f);
}

size_t fsm_len(fsm *f){
    return f->data.len;
}

fsm *make_string_fsm(wchar_t *str){
    fsm *f = fsm_make();

    if(*str == '\0'){
        return NULL;
    }

    while(*(str + 1) != '\0'){
        size_t nd = add_node(f);
        insert_transition(f, nd, EQ, *str, *str, nd + 1);
        str++;
    }
    size_t nd = add_node(f);
    insert_transition(f, nd, EQ, *str, *str, 1);

    print_fsm(f);

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

static inline void get_next_states(
        fsm *f, wchar_t ch, arrayList *nd,
        struct state_tuple curr_st, arrayList *next_states){

    fsmTransition *t;
    for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
        size_t exit_nd = test_transition(*t, ch);

        if(exit_nd){
            struct state_tuple next_state = 
                {exit_nd, curr_st.strt, curr_st.end + 1};

            arrayList_append(next_states, &next_state);
        }
    }
}

arrayList fsm_match(
        fsm *f, wchar_t *str, size_t max_matches){

    arrayList matches = arrayList_make(sizeof(struct uint_tuple), true);

    arrayList stack_a = arrayList_make(sizeof(struct state_tuple), true);
    arrayList *active_states = &stack_a;

    arrayList stack_b = arrayList_make(sizeof(struct state_tuple), true);
    arrayList *next_states = &stack_b;

    struct state_tuple curr_st = {2, 0, 0};
    size_t i = 0;
    bool sent = true;
    arrayList_append(active_states, &curr_st);

    while(sent && matches.len < max_matches){
        if(str[i] == '\0')
            sent = false;

        printf("%c\n", (char) str[i]);
        print_states(active_states);
        assert(next_states->len == 0);
        
        while(active_states->len > 0){
            arrayList_pop(active_states, &curr_st);
            
            if(curr_st.nd == 1){
                struct uint_tuple match = {curr_st.strt, curr_st.end};
                arrayList_append(&matches, &match);
                
                if(matches.len == max_matches)
                    break;

            }else{
                arrayList nd;
                arrayList_get_cpy(&f->data, curr_st.nd, &nd);

                get_next_states(f, str[i], &nd, curr_st, next_states);
            }
        }

        struct state_tuple next = {2, i + 1, i + 1};
        arrayList_append(next_states, &next);
        
        i++;

        memswap(&next_states, &active_states, sizeof(arrayList *));
    }

    arrayList_free(&stack_a);
    arrayList_free(&stack_b);

    return matches;
}


