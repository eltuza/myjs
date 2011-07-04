#include <string.h>
#include <malloc.h>

char * get_string(char *, char* );
int is_char_in_array(char c, const char * arr);
int is_string_in_array(char * str, const char ** arr, int arrlen);


char * get_string(char * from, char* to) {
    int len = (to - from) / sizeof(char);
    char * new = (char *)malloc(len+1);
    
    memcpy(new, from, len+1);
    return new;
}


int is_char_in_array(char c, const char * arr){
    char * pt;
    pt = strchr(arr, c);

    return (pt == NULL) ? 0 : 1;
}

int is_string_in_array(char * str, const char ** arr, int arrlen) {
    int i;
    for(i = 0; i< arrlen; i++){
        if(! strcmp(str, arr[i])){
            return i+1;
        }
    }
    return 0;
}