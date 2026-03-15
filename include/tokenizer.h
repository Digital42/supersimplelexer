/******************************************************************************
* File:        tokenizer.h   
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: Header file for the tokenizer
******************************************************************************/
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "scanner.h"
#include <stddef.h>   // size_t
#include <stdint.h>   // uint8_t, int32_t
#include <stdio.h>
 
/* =======================
    Token and State Enums
   ======================= */

#define TOKEN_LIST \
    /* ================= IDENTIFIERS / LITERALS ================= */ \
    X(TOKEN_IDEN_GENERIC) \
    X(TOKEN_KEYWORD) \
    X(TOKEN_INT) \
    X(TOKEN_FLOAT) \
    X(TOKEN_STRING) \
    X(TOKEN_CHAR) \
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
    X(TOKEN_LBRAK) \
    X(TOKEN_RBRAk) \
    X(TOKEN_LPAREN) \
    X(TOKEN_RPAREN) \
    X(TOKEN_OBJ_INDREC) \
    X(TOKEN_INDIRECTION) \
    X(TOKEN_STRUC_REF) \
    X(TOKEN_COLON) \
    X(TOKEN_SEPERATOR) \
    /* ================= MULTI-CHAR OPERATORS ================= */ \
    X(TOKEN_LITERAL) \
    X(TOKEN_NEGATION) \
    X(TOKEN_EQ_EQ) \
    X(TOKEN_LOGICAL_NOT) \
    X(TOKEN_NOTEQ) \
    X(TOKEN_LTE) \
    X(TOKEN_GTE) \
    X(TOKEN_PLUS_EQ) \
    X(TOKEN_MINUS_EQ) \
    X(TOKEN_MUL_EQ) \
    X(TOKEN_DIV_EQ) \
    X(TOKEN_INCR) \
    X(TOKEN_DECR) \
    X(TOKEN_BITWISEL) \
    X(TOKEN_BITWISER) \
    X(TOKEN_ADDRESS_OF) \
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

typedef void (*TokenErrorCallback)(size_t line, size_t column, const char *message, void *userData, const char *errChar);

typedef enum {
    #define X(name) name,
        TOKEN_LIST
    #undef X
        TOKEN_COUNT
} TokenType;
 

 /* =======================
        Tokenizer Structs
    ======================= */
typedef struct {
    TokenType type;
    const char *start; // points into lexer->input
    size_t length;
} Token;
 
typedef struct {
    Reader *reader;
    TokenErrorCallback errorFn;  // user-supplied callback
    void *errorUserData;         // user data passed back to callback
} Tokenizer;


/* =======================
          Prototypes         
   ======================= */
 
//token functions and helpers
Token tokNextToken(Tokenizer *tok);
Token numHandler(Tokenizer *tok);
Token opHandler(Tokenizer *tok);
Token identHandler(Tokenizer *tok);
Token stringHandler(Tokenizer *tok);
void delimHandler(Tokenizer *tok);
Token charHandler(Tokenizer *tok);
Token commentHandler(Tokenizer *tok);
Token eofHandler(Tokenizer *tok);

// creates a tokenizers and sets default values
void tokenizerInit(Tokenizer *Tok, Reader *reader, TokenErrorCallback errorFn, void *userData);

// error reporting functions
void reportTokenError(Tokenizer *tok, size_t line, size_t row, const char *msg, const char *current);

// debug and display functions
void printFormatted(const char *tokStr, int length);
void printTokenType(Token tok);
#endif /* TOKENIZER_H */