/******************************************************************************
 * File:        main.c   
 * Date:        02-11-26
 * 
 * Description: lexer project
 *
 * Notes: header file for the lexer
 ******************************************************************************/

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>   // size_t
#include <stdint.h>   // uint8_t

/* =======================
   Basic Types
   ======================= */

typedef uint8_t bool;
#define true 1
#define false 0


/* =======================
   Token and State Enums
   ======================= */

typedef enum {
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
    TOKEN_EOF,
    TOKEN_ERR
} TokenType;

typedef enum {
    STATE_START,
    STATE_NUMBER,
    STATE_OP,
    STATE_EOF,
    STATE_ERR
} LexerState;

typedef enum {
    OP_START,
    OP_EQ,
    OP_BANG,
    OP_LT,
    OP_GT,
    OP_DONE,
    OP_ERR
} OpState;

typedef enum {
    NUM_START,
    NUM_INT,
    NUM_FLOAT,
    NUM_DONE,
    NUM_ERR
} NumState;


/* =======================
   Structs / Unions
   ======================= */

#define NUM_STACK_MAX 8

typedef struct {
    NumState stack[NUM_STACK_MAX];
    int top;
} NumStateStack;

typedef struct {
    int intPart;
    float fracPart;
    float fracDiv;
    bool isFloat;
} NumCtx;

typedef union {
    int intVal;
    float floatVal;
} numTypeRtrn;

typedef struct {
    TokenType type;
    union {
        int tokenIntVal;
        float tokenFloatVal;
    } value;
} Token;

typedef struct {
    const char *input;
    size_t pos;
} LexerInfo;


/* =======================
   Function Pointer Types
   ======================= */

typedef void (*StateHandler)(LexerInfo *lxer, LexerState *state);
typedef void (*OpHandler)(LexerInfo *lxer, OpState *state, TokenType *tok);
typedef void (*NumHandler)(LexerInfo *lxer, NumState *state, TokenType *tok);


/* =======================
   Public API Prototypes
   ======================= */

void lexer(const char *inputString);
void lexerState(LexerState state);

numTypeRtrn numHandler(LexerInfo *lxer);

/* Character helpers */
char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);

/* Top-level state handlers */
void startState(LexerInfo *lxer, LexerState *state);
void numberState(LexerInfo *lxer, LexerState *state);
void opState(LexerInfo *lxer, LexerState *state);
void errState(LexerInfo *lxer, LexerState *state);
void eofState(LexerInfo *lxer, LexerState *state);

/* Operator sub-state handlers */
void opStart(LexerInfo *lxer, OpState *st, TokenType *out);
void opEq(LexerInfo *lxer, OpState *st, TokenType *out);
void opBang(LexerInfo *lxer, OpState *st, TokenType *out);
void opLt(LexerInfo *lxer, OpState *st, TokenType *out);
void opGt(LexerInfo *lxer, OpState *st, TokenType *out);

/* Number sub-state handlers */
void numStart(LexerInfo *lxer, NumState *st, TokenType *out);
void numInt(LexerInfo *lxer, NumState *st, TokenType *out);
void numFloat(LexerInfo *lxer, NumState *st, TokenType *out);

/* Stack helpers */
void push(NumStateStack *s, NumState st);
void pop(NumStateStack *s);
NumState peekNum(NumStateStack *s);


/* =======================
   Extern Global Tables
   ======================= */

extern OpHandler opHandlers[];
extern NumHandler numHandlers[];

#endif /* LEXER_H */
