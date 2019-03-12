#include <wchar.h>
#ifndef REGEX_CONST
#define REGEX_CONST

const int NUM_MCH = 10;
const wchar_t metaChars[] = {0,
                        
    92,                 
    
    '.',                  

    '*',  '+',  '?',    
    
    '|',                   

    '(', ')',           
    
    '[', ']'            

};

const int NUM_QTFR = 3;
const wchar_t quantifier[] = {
    '*', '+', '?'
};

const char *metaChar_repr[] = {"",
    "escape", "all characters except newline",

    "kleene star", "kleene plus", "at most once", "alternation",

    "grouping", "grouping", "character set", "character set"
};

#endif
