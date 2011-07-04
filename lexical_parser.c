#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "util.h"
#include "token.h"
#include "lexical_parser.h"

static char * number(char *, char *);
static char * real_number(char *, char *);
static char * operator(char *, char *);
static char * error(char *);
static char * single_line_comment(char *, char*);
static char * multi_line_comment(char *, char*);
static char * string_single_quote(char * cur, char * start);
static char * string_double_quote(char * cur, char * start);
static char * identifier(char * cur, char * start);
static int is_valid_symbol(char c);
static char * regex(char * cur, char * start);
static char * regex_flag(char * cur, char * start);
static char * escape_sequence(char * cur, char * start);

static const char * literals[] = {
    "true",     "false",    "{}",   "Infinity", "NaN", "null", "undefined"
};

#define RESERVED_WORDS_LENGTH 32
static const char* reserved_words[] = {
    "this",         "new",      "function",  "var",
    "for",          "if",       "else",     "true", 
    "false",        "return",   "undefined","null",
    "instanceof",   "in",       "do",       "default",
    "void",         "while",    "break",    "case", 
    "typeof",       "continue", "Infinity", "NaN",
    "delete",       "switch",   "try",      "catch",        
    "throw",        "finally",  "with",     "debugger",
};


#define FUTURE_RESERVED_WORDS_LENGTH 7
static const char * future_reserved_words[] = {
    "class",    "const",    "enum",     "export",
    "extends",  "import",   "super"
};


static const char * reserved_symbols = "{}.+!=<>;-~(),*&|%&[]?^/:\\";

#define PUNCTUATORS_LENGTH 49
static const  char * punctuators[] = {
    "{",    "}",    ".",    ">=",   "+",
    "<<",   "!",    "=",    ">>=",  ";", "/",
    "==",   "-",    ">>",   "~",    "+=", "/=", 
    ">>>=", "(",    ",",    "!=",   "*",
    ">>>",  "&&",   "-=",   "&=",   ")",
    "<",    "===",  "%",    "&",    "||",
    "*=",   "|=",   "[",    ">",    "!==",
    "++",   "|",    "?",    "%=",   "^=", "\\",
    "--",   "^",    ":",    "<<=",  "]",    "<="
};

#define ML_OFF 0
#define ML_COMMENT  1
#define ML_STRING 2
static int multiline_switch = ML_OFF;

static int current_line = (int) NULL;

//static int token_list_exist = 0;

static TokenList * tklst;

/**
    This method takes a line, iterates over its chars
    and extracts it's tokens determining whether it has lexical errors.
*/
int lexical_analisys(char * line, TokenList * tokenList, int line_number) {
    char * c;
    c = line; //initial position

    tklst = tokenList;

    current_line = line_number;

    while(1) {
        if(multiline_switch) {                                  //multiline_switch is ON
            switch(multiline_switch){
                case ML_COMMENT:
                	if(LOG) printf("Line numbr: %d\n", current_line);
                    c = multi_line_comment(c+1, c);
                    break;
                case ML_STRING:
                    //not implemented yet
                    error(c);
                    return 1;
                default:
                    //not implemented yet
                    error(c);
                    return 1;
            }
        }

        if(isdigit(*c)){                                        //digito
            c = number(c+1, c);
            if(c == NULL) {
                return 1;
            }
        } else if (isalpha(*c) || *c == '$' || *c == '_') {     //alpha
            c = identifier(c+1, c);
            
            
        } else if(is_valid_symbol(*c)){                         //symbol
            c = operator(c+1, c);
            //printf("%c, %d - %x: en symbol\n", *c, *c, (unsigned int)*c);
            if(c == NULL){ //TODO: why is this case here? check!
                return 1;
            } else if (*c == '\n' || *c == '\r') {
            	return 0;
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
            case '\n': case '\r':
                if(LOG) printf("== New line character ==\n");
                if(LOG) printf("Finished parsing line\n\n");

                Token * token = createToken(c, c, 0, 0, NEW_LINE);
                appendToken(tklst, token);
                return 0;
            case '\0': //accounts for EOF, when a new line is not provided.
                if(LOG) printf("Finished parsing line\n\n");
                return 0;
            default:
                printf("unrecognized char! %c\n", *c);
                error(c);
                return 1;
            }
        }
    }
}

