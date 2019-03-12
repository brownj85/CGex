#include <wchar.h>
#include <stdbool.h>
#ifndef REGEX_CONST
#define REGEX_CONST

const int NUM_MCH = 8;
const wchar_t metaChars[] = {0,
    
    92,   '.',                  //character specifiers

    '*',    '|',                 //

    '(', ')',  '[', ']'         

};

const char *metaChar_repr[] = {"",
    "escape", "all char except newline",

    "kleene star", "alternation",

    "grouping_r", "grouping_l", "character set l", "character set r"
};

#endif
