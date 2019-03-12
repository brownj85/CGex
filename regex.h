#include "fsm.h"
#include <wchar.h>

#ifndef REGEX_H
#define REGEX_H

enum metaCharIdx{
    ESC = 1,
    MATCH_ANY = 2,
    KSTAR = 3,
    ALT = 4,
    LGRP = 5,
    RGRP = 6,
    LSET = 7,
    RSET = 8
};

extern const int NUM_MCH;
extern const wchar_t metaChars[];
extern const char *metaChar_repr[];
extern const bool metaChar_impl[];

fsm *parse_regex(wchar_t *pattern);

#endif
