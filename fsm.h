#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "array_list/array_list.h"
#include "util.h"

enum transitionRule{
    FAIL    = 0,
    ACCEPT  = 1,

    EQ      = 3,
    N_EQ    = 4,
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

fsmTransition fsmTransition_make(
        enum transitionRule rule, wchar_t min_ch, wchar_t max_ch, size_t exit_nd);

size_t fsmTransition_test(fsmTransition transition, wchar_t ch);

fsm *fsm_make();

void fsm_free(fsm *f);

size_t fsm_len(fsm *f);

fsm *make_string_fsm(wchar_t *str, bool invert);

fsm *make_charset_fsm(struct uint_tuple *ranges, size_t num_ranges, bool invert);

fsm *fsm_union(fsm *a, fsm *b);

fsm *fsm_concat(fsm *a, fsm *b);

fsm *fsm_k_star(fsm *f);

arrayList fsm_match(fsm *f, wchar_t *str, size_t max_matches);


void print_fsm(fsm *);
