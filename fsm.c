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
void insert_transition(fsm *f, size_t node_id, fsmTransition *t){
 
    assert(node_id < fsm_len(f));

    arrayList_append(
            arrayList_get_ptr(&f->data, node_id),
            t);
}

static inline size_t add_node(fsm *f){
    arrayList nd = arrayList_make(sizeof(fsmTransition), true);
    
    return arrayList_append(&f->data, &nd) - 1;
}

size_t fsm_add_node(fsm *f){
    return add_node(f);
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

void fsm_insert_transition(fsm *f, size_t node_id, fsmTransition *t){
    insert_transition(f, node_id, t);
}
fsm *fsm_make(){
    fsm *f = check_malloc(sizeof(fsm), "malloc in fsm_make");

    f->data = arrayList_make(sizeof(arrayList), true);

    add_node(f);
    fsmTransition fail = {FAIL, 0, 0, 0};
    insert_transition(f, 0, &fail);

    add_node(f);
    fsmTransition acc = {ACCEPT, 0, 0, 1};
    insert_transition(f, 1, &acc);

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

fsm *make_string_fsm(wchar_t *str, bool invert){
    fsm *f = fsm_make();

    char match_type = EQ;
    if(invert)
        match_type = N_EQ;

    if(*str == '\0'){
        return f;
    }

    while(*(str + 1) != '\0'){
        size_t nd = add_node(f);
        fsmTransition t = {match_type, *str, *str, nd + 1};
        insert_transition(f, nd, &t);
        str++;
    }
    size_t nd = add_node(f);
    fsmTransition t = {match_type, *str, *str, 1};
    insert_transition(f, nd, &t);
    
    print_fsm(f);

    return f;
}

fsm *make_charset_fsm(struct uint_tuple *ranges, size_t num_ranges, bool invert){
    return NULL;
    //    char match_type = EQ;
//    if(invert){
//        match_type = N_EQ;
//    }

//    fsm *f = fsm_make();
//    size_t nd = add_node(f);
//    for(int i = 0; i < num_ranges; i++){
//        struct uint_tuple range = ranges[i];
//        insert_transition(f, nd, match_type, range.a, range.b, 1);
//    }
//
//    print_fsm(f);
//
//    return f;
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
       if(t->exit_nd > 2)
           exit_node = t->exit_nd + shift_amt;
       else
           exit_node = t->exit_nd;

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
            if(t->exit_nd > 2)
                exit_node = t->exit_nd + shift_amt;
            else
                exit_node = t->exit_nd;
       
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
            if(t->exit_nd == 1)
                exit_node = b_start;
            else
                exit_node = t->exit_nd;
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
            if(t->exit_nd < 2)
                exit_node = t->exit_nd;
            else
                exit_node = t->exit_nd + shift_amt;
            add_node(dest);

            fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, exit_node};
            insert_transition(dest, id, &t_p);
        }
        id++;
    }

    return dest;  
}

fsm *fsm_k_star(fsm *f){
    fsm *dest = fsm_make();
    
    arrayList *nd;
    for(nd = aL_idx(&f->data, 2); nd != aL_done(&f->data); nd = aL_next(&f->data, nd)){

        fsmTransition *t;
        for(t = aL_first(nd); t != aL_done(nd); t = aL_next(nd, t)){
            size_t id = add_node(dest);
            if(t->exit_nd == 1){
                fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, 2};
                insert_transition(dest, id, &t_p);
            }
            
            fsmTransition t_p = {t->rule, t->min_ch, t->max_ch, t->exit_nd};
            insert_transition(dest, id, &t_p);
        }
    }

    return dest;
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

    while(sent && matches.len != max_matches){
        if(str[i] == '\0')
            sent = false;

        assert(next_states->len == 0);

        print_states(active_states);
        printf("%c, %d\n", str[i], escaped);        
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

       // bool escaped_trig = escaped;

     //   if(str[i] == ESC_CH && !escaped)
    //        escaped = true;
        
        if(!escaped){
            struct state_tuple next = {2, i + 1, i + 1};
            arrayList_append(next_states, &next);
        }
        i++;

//        if(escaped_trig)
  //          escaped = false;

        

        memswap(&next_states, &active_states, sizeof(arrayList *));

            
    }

    arrayList_free(&stack_a);
    arrayList_free(&stack_b);

    return matches;
}


