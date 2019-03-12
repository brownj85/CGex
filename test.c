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


fsm *read_in_string_fsm(){
    wchar_t *str = prompt_input("Enter string regex: ");

    fsm *f = make_string_fsm(str, true);

    free(str);

    return f;
}

fsm *read_in_set_fsm(){
    wchar_t *str = prompt_input("Enter set regex: ");
    size_t num_tokens = wstrtok(str, ' ');

    struct uint_tuple ranges[16];

    wchar_t *curr = str;
    for(int i = 0; i < num_tokens; i++){
        ranges[i] = (struct uint_tuple) {*curr, *(curr + 2)};
        printf("%c, %c \n", *curr, *(curr + 2));
        curr += 4;
    }

    fsm *f = make_charset_fsm(ranges, num_tokens, true);

    free(str);

    return f;
}

fsm *get_fsm(char *type){
    fsm *ret = NULL;
    
    if(type == NULL){
        char *type = cprompt_input("Enter regex type: ");
        
        ret = get_fsm(type);
        free(type);
    }else if(strcmp(type, "string") == 0){
        ret = read_in_string_fsm();
    }else if(strcmp(type, "set") == 0){
        ret = read_in_set_fsm();
    }else if(strcmp(type, "union") == 0){
        fsm *f1 = get_fsm(NULL);
        fsm *f2 = get_fsm(NULL);

        ret = fsm_union(f1, f2);
        fsm_free(f1);
        fsm_free(f2);
    }else if(strcmp(type, "concat") == 0){
        fsm *f1 = get_fsm(NULL);
        fsm *f2 = get_fsm(NULL);

        ret = fsm_concat(f1, f2);
        fsm_free(f1);
        fsm_free(f2);
    }else if(strcmp(type, "kstar") == 0){
        fsm *f = get_fsm(NULL);

        ret = fsm_k_star(f);

        fsm_free(f);
    }else{
        printf("invalid regex type\n");
        ret = get_fsm(NULL);
    }

    return ret;
}
int main(int argc, char **argv){
  

    while(true){
        wchar_t *pattern = prompt_input("enter regex: ");
        fsm *f = parse_regex(pattern);

        print_fsm(f);
    }
}
