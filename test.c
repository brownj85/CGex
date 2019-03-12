#include "regex.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
        wchar_t *pattern = readline(L"enter regex: ");
        fsm *f = parse_regex(pattern);

        while(true){
            wchar_t *in = readline(L"enter string: ");
            arrayList m = fsm_match(f, in, -1);

            struct uint_tuple *t;
            for(t = aL_first(&m); t != aL_done(&m); t = aL_next(&m, t)){
                wchar_t *mtch = get_substring(in, *t);
                printf("%ls\n", mtch);
            }
        }
}

