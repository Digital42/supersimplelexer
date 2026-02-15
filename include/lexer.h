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
 
 /* =======================
        Basic Types
    ======================= */
 
typedef uint8_t bool;
#define true  1
#define false 0
 
/* =======================
    Token and State Enums
   ======================= */
 
typedef enum {
    TOKEN_IDEN_GENERIC,
    TOEKEN_KEYWORD,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOTEQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_SEMICOL,
    TOKEN_EOF,
    TOKEN_ERR
} TokenType;
 
typedef enum {
    STATE_START,
    STATE_EOF,
    STATE_ERR
} LexerState;
 
 /* =======================
        Lexer Structs
    ======================= */
 
/* Maximum size for numeric stack (unused for now) */
#define NUM_STACK_MAX 8
/*
typedef struct {
    NumState stack[NUM_STACK_MAX];
    int32_t top;
} NumStateStack;
*/
 
typedef union {
    int32_t intVal;
    float   floatVal;
} numTypeRtrn;

typedef struct {
    TokenType type;
    union {
        int32_t tokenIntVal;
        float   tokenFloatVal;
        char    operator;
        char    identifier[10];
    } value;
} Token;
 
typedef struct {
    const char *input;  /* Input string to tokenize */
    size_t      pos;    /* Current position in input */
} LexerInfo;

/* =======================
    Function Pointer Types
   ======================= */

/* Pointer to top-level state handler */
typedef void (*StateHandler)(LexerInfo *lxer, LexerState *state);

/* =======================
          Prototypes
   ======================= */
 
/* Lexer entry point */
void lexer(const char *inputString);
 
/* Debug helper */
void lexerState(LexerState state);
 
/* Keyword checking */
bool isKeyWord(char *identifier);
 
/* Character helpers */
char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);
 
/* Top-level state handlers */
void startState(LexerInfo *lxer, LexerState *state);
void errState(LexerInfo *lxer, LexerState *state);
void eofState(LexerInfo *lxer, LexerState *state);
 
/* Token handlers */
Token numHandler(LexerInfo *lxer);
Token opHandler(LexerInfo *lxer);
Token identHandler(LexerInfo *lxer);
 
 /* Stack helpers (currently unused) */
/*
void push(NumStateStack *s, NumState st);
void pop(NumStateStack *s);
NumState peekNum(NumStateStack *s);
*/
 
/* =======================
Extern Global Tables (optional)
   ======================= */
 
// extern OpHandler opHandlers[];
// extern NumHandler numHandlers[];
 
#endif /* LEXER_H */
 