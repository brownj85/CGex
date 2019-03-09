#include <stddef.h>
#include <stdint.h>

#include "bucket_list/bucket_list.h"
#include "util.h"

typedef uint32_t w_char_t;

enum transitionRule{
    FAIL    = 0,
    ACCEPT  = 1,

    EQ      = 3,
    N_EQ    = 4,
};

typedef struct fsmTransition{
    char rule;
    w_char_t min_ch;
    w_char_t max_ch;

    size_t exit_nd;
}fsmTransition;

typedef struct fsm{
    bucketList data;
    size_t num_nodes;
}fsm;

fsmTransition fsmTransition_make(
        enum transitionRule rule, w_char_t min_ch, w_char_t max_ch, size_t exit_nd);

size_t fsmTransition_test(fsmTransition transition, w_char_t ch);

fsm *fsm_make();

void fsm_free(fsm *f);

fsm *make_string_fsm(w_char_t *str);

fsm *make_charset_fsm(struct uint_tuple *ranges, size_t);

fsm *fsm_union(fsm *a, fsm *b);

fsm *fsm_concat(fsm *a, fsm *b);

fsm *fsm_k_star(fsm *f);

void fsm_match(
        fsm *f, w_char_t str,
        struct uint_tuple *match_tups,
        size_t max_matches);
