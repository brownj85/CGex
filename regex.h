#include "fsm.h"
#include <wchar.h>

#ifndef REGEX_H
#define REGEX_H

enum metaCharIdx{
    ESC = 1,
    MATCH_ANY = 2,
    
    KSTAR = 3,
    KPLUS = 4,
    AMO = 5,
    ALT = 6,

    LGRP = 7,
    RGRP = 8,
    LSET = 9,
    RSET = 10
};

extern const int NUM_MCH;

extern const wchar_t metaChars[];
extern const char *metaChar_repr[];

extern const int NUM_QTFR;
extern const wchar_t quantifier[];

fsm *parse_regex(wchar_t *pattern);

#endif
