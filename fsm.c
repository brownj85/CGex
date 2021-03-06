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
static inline void insert_transition(fsm *f, size_t node_id, fsmTransition *t){
 
    assert(node_id < fsm_len(f));

    arrayList_append(
            arrayList_get_ptr(&f->data, node_id),
            t);
}

static inline size_t add_node(fsm *f){
    arrayList nd = arrayList_make(sizeof(fsmTransition), true);
    
    return arrayList_append(&f->data, &nd) - 1;
}


static inline size_t test_transition(fsmTransition t, wchar_t ch){

    if(t.rule < EQ){
        return t.exit_nd;
    }else if(t.rule == EQ){
        if(t.min_ch <= ch && t.max_ch >= ch)
            return t.exit_nd;
        else
            return 0;

    }else if(t.rule == N_EQ){
        if(t.min_ch > ch || t.max_ch < ch){
            return t.exit_nd;
        }else{
            return 0;
        }
    }

    return 0;
}


static void get_next_states(
        fsm *f, wchar_t ch,
        struct state_tuple curr_st, arrayList *next_states){

    arrayList nd;
    arrayList_get_cpy(&f->data, curr_st.nd, &nd);
    
    fsmTransition *t;
    for(t = aL_first(&nd); t != aL_done(&nd); t = aL_next(&nd, t)){

        if(t->rule == NIL){
            struct state_tuple next_state = {t->exit_nd, curr_st.strt, curr_st.end};
            arrayList_append(next_states, &next_state);
        }else{
            size_t exit_nd = test_transition(*t, ch);

            if(exit_nd){
                struct state_tuple next_state = 
                    {exit_nd, curr_st.strt, curr_st.end + 1};

                arrayList_append(next_states, &next_state);
            }
        }
    }
}


//  ==


fsm *fsm_make(){
    fsm *f = check_malloc(sizeof(fsm), "malloc in fsm_make");

    f->data = arrayList_make(sizeof(arrayList), true);

    add_node(f);
    add_node(f);
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

size_t fsm_add_node(fsm *f){
    return add_node(f);
}

void fsm_insert_transition(fsm *f, size_t node_id, fsmTransition *t){
    insert_transition(f, node_id, t);
}

fsm *fsm_union(fsm *a, fsm *b){
    fsm *dest = fsm_make();
 
    size_t id = 0;
    arrayList *nd;
    for(nd = aL_first(&a->data); nd != aL_done(&a->data); nd = aL_next(&a->data, nd)){
        if(id >= 2){
            arrayList dest_nd = arrayList_make(0, false); 
            size_t idx = arrayList_append(&dest->data, &dest_nd) - 1;
            arrayList_deep_cpy(nd, arrayList_get_ptr(&dest->data, idx));
        }
        id++;
    }

    if(fsm_len(a) < 3)
        return dest;

    size_t shift_amt = fsm_len(a) - 3;
    nd = arrayList_get_ptr(&b->data, 2);
    fsmTransition *t;
    for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
       size_t exit_node;
       if(t->exit_nd > 2){
           exit_node = t->exit_nd + shift_amt;
       }else{
           exit_node = t->exit_nd;
       }
       fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, exit_node};
       insert_transition(dest, 2, &t_p);
    }

    
    if(fsm_len(b) < 4){
        return dest;
    }

    id = 3;
    for(nd = aL_idx(&b->data, 3); nd != aL_done(&b->data); nd = aL_next(&b->data, nd)){
        id++;
        size_t id_p = add_node(dest);
        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            size_t exit_node;
            if(t->exit_nd > 2){
                exit_node = t->exit_nd + shift_amt;
            }else{
                exit_node = t->exit_nd;
            }
            fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, exit_node};
            insert_transition(dest, id_p, &t_p);
        }
    }

    return dest;
}