static char * escape_sequence(char * cur, char * start) {
	//hex escape sequence: '\x64' (hex char(2 digits))
	//unicode escape: '\u1234' (unicode char (4 digits))
	switch(*cur) {
	case '\'':
		cur++;
		//create token for escaped single quote
		break;
	case '\"':
		cur++;
		//create token for escaped double quote
		break;
	case '\\':
		cur++;
		//create token for escaped backslash
		break;
	case 'b':
		cur++;
		//TODO: which is this escaped char for?
		break;
	case 'f': //form feed
		cur++;
		//create token for escaped form feed
		break;
	case 'n': //line feed
		cur++;
		//create token for escaped line feed
		break;
	case 'r': //carriage return
		cur++;
		//create token for escaped car. return
		break;
	case 't': //horizontal tab
		cur++;
		//create token for escaped horiz tab
		break;
	case 'v': //vertical tab
		cur++;
		//create token for escaped vertical tab
		break;
	case 'x':
		//hex_character(*cur);
		cur = cur+3; //temporary
		break;
	case 'u':
		//unicode_chracter(*cur); TODO
		cur = cur+5; //temp jumping to next characters
		break;
	case '/': //this slash is escaped when on regex. does this belong here? Temp fix
		cur++;
		if(LOG) printf("Escaped Bar");
		break;
	default:
		break;
	}
	return cur;
}

static char * regex(char * cur, char * start) {

	if(*cur == '\\') {
		cur = escape_sequence(cur+1, cur); //jump through escaped parts
	}

	if( *cur == '/') {
		return regex_flag(++cur, start);
	} else { //TODO: here I should check the standard for not allowed characters instead of else)
		return regex(++cur, start);
	}
}

static char * regex_flag(char * cur, char * start) {
	if(isalpha(*cur)) { //TODO: are only letters valid? do numbers and punct work too?
		return regex_flag(++cur, start);
	} else {
		Token * token = createToken(start, cur-1, 0, 0, REGEX);
		appendToken(tklst, token);

		if(LOG) printf("regex: %s\n", get_string(start, cur-1));

		return cur;
	}
}

static char * operator(char * cur, char * start){
	if(*start == '/' && isgraph(*cur)) {
		if(*cur == '/'){
			return single_line_comment(cur+1, start);
		}
		if(*cur == '*'){
			return multi_line_comment(cur+1, start);
		}
		else {
			//printf("REGEXP! %c, %c \n", *cur-1, *cur);
			return regex(cur, start);
		}
	}else if(is_valid_symbol(*cur) ) {
		//continues finding an operator
        return operator(cur+1, start);
    } else {
        int string_index;
        if((string_index = is_string_in_array(get_string(start, cur-1), punctuators, PUNCTUATORS_LENGTH))) { 
            if(LOG) printf("punctuator: %s - punctuator index: %d, string_index: %d\n", get_string(start, cur-1), PUNCTUATOR, string_index);

            Token * token = createToken(start, cur-1, 0, 0, PUNCTUATOR + string_index);
            appendToken(tklst, token);
            return cur;
        } else { 
            /* This else branch means that some concatenated punctuators were identified but not recognized
                They need to be separated on their own. e.g. "(){" would be needed to expand to tokens "(", ")" and "{" respectively
            */
            if(LOG) printf("possible error on punctuator: %s\n", get_string(start, cur-1));

            char * c = cur-1; //last punctuator found
            //decrementing c until a valid punctuator is found. next char of c is returned
            while(!(string_index = is_string_in_array(get_string(start, --c), punctuators, PUNCTUATORS_LENGTH)));  //&& cur >= start){
                
            if(LOG) printf("trying to fix punctuator: %s\n", get_string(start, c));
            
            Token * token = createToken(start, c, 0, 0, PUNCTUATOR + string_index);
            appendToken(tklst, token);
            
            return c+1;
        }
    }
}

