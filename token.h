#ifndef TOKEN
#define TOKEN


enum TokenType
{   IDENTIFIER, LITERAL_OBJ, LITERAL_STRING, LITERAL_BOOLEAN, LITERAL_NUMBER, LITERAL_UNDEFINED,
    LITERAL_NAN, LITERAL_INFINITY, NEW_LINE, SINGLE_LINE_COMMENT, MULTI_LINE_COMMENT, REGEX,  //regex should be in syntax
    
    KEYWORD, KW_THIS, KW_NEW, KW_FUNCTION, KW_VAR, KW_FOR, KW_IF, KW_ELSE, KW_TRUE, KW_FALSE, KW_RETURN, KW_UNDEFINED, KW_NULL, 
    KW_INSTANCEOF, KW_IN, KW_DO, KW_DEFAULT, KW_VOID, KW_WHILE, KW_BREAK, KW_CASE, KW_TYPEOF, KW_CONTINUE, KW_INFINITY, KW_NAN, 
    KW_DELETE, KW_SWITCH, KW_TRY, KW_CATCH, KW_THROW, KW_FINALLY, KW_WITH, KW_DEBUGGER, 
    
    PUNCTUATOR, LBRACKET, RBRACKET, DOT, GEQT, PLUS, LSHIFT, NEGATION, EQUAL, GGEQT, SEMICOLON, DIVISION, EEQUAL, MINUS, RSHIFT, CEDILLA, PLUSEQ, DIVISIONEQ,
    GGGEQT, LPAREN, COMMA, NOTEQUAL, STAR, GGGT, AND, MINUSEQ, ANDEQ, RPAREN, LT, EEEQUAL, PERCENTAGE, BW_AND, OR, TIMESEQ, OREQ, LBRACE,
    GT, NOTEEQUAL, INCREMENT, BW_OR, QUESTION, PERCENTAGEEQUAL, EXPEQUAL, ESCAPE, DECREMENT, EXP, COLON, LLEQT, RBRACE, LEQT
    //ADD DIVIDER PUNCTUATORS
};
typedef enum TokenType TokenType;

typedef struct TokenValue TokenValue;
struct TokenValue
{
    char * string;
    double number;
    char symbol;
};

typedef struct Token Token;
struct Token
{
    char * content;
    size_t tokenLength;
    
    int lineNumber;
    int position; //equal to start pointer. This is the char position in the source code

    char * line;
    size_t lineLength;

    TokenType type;
    TokenValue value;
    int tokenPosition; //token position in the token list

    Token * next;
};

typedef struct TokenList TokenList;
struct TokenList
{
    Token * first;
    int length;
};


int appendToken(TokenList *, Token *);
Token * getNextToken();
TokenList * initializeTokenList(TokenList *);
Token * createToken ( char * start, char *  end, int lineNumber, int position, TokenType type);
void printToken(Token *);
void printTokenList(TokenList *);

#endif