fsm *fsm_concat(fsm *a, fsm *b){
    fsm *dest = fsm_make();

    size_t b_start = fsm_len(a);

    arrayList *nd;
    size_t id = 2;
    for(nd = aL_idx(&a->data, 2); nd != aL_done(&a->data); nd = aL_next(&a->data, nd)){
        
        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            size_t exit_node;
            if(t->exit_nd == 1){
                exit_node = b_start;
            }else{
                exit_node = t->exit_nd;
            }
             
            add_node(dest);

            fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, exit_node};
            insert_transition(dest, id, &t_p);
        }
        id++;
    }

    assert(id == b_start);

    size_t shift_amt = fsm_len(a) - 2;

    for(nd = aL_idx(&b->data, 2); nd != aL_done(&b->data); nd = aL_next(&b->data, nd)){
        
        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            size_t exit_node;
            if(t->exit_nd < 2){
                exit_node = t->exit_nd;
            }else{
                exit_node = t->exit_nd + shift_amt;
            }
                
            add_node(dest);

            fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, exit_node};
            insert_transition(dest, id, &t_p);
        }
        id++;
    }

    return dest;  
}

fsm *fsm_kstar(fsm *f){
    fsm *dest = fsm_make();
    
    arrayList *nd;
    size_t start_0 = add_node(dest);
    fsmTransition null = {NIL, 0, 0, 1};
    insert_transition(dest, 2, &null);

    assert(start_0 == 2);

    fsmTransition pivot = {NIL, 0, 0, 3};
    insert_transition(dest, 2, &pivot);
   
    for(nd = aL_idx(&f->data, 2); nd != aL_done(&f->data); nd = aL_next(&f->data, nd)){

        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            size_t id = add_node(dest);
            if(t->exit_nd == 1){
                fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, 3};
                insert_transition(dest, id, &t_p);
            }
            
            fsmTransition t_p;
            if(t->exit_nd == 2){
                t_p = (fsmTransition) {t->rule, t->min_ch, t->max_ch, 3};
            }else{
                t_p = (fsmTransition) {t->rule, t->min_ch, t->max_ch, t->exit_nd};
            }
            insert_transition(dest, id, &t_p);
        }
    }

    return dest;
}

fsm *fsm_kplus(fsm *f){
    fsm *g = fsm_kstar(f);
    fsm *q = fsm_concat(f, g);

    fsm_free(g);

    return q;
}

fsm *fsm_zero_or_one(fsm *f){
    fsm *e = fsm_make();

    fsm_add_node(e);

    fsmTransition null = {NIL, 0, 0, 1};
    fsm_insert_transition(e, 2, &null);

    fsm *g = fsm_union(e, f);

    fsm_free(e);

    return g;
}
    
arrayList fsm_match(
        fsm *f, wchar_t *str, int max_matches){

    arrayList matches = arrayList_make(sizeof(struct uint_tuple), true);

    arrayList stack_a = arrayList_make(sizeof(struct state_tuple), true);
    arrayList *active_states = &stack_a;

    arrayList stack_b = arrayList_make(sizeof(struct state_tuple), true);
    arrayList *next_states = &stack_b;

    struct state_tuple curr_st = {2, 0, 0};
    size_t i = 0;
    bool sent = true;
    arrayList_append(active_states, &curr_st);

    bool escaped = false;

    while(active_states->len > 0 && matches.len != max_matches){
        assert(next_states->len == 0);

        while(active_states->len > 0){
            arrayList_pop(active_states, &curr_st);
           
            if(curr_st.nd == 1){
                struct uint_tuple match = {curr_st.strt, curr_st.end};
                arrayList_append(&matches, &match);
                
                if(matches.len == max_matches)
                    break;

            }else{
                get_next_states(f, str[curr_st.end], curr_st, next_states);
            }
        }

        if(str[i] != '\0'){
            struct state_tuple next = {2, i + 1, i + 1};
            arrayList_append(next_states, &next);
            i++;
        }

        memswap(&next_states, &active_states, sizeof(arrayList *));
    
    }

    arrayList_free(&stack_a);
    arrayList_free(&stack_b);

    return matches;
}