//TODO: Add support for hex (ie: 0x123)
// and positive and negative numbers (at sintax?), and exponents(ie: 234e10, 45E100)
static char * number(char * cur, char * start) {
    switch(*cur) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        //printf("%c - %x\n",*cur, (unsigned int)cur );
        return number(cur+1, start);
    //case '+': case '-': case '*': case '/':
    //case ' ': case '\t': case '\0': case '\n':
    case '.':
        return real_number(cur+1, start); 
    case 'e': case 'E':
    	//-return exponential_number(cur+1, start);
    default:
        if(LOG) printf("integer: %s\n", get_string(start, cur-1));

        Token * token = createToken(start, cur-1, 0, 0, LITERAL_NUMBER);
        appendToken(tklst, token);
        
        return cur;
    }
}

static char * identifier(char * cur, char * start) {
    
    
    if(isalnum(*cur) || *cur == '_' || *cur == '$') {
        return identifier(++cur, start);
    }
    
    char * str = get_string(start, cur-1);  
    int string_index;
    if((string_index = is_string_in_array(str, reserved_words, RESERVED_WORDS_LENGTH))) {
        if(LOG) printf("reserved word: %s - kw_index = %d - token index: %d\n", str, string_index, KEYWORD + string_index );
        
        Token * token = createToken(start, cur-1, 0, 0, KEYWORD + string_index);
        appendToken(tklst, token);
        
    } else if ((string_index = is_string_in_array(str, future_reserved_words, FUTURE_RESERVED_WORDS_LENGTH))) {
        if(LOG) printf("future reserved word: %s - kw_index = %d - token index: %d\n", str, string_index, KEYWORD + string_index );
        
        Token * token = createToken(start, cur-1, 0, 0, KEYWORD + string_index);
        appendToken(tklst, token);
        
    } else {
        if(LOG) printf("identifier: %s\n", str);
        Token * token = createToken(start, cur-1, 0, 0, IDENTIFIER);
        appendToken(tklst, token);
        
    }
    return cur;
}

/*
TODO I Need to account for zero at left. 0005 should not be a valid number.
0 is valid, 30 is valid, 0.3 is valid, 03 is not
*/
static char * real_number(char * cur, char * start) {
    switch(*cur) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return real_number(cur+1, start);
    default:
        if(LOG) printf("float: %s\n", get_string(start, cur-1));
        Token * token = createToken(start, cur-1, 0, 0, LITERAL_NUMBER);
        appendToken(tklst, token);
        return cur;
    }
}

/*
* Parses the double-quote strings. i.e: "abcdefg"
*/
static char * string_double_quote(char * cur, char * start) {
    switch(*cur){
        case '\"':
            if(*(cur-1) != '\\') {
                if(LOG) printf("string: %s\n", get_string(start, cur));
                Token * token = createToken(start, cur, 0, 0, LITERAL_STRING);
                appendToken(tklst, token);
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
                if(LOG) printf("string: %s\n", get_string(start, cur));
                Token * token = createToken(start, cur, 0, 0, LITERAL_STRING);
                appendToken(tklst, token);
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
        case '\n':  case '\r':
            if(LOG) printf("SLC: %s\n", get_string(start, cur-1));
            Token * token = createToken(start, cur-1, 0, 0, SINGLE_LINE_COMMENT);
            appendToken(tklst, token);
            return cur;
        default:
            //printf("in comm: %c (%d) - %x\n", *cur, *cur, (unsigned int) cur);
            return single_line_comment(cur+1, start);
    }
}

static char * multi_line_comment(char * cur, char * start) {
	if(LOG) printf("%c, %d - %x: detecto comment ml!\n", *cur, *cur, (unsigned int)*cur);
    switch(*cur){
        case '\n': case '\r':
            multiline_switch = ML_COMMENT;
            return cur;
        case '/':
            //posible segfault when first element of the line buffer
            if(*(cur-1) == '*'){
                if(LOG) printf("MLC: %s\n", get_string(start, cur));

                Token * token = createToken(start, cur, 0, 0, MULTI_LINE_COMMENT);
                appendToken(tklst, token);

                //turn off ml switch
                if(multiline_switch) multiline_switch = ML_OFF;
                return cur+1;
            }
            //do not break case as it should enter default
        default:
            return multi_line_comment(cur+1, start);
    }
}



static char * error(char *  cur) {
    printf("Error found near %c. Stopping parser\n", *cur);
    return cur;
}


static int is_valid_symbol(char c){
    if(c == '\0') return 0;
    
    return is_char_in_array(c, reserved_symbols);
}



