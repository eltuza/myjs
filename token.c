#include <malloc.h>
#include <string.h>
#include "token.h"

TokenList * initializeTokenList(TokenList * tkList) {
    tkList = malloc(sizeof(TokenList));
    tkList->length = 0;
    tkList->first = NULL;

    return tkList;
}


void printTokenList(TokenList * tl){
    if(tl->first == NULL){
        printf("The Token List is empty!!\n\n");
    }
    Token * token = tl->first;
    while(token != NULL){
        printToken(token);
        token = token->next;
        printf("--------------------------------------------------\n");
    }
    printf("Token List size: %d\n", tl->length);

    return;
}

int appendToken(TokenList * list, Token * newToken) {
    Token * token;
    if(list == NULL) return 0;
    
    newToken->tokenPosition = list->length;
    
    if(list->first == NULL) {
        list->first = newToken;
    } else {
        token = (Token*) list->first;
        
        while(token->next != NULL){
            token = token->next;
        }
        token->next = newToken;
    }
    list->length++;
    return 1;
}

Token * getNextToken(TokenList * list) {
    Token * ret;
    if(list == NULL || list->first == NULL) {
        return NULL;
    }
    
    ret = list->first;

    list->first = list->first->next;
    list->length--;

    return ret;
}

Token * createToken ( char * start, char *  end, int lineNumber, int position, TokenType type) {

    char * content;
    size_t tokenLength;

    Token * token = malloc(sizeof(Token));

    token->tokenLength =  ((end - start) / sizeof(char)) +1;
    token->content = malloc(token->tokenLength);
    
    memcpy(token->content, start, (token->tokenLength));
    token->type = type;
    token->position = position;
    token->lineNumber = lineNumber;

    TokenValue * val = malloc(sizeof(TokenValue));

    //TODO hacen lo mismo por ahora
    if(type == LITERAL_NUMBER ){
        val->string = start;
    } else {
        val->string = start;
    }

    //TODO: Checkear esto, Token tiene un TokenValue en lugar de un apuntador
    token->value = *val;

    //token->line
    //token->lineLength

    token->next = NULL;
    return token;
}

void printToken(Token *token) {
    printf("Token: %s \t Type: %d \t Length: %d \n", token->content, token->type, (int)token->tokenLength);
    return;
}
