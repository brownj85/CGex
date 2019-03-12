#include "regex.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


size_t wstrtok(wchar_t *str, wchar_t delim){
    size_t num_tok = 1;
    for(int i = 0; i < wcslen(str); i++){
        if(str[i] == delim){
            str[i] = '\0';
            num_tok++;
        }
    }

    return num_tok;
} 


wchar_t *convert_to_wchar(char *str){
    wchar_t *wstr = malloc(sizeof(wchar_t) * (strlen(str) + 1));

    for(int i = 0; i < strlen(str); i++)
        wstr[i] = (wchar_t) str[i];

    wstr[strlen(str)] = (wchar_t) '\0';

    return wstr;
}

void remove_newline(char *str){
    while(*str != '\0'){
        if(*str == '\n')
            *str = '\0';

        str++;
    }
}

wchar_t *prompt_input(char *prompt){
    char buf[128];
    printf("%s", prompt);
    fgets(buf, 128, stdin);
    remove_newline(buf);
    
    return convert_to_wchar(buf);
}

char *cprompt_input(char *prompt){
    char *buf = malloc(128);
    printf("%s", prompt);
    fgets(buf, 128, stdin);
    buf[strlen(buf) - 1] = '\0';

    return buf;
}    
int main(int argc, char **argv){
    
        wchar_t *pattern = prompt_input("enter regex: ");
        fsm *f = parse_regex(pattern);

        while(true){
            wchar_t *in = prompt_input("enter string: ");
            arrayList m = fsm_match(f, in, -1);

            struct uint_tuple *t;
            for(t = aL_first(&m); t != aL_done(&m); t = aL_next(&m, t)){
                print_tup(*t);
            }
        }
}

