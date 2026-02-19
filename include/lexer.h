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
 
 /* =======================
        Basic Types
    ======================= */
 
typedef uint8_t bool;
#define true  1
#define false 0
#define MAX_IDENT_LEN 10
 
/* =======================
    Token and State Enums
   ======================= */
 
typedef enum {
    TOKEN_IDEN_GENERIC,     // 0
    TOKEN_KEYWORD,          // 1
    TOKEN_INT,              // 2
    TOKEN_FLOAT,            // 3
    TOKEN_PLUS,             // 4
    TOKEN_MINUS,            // 5
    TOKEN_MUL,              // 6
    TOKEN_DIV,              // 7
    TOKEN_ASSIGN,           // 8
    TOKEN_EQUAL,            // 9
    TOKEN_NOTEQ,            // 10
    TOKEN_LT,               // 11
    TOKEN_LTE,              // 12
    TOKEN_GT,               // 13
    TOKEN_GTE,              // 14
    TOKEN_SEMICOL,          // 15
    TOKEN_EOF,              // 16
    TOKEN_ERR,              // 17
    TOKEN_LBRACE,           // 18
    TOKEN_RBRACE,           // 19
    TOKEN_LPAREN,           // 20
    TOKEN_RPAREN,           // 21
    TOKEN_STRING,           // 22
    TOKEN_DELIM,            // 23
    TOKEN_UNKOWN,           // 24
    TOKEN_OPER
} TokenType;
 
 /* =======================
        Lexer Structs
    ======================= */

typedef struct {
    TokenType type;
    // anonomous union you dont need to .value.value
    union {
        //char identifier[MAX_IDENT_LEN];
        char *stringVal;
        int intVal;
        float floatVal;
        char operator;
        char value; // for simple tokens like ';'
    };
} Token;
 
typedef struct {
    const char *input;  /* Input string to tokenize */
    size_t      pos;    /* Current position in input */
} LexerInfo;


/* =======================
          Prototypes
   ======================= */
 
/* Lexer entry point */
LexerInfo *lexerCreate(const char *inputString);
LexerInfo *lexerCreateFromFile(const char *filename);

//lexer helpers
char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);

//token functions and helpers
Token *nextToken(LexerInfo *lxer);
Token *numHandler(LexerInfo *lxer);
Token *opHandler(LexerInfo *lxer);
Token *identHandler(LexerInfo *lxer);
Token *stringHandler(LexerInfo *lxer);
Token *delimHandler(LexerInfo *lxer);

void printTokenType(Token *tok);
#endif /* LEXER_H */
 