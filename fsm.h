#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#include "bucket_list/bucket_list.h"
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
    bucketList data;
    size_t num_nodes;
}fsm;

fsmTransition fsmTransition_make(
        enum transitionRule rule, wchar_t min_ch, wchar_t max_ch, size_t exit_nd);

size_t fsmTransition_test(fsmTransition transition, wchar_t ch);

fsm *fsm_make();

void fsm_free(fsm *f);

fsm *make_string_fsm(wchar_t *str);

fsm *make_charset_fsm(struct uint_tuple *ranges, size_t);

fsm *fsm_union(fsm *a, fsm *b);

fsm *fsm_concat(fsm *a, fsm *b);

fsm *fsm_k_star(fsm *f);

int fsm_match(
        fsm *f, wchar_t *str,
        struct uint_tuple *match_tups,
        size_t max_matches);
