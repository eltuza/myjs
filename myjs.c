#include <stdio.h>
#include <malloc.h>
#include "token.h"
#include "lexical_parser.h"
#include "io.h"


int main(int argc, char *argv[]){
    FILE * f;
    TokenList * tokenList;
    f_i fi;
    
    printf("============\n");
    printf("    myjs\n");
    printf("============\n\n");
    
    if(argc == 1){
        //TODO this default src file is for testing speed. Remove later.
        f = fopen("src.js", "r");
        //printf("Usage: %s script.js\n", argv[0]);
        //return 0;
    } else {
        f = fopen(argv[1], "r");
    }

    if(f == NULL){
        printf(">>>>>ERROR: File does not exist.\n>>>>>Exiting...\n");
        return 1;
    }

    fi = (f_i) malloc(sizeof(struct FILE_INFO));
    fi = get_file_info(f, &fi);

    rewind(f);

    tokenList = initializeTokenList(tokenList);

    printf("LoC: %d\n", fi->lines);
    printf("Max line length: %d\n", fi->max_line_length);
    
    char * line_buff = (char*) malloc(sizeof(char) * fi->max_line_length);
    
    int line_num = 0;

    while(get_line(line_buff, fi->max_line_length, f) != NULL){

        if(LOG) printf(">>>>> Parsing Line: %s\n", line_buff);
        
        if(lexical_analisys(line_buff, tokenList, ++line_num) == 1) {
            fclose(f);
            return 1;
        }
    }
    
    printf("\n====== Finished parsing file ======\n");
    printTokenList(tokenList);
    
    fclose(f);
    return 0;
}

