#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "array_list/array_list.h"
#include "util.h"

enum transitionRule{
    EQ      = 3,
    N_EQ    = 4,

    NIL     = 5,

    CTR     = 6
};

typedef struct fsmTransition{
    char rule;
    wchar_t min_ch;
    wchar_t max_ch;

    size_t exit_nd;

}fsmTransition;

typedef struct fsm{
    arrayList data;
}fsm;

fsm *fsm_make();

void fsm_free(fsm *f);

size_t fsm_len(fsm *f);

void fsm_insert_transition(fsm *f, size_t node_id, fsmTransition *t);

size_t fsm_add_node(fsm *f);

fsm *fsm_union(fsm *a, fsm *b);

fsm *fsm_concat(fsm *a, fsm *b);

fsm *fsm_kstar(fsm *f);

fsm *fsm_kplus(fsm *f);

fsm *fsm_zero_or_one(fsm *f);

arrayList fsm_match(fsm *f, wchar_t *str, int max_matches);

void print_fsm(fsm *);
