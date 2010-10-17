#include <stdio.h>
#include <malloc.h>
#include "src_reader.h"

f_i get_file_info(FILE *fp, f_i * fi){
    char c;
    int char_count = 0;
    int lines = 0;
    int max = 0;

    *fi = (f_i) malloc(sizeof(struct FILE_INFO));
    while((c = getc(fp)) != EOF){
        if(c == '\n'){
            ++lines;
            ++char_count; //always accounting for the new line character
            if(char_count > max){
                max = char_count;
            }
            char_count = 0;
        } else {
            char_count++;
        }
    }
    (*fi)->max_line_length = max;
    (*fi)->lines = ++lines;
    return *fi;
}

char * get_line(char * line, int max_line, FILE * f) {
    register int c;
    register char * str;

    str = line;
    while(--max_line > 0 && (c = getc(f)) != EOF) {
        *str++ = c;
        if(c == '\n')
            break;
    }
    *str = '\0';
    return (c == EOF && str == line) ? NULL : line;
}