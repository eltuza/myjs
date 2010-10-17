#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "lexical_parser.h"


/*
    GRAMATIC: (ONLY FOR CALCULATOR - OUTDATED)
        EXPRESSION := TERM {"+" TERM | "-" TERM}.
        TERM := FACTOR {"*" FACTOR | "/" FACTOR}.
        FACTOR := NUMBER | "(" EXPRESSION ")".
        NUMBER := "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" |"8" |"9" | {NUMBER}.
 
*/
static char * number(char *, char *);
static char * real_number(char *, char *);
static char * operator(char *, char *);
static char * get_string(char *, char* );
static char * error(char *);
static char * single_line_comment(char *, char*);
static char * string_single_quote(char * cur, char * start);
static char * string_double_quote(char * cur, char * start);
static char * identifier(char * cur, char * start);
static int is_char_in_array(char c, const char * arr);
static int is_valid_symbol(char c);
static int is_string_in_array(char * str, const char ** arr, int arrlen);

#define RESERVED_WORDS_LENGTH 26

static const char* reserved_words[] = {
    "function",     "var",      "new",      "break",
    "case",         "catch",    "continue", "debugger",
    "default",      "delete",   "do",       "else",
    "finally",      "for",      "if",       "in",
    "instanceof",   "return",   "switch",   "this",
    "throw",        "try",      "typeof",   "void",
    "while",        "with"
};

#define FUTURE_RESERVED_WORDS_LENGTH 7
static const char * future_reserved_words[] = {
    "class",    "const",    "enum",     "export",
    "extends",  "import",   "super"
};


static const char * reserved_symbols = "{}.+!=<>;-~(),*&|%&[]?^/:";

#define PUNCTUATORS_LENGTH 46
static const  char * punctuators[] = {
    "{",    "}",    ".",    ">=",   "+",
    "<<",   "!",    "=",    ">>=",  ";",
    "==",   "-",    ">>",   "~",    "+=",
    ">>>=", "(",    ",",    "!=",   "*",
    ">>>",  "&&",   "-=",   "&=",   ")",
    "<",    "===",  "%",    "&",    "||",
    "*=",   "|=",   "[",    ">",    "!==",
    "++",   "|",    "?",    "%=",   "^=",
    "--",   "^",    ":",    "<<=",  "]",    "<="
};

#define DIVIDER_PUNCTUATORS_LENGTH 2
static const char *divider_punctuators[] = { "/", "/=" };

int lexical_analisys(char * line) {
    char * c;
    c = line; //initial position

    while(1) {
        if(isdigit(*c)){                                        //digito
            
            c = number(c+1, c);
            if(c == NULL) {
                return 1;
            }
        } else if (isalpha(*c) || *c == '$' || *c == '_') {     //alpha
            
            c = identifier(c+1, c);
            
        } else if(is_valid_symbol(*c)){                         //symbol
            
            switch(*c){
            //TODO equal "=" is not an operator....this is for testing only, locate it properly    
            case '+': case '-': case '*': case '/': case '=':
                c = operator(c+1, c);
                if(c == NULL){
                    return 1;
                }
                break;
            case '(':
                error(NULL);
                break;
            case ')':
                error(NULL);
                break;
            default:
                //TODO  Implement this. Should not be an error.
                error(c);
                return 1;    
            }
        } else {
            
            switch(*c){
            case '\"':
                c = string_double_quote(c+1, c);
                break;
            case '\'':
                c = string_single_quote(c+1, c);
                break;
            case ' ': case '\t':
                c = c+1;
                break;
            case '\n': case '\0':
                printf("Finished parsing line\n");
                return 0;
            default:
                error(c);
                return 1;
            }
        }
    }
}

static char * operator(char * cur, char * start){
    switch(*cur){
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case ' ': case '(':
        printf("operator: %s\n", get_string(start, cur-1));
        return cur;
    case '/':
        if(*start == '/'){
            return single_line_comment(cur+1, start);
        }
    case '+': case '*': case '-':
        error(cur);
        return NULL;
    default:
        printf("operator: %s\n", get_string(start, cur-1));
        return cur;
    } 
}

static char * number(char * cur, char * start) {
    switch(*cur) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
//             printf("%c - %x\n",*cur, (unsigned int)cur );
        return number(cur+1, start);
    case '+': case '-': case '*': case '/':
    case ' ': case '\t': case '\0': case '\n':
        printf("integer: %s\n", get_string(start, cur-1));
        return cur;
    case '.':
        return real_number(cur+1, start);
    default:
        error(cur);
        return NULL;
    }
}

static char * identifier(char * cur, char * start) {
    if(isalnum(*cur) || *cur == '_' || *cur == '-') {
        return identifier(++cur, start);
    }

    char * str = get_string(start, cur-1);
    if(is_string_in_array(str, reserved_words, RESERVED_WORDS_LENGTH)) {
        printf("reserved word: %s\n", str );
    } else if (is_string_in_array(str, future_reserved_words, FUTURE_RESERVED_WORDS_LENGTH)) {
        printf("future reserved word: %s\n", str );
    } else {
        printf("identifier: %s\n", str);
    }
    
    return cur;
}

static char * real_number(char * cur, char * start) {
    switch(*cur) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return real_number(cur+1, start);
    case '+': case '-': case '*': case '/':
    case ' ': case '\t': case '\0': case '\n':
        printf("float: %s\n", get_string(start, cur-1));
        return cur;
    default:
        error(cur);
        return NULL;
    }
}
/*
* Parses the double-quote strings. i.e: "abcdefg"
*/
static char * string_double_quote(char * cur, char * start) {
    switch(*cur){
        case '\"':
            if(*(cur-1) != '\\') {
                printf("string: %s\n", get_string(start, cur));
                return ++cur;
            }
            return string_double_quote(cur+1, start);
        default:
            //printf(" %c, %x", *cur, (unsigned int) cur);
            return string_double_quote(cur+1, start);

    }
}
/*
* Parses the single-quote strings. i.e: 'abcdefg'
*/
static char * string_single_quote(char * cur, char * start) {
    switch(*cur){
        case '\'':
            if(*(cur-1) != '\\') {
                printf("string: %s\n", get_string(start, cur));
                return ++cur;
            }
            return string_single_quote(cur+1, start);
        default:
            return string_single_quote(cur+1, start);

    }
}

/*
* Parses single-line comments. i.e: //some comment till end of line
*/
static char * single_line_comment(char * cur, char * start) {
    switch(*cur){
        case '\n': 
            printf("SLC: %s\n", get_string(start, cur));
            return cur+1;
        default:
            //printf("in comm: %c, %x\n", *cur, (unsigned int) cur);
            return single_line_comment(cur+1, start);
    }
}

static char * get_string(char * from, char* to) {
    int len = (to - from) / sizeof(char);
    char * new = (char *)malloc(len+1);
    
    memcpy(new, from, len+1);
    return new;
}

static char * error(char *  cur) {
    printf("Error found near %c. Stopping parser\n", *cur);
    return cur;
}

static int is_char_in_array(char c, const char * arr){
    char * pt;
    pt = strchr(arr, c);

    return (pt == NULL) ? 0 : 1;
}
static int is_valid_symbol(char c){
    if(c == '\0') return 0;
    
    char * pt;
    pt = strchr(reserved_symbols, c);

    return (pt == NULL) ? 0 : 1;
}

static int is_string_in_array(char * str, const char ** arr, int arrlen) {
    int i;
    for(i = 0; i< arrlen; i++){
        if(! strcmp(str, arr[i])){
            return 1;
        }
    }
    return 0;
}

