#include <stdio.h>
#include <malloc.h>
#include "lexical_parser.h"
#include "src_reader.h"



int main(int argc, char *argv[]){
    printf("========================\n\n");
    printf("Tuza's Javascript Engine\n\n");
    printf("========================\n\n");


    FILE * f = fopen("src.js", "r");

    f_i fi = (f_i) malloc(sizeof(struct FILE_INFO));
    fi = get_file_info(f, &fi);

    rewind(f);

    printf("LoC: %d\n", fi->lines);
    printf("Max line length: %d\n", fi->max_line_length);
    
    char * line = (char*) malloc(sizeof(char) * fi->max_line_length);
    while(get_line(line, fi->max_line_length, f) != NULL){
        printf("MYLINE: %s\n", line);
        if(lexical_analisys(line) == 1) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

