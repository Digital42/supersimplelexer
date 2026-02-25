/******************************************************************************
* File:        lexer.h   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Header file for the lexer
******************************************************************************/
#ifndef LEXER_H
#define LEXER_H
 
#include <stddef.h>   // size_t
#include <stdint.h>   // uint8_t, int32_t
#include <stdio.h>
 
// i dont like this seems like its bad practice make sure to figure this out later
#define isoctal(c) ((c) >= '0' && (c) <= '7')
#define isbinary(c) ((c) >= '0' && (c) <= '1')
 /* =======================
        Basic Types
    ======================= */
 
typedef uint8_t bool;
#define true  1
#define false 0

#define TOKEN_LIST \
    /* ================= IDENTIFIERS / LITERALS ================= */ \
    X(TOKEN_IDEN_GENERIC) \
    X(TOKEN_KEYWORD) \
    X(TOKEN_INT) \
    X(TOKEN_FLOAT) \
    X(TOKEN_STRING) \
    X(TOKEN_HEX) \
    X(TOKEN_BIN) \
    X(TOKEN_OCT) \
    /* ================= SINGLE-CHAR OPERATORS ================= */ \
    X(TOKEN_PLUS) \
    X(TOKEN_MINUS) \
    X(TOKEN_MUL) \
    X(TOKEN_DIV) \
    X(TOKEN_MOD) \
    X(TOKEN_ASSIGN) \
    X(TOKEN_LT) \
    X(TOKEN_GT) \
    X(TOKEN_SEMICOL) \
    X(TOKEN_LBRACE) \
    X(TOKEN_RBRACE) \
    X(TOKEN_LPAREN) \
    X(TOKEN_RPAREN) \
    /* ================= MULTI-CHAR OPERATORS ================= */ \
    X(TOKEN_LITERAL) \
    X(TOKEN_NEGATION) \
    X(TOKEN_EQ_EQ) \
    X(TOKEN_NOTEQ) \
    X(TOKEN_LTE) \
    X(TOKEN_GTE) \
    X(TOKEN_PLUS_EQ) \
    X(TOKEN_MINUS_EQ) \
    X(TOKEN_MUL_EQ) \
    X(TOKEN_DIV_EQ) \
    X(TOKEN_INCR) \
    X(TOKEN_DECR) \
    /* ================= DELIMITERS / WHITESPACE ================= */ \
    X(TOKEN_DELIM_F) \
    X(TOKEN_DELIM_N) \
    X(TOKEN_DELIM_R) \
    X(TOKEN_DELIM_T) \
    X(TOKEN_DELIM_V) \
    X(TOKEN_DELIM_S) \
    X(TOKEN_DELIM_U) \
    /* ================= SPECIAL ================= */ \
    X(TOKEN_OPER) \
    X(TOKEN_EOF) \
    X(TOKEN_ERR) \
    X(TOKEN_UNKNOWN) \
    X(TOKEN_COMMENT) \



typedef void (*LexerErrorCallback)(int line, int column, const char *message, void *userData, const char *errChar);

/* =======================
    Token and State Enums
   ======================= */

typedef enum {
    #define X(name) name,
        TOKEN_LIST
    #undef X
        TOKEN_COUNT
} TokenType;
 



 /* =======================
        Lexer Structs
    ======================= */

typedef struct {
    TokenType type;
    const char *start; // points into lexer->input
    size_t length;
} Token;
 
typedef struct {
    const char *input;  /* Input string to tokenize */
    size_t      pos;    /* Current position in input */
    bool ownsInput;     /* tracks if you need to delete buffer or not     */
    size_t cols;
    size_t lines;
    LexerErrorCallback errorFn;  // user-supplied callback
    void *errorUserData;         // user data passed back to callback
} LexerInfo;




/* =======================
          Prototypes         FIX THESE PROTOTYPES A LOT OF THIS STUFF SHOULDNT BE EXPOSED FROM THIS HEADER FILE I WAS JUST DOING IT FOR UNIT TESTING BECAUSE IM LE NOT SMART
   ======================= */
 
/* Lexer entry point */
LexerInfo *lexerCreate(const char *inputString);
LexerInfo *lexerCreateFromFile(const char *filename);
void lexerDestroy(LexerInfo *lex);
void reportLexerError(LexerInfo *lex, const char *msg);

//lexer helpers
char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);

//token functions and helpers
Token nextToken(LexerInfo *lxer);
Token numHandler(LexerInfo *lxer);
Token opHandler(LexerInfo *lxer);
Token identHandler(LexerInfo *lxer);
Token stringHandler(LexerInfo *lxer);
Token delimHandler(LexerInfo *lxer);

void printTokenType(Token tok);
#endif 