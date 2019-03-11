#include "fsm.h"
#include <wchar.h>

const int NUM_MCH = 14;
const wchar_t metaChars[] = {0,
    
    '\\',                           //escape

    '^', '$', '.',                  //start

    '*', '+', '?', '|',

    '(', ')',  '[', ']',  '{', '}'

};
const char *metaChar_repr[] = {"",
    "escape",
    
    "beginning of string", "end of string", "all char except newline",

    "kleene star", "match at least once", "match at most once",

    "grouping", "grouping", "character set", "character set", "repitiion", "repitition"
};


fsm *parse_regex(wchar_t *pattern);
