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
    TOKEN_LBRACK,           // 18
    TOKEN_RBRACK,           // 19
    TOKEN_LPAREN,           // 20
    TOKEN_RPAREN,           // 21
    TOKEN_STRING,           // 22
    TOKEN_DELIM             // 23
} TokenType;
 
 /* =======================
        Lexer Structs
    ======================= */
 
/* Maximum size for numeric stack (unused for now) */
#define NUM_STACK_MAX 8
 

typedef struct {
    TokenType type;
    union {
        //change this later maybe not i dunno
        char identifier[MAX_IDENT_LEN];
        int tokenIntVal;
        float tokenFloatVal;
        char operator;
        char value; // for simple tokens like ';'
    } value;
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
Token nextToken(LexerInfo *lxer);
LexerInfo *lexerCreateFromFile(const char *filename);

bool isKeyWord(const Token *tok);
 

char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);
 
 

Token numHandler(LexerInfo *lxer);
Token opHandler(LexerInfo *lxer);
Token identHandler(LexerInfo *lxer);
Token stringHandler(LexerInfo *lxer);
Token delimHandler(LexerInfo *lxer);

 /* Stack helpers (currently unused) */
/*
void push(NumStateStack *s, NumState st);
void pop(NumStateStack *s);
NumState peekNum(NumStateStack *s);
*/
void printTokenType(Token tok);
#endif /* LEXER_H */
 