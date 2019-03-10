#include "fsm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


wchar_t *prompt_input(char *prompt){
    char buf[128];
    printf("%s: ", prompt);
    fgets(buf, 128, stdin);

    return convert_to_wchar(buf);
}



wchar_t *convert_to_wchar(char *str){
    wchar_t wstr = malloc(sizeof(wchar_t) * (strlen(str) + 1));

    for(int i = 0; i < strlen(str); i++)
        wstr[i] = (wchar_t) str[i];

    wstr[strlen(str)] = (wchar_t) '\0';

    return wstr;
}

fsm *read_in_string(){
    char buf[128];
    wchar_t *str = prompt_input("Enter string regex", buf);

    fsm *f = make_string_fsm(str);

    free(str);

    return f;
}


int main(int argc, char **argv){
    f = read_in_string();

    wchar_t *str = prompt_input("Enter string");



        
}
