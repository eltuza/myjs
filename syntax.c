#include <stdio.h>
#include <malloc.h>
#include "util.h"
#include "token.h"
#include "lexical_parser.h"
#include "io.h"

static int parseReservedWord(Token * tkn) ;
    
    
void parse(TokenList * tkLst) {
    Token * tkn = getNextToken(tkLst);
    
    while(tkn != NULL) {
        
        if(tkn->type == KEYWORD) {
            parseReservedWord(tkn);
        } else if (tkn->type == PUNCTUATOR) {
            
        } else if (tkn->type == IDENTIFIER) {
            
        }
        
        tkn = getNextToken(tkLst);
    
        
    }
}


static int parseReservedWord(Token * tkn) {
    if(! strcmp(tkn->content, "this")) {
        return 0;
        
    }
}

/*static int parseThisKeyword() {
    
}

/* Primary Expresion esta formada por
    this (keyword)
    Identifier: 
    Literal
        NullLiteral (null)
        BooleanLiteral (true | false)
        NumericLiteral
            DecimalLiteral
                DecimalIntegerLiteral  .  DecimalDigitsopt ExponentPartopt
                .  DecimalDigits ExponentPartopt
                DecimalIntegerLiteral ExponentPartopt
            HexIntegerLiteral
        StringLiteral
        RegularExpressionLiteral
    Array Literal
    Object Literal
    ( Expression )
*/