/******************************************************************************
 * File:        main.c   
 * Date:        02-11-26
 * 
 * Description: lexer project
 *
 * Notes: definition file for the lexer
 ******************************************************************************/

#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>


/* ============================================================
   ============== Function tables for handlers ================
   ============================================================ */

/* Table of top-level state handler */
    StateHandler handlers[] = {
        startState,
        numberState,
        opState,
        eofState,
        errState
    };
    
    /* Table of sub-level operator handler */
OpHandler opHandlers[] = {
    opStart,
    opEq,
    //opBang,
    //opLt,
   // opGt,
    //NULL,
    //NULL
};
    /* Table of sub-level number handler */
NumHandler numHandlers[] = {
    numStart,
    numInt,
    numFloat,
    //NULL,
    //NULL
};

/* ============================================================
   ===================== LEXER ENTRY POINT ====================
   ============================================================ */

/*
    Top-level driver for the lexer state machine.
    Initializes the lexer state and repeatedly dispatches
    to the correct state handler until EOF.
*/
void lexer(const char *inputString)
{
    LexerState state = STATE_START;
    LexerInfo lex = { inputString, 0 };

    /* Main state machine loop */
    while (true)
    {
        handlers[state](&lex, &state);

        if (state == STATE_EOF)
        {
            handlers[STATE_EOF](&lex, &state);
            break;
        }
        else if (state == STATE_ERR)
        {
            handlers[STATE_ERR](&lex, &state);
        }
    }
}


/* ============================================================
   ===================== TOP LEVEL STATES =====================
   ============================================================ */

/*
    Decides what kind of token begins at the current position.
    Transitions into NUMBER, OP, EOF, or ERR states.
*/
void startState(LexerInfo *lxer, LexerState *state)
{
    char c = peek(lxer);

    if (isdigit(c))
    {
        *state = STATE_NUMBER;
    }
    else if (isspace(c))
    {
        advance(lxer);
    }
    else if (c == '\0')
    {
        *state = STATE_EOF;
    }
    else if (strchr("+-*/=!<>", c))
    {
        *state = STATE_OP;
    }
    else
    {
        *state = STATE_ERR;
    }
}


/*
    Handles numeric token parsing by delegating to the
    number sub-state machine.
*/
void numberState(LexerInfo *lxer, LexerState *state)
{
    NumState numState = NUM_START;
    TokenType tok = TOKEN_ERR;

    while (numState != NUM_DONE && numState != NUM_ERR)
    {
        numHandlers[numState](lxer, &numState, &tok);
    }

    if (numState == NUM_ERR)
    {
        printf("TOKEN_ERR at %zu\n", lxer->pos);
    }

    *state = STATE_START;
}


/*
    Handles operator parsing using the operator
    sub-state machine.
*/
void opState(LexerInfo *lxer, LexerState *state)
{
    OpState opState = OP_START;
    TokenType tok = TOKEN_ERR;

    while (opState != OP_DONE && opState != OP_ERR)
    {
        opHandlers[opState](lxer, &opState, &tok);
    }

    if (opState == OP_DONE)
    {
        printf("TOKEN_OP: %d\n", tok);
    }
    else
    {
        printf("TOKEN_ERR at %zu\n", lxer->pos);
    }

    *state = STATE_START;
}


/*
    Handles end-of-file condition.
*/
void eofState(LexerState *state)
{
    printf("EOF\n");
    *state = STATE_EOF;
}


/*
    Handles invalid characters and recovers
    by advancing one character.
*/
void errState(LexerInfo *lxer, LexerState *state)
{
    advance(lxer);
    printf("ERR\n");
    *state = STATE_START;
}


/* ============================================================
   ================= NUMBER SUB-STATE MACHINE =================
   ============================================================ */

/*
    Determines if number begins as integer or float.
*/
void numStart(LexerInfo *lxer, NumState *st, TokenType *out)
{
    char c = peek(lxer);

    if (isdigit(c))
    {
        *st = NUM_INT;
    }
    else
    {
        *st = NUM_ERR;
    }
}


/*
    Parses the integer portion of a number.
    May transition to float state if '.' encountered.
*/
void numInt(LexerInfo *lxer, NumState *st, TokenType *out)
{
    //uint32_t intPart = 0;

    while (isdigit(peek(lxer)))
    {
        if (peekNext(lxer) == '.')
        {
            intPart = intPart * 10 + (peek(lxer) - '0');
            advance(lxer);
            *st = NUM_FLOAT;
            return;
        }

        intPart = intPart * 10 + (peek(lxer) - '0');
        advance(lxer);
    }

    printf("TOKEN_INT: %d\n", intPart);
    *st = NUM_DONE;
    intPart = 0;
}


/*
    Parses fractional portion of a float.
*/
void numFloat(LexerInfo *lxer, NumState *st, TokenType *out)
{
    advance(lxer);  /* skip '.' */
    uint32_t degree = 1;


    while (*st == NUM_FLOAT)
    {
        char c = peek(lxer);
        if (isdigit(c))
        {
            degree *= 10;
            intPart = intPart * 10 + (peek(lxer) - '0');
            //printf("TOKEN_FLOAT: %c\n", c);
            advance(lxer);

        }
        else{
            *st = NUM_DONE;
            printf("TOKEN_FLOAT: %.2f\n", (float)intPart/degree);
            //fix this later right now you have to reset intPart to 0 between tokens
            intPart = 0;
            return;
        }

        //printf("TOKEN_FLOAT\n");
    }
    
    *st = NUM_ERR;
    
    /*if (isdigit(peekNext(lxer)))
    {
        printf("TOKEN_FLOAT?: %c <-\n", peek(lxer));
        *st = NUM_DONE;
    }
    else
    {
        *st = NUM_ERR;
    }*/

}


/*
    Alternative number parser (standalone).
    Parses full integer or float in one pass.
*/
numTypeRtrn numHandler(LexerInfo *lxer)
{
    numTypeRtrn result = {0};
    uint32_t intPart = 0;
    bool isFloat = false;

    while (isdigit(peek(lxer)) || peek(lxer) == '.')
    {
        if (peek(lxer) == '.')
        {
            isFloat = true;
            advance(lxer);

            float fracPart = 0.0f;
            float divisor = 10.0f;

            while (isdigit(peek(lxer)))
            {
                fracPart += (peek(lxer) - '0') / divisor;
                divisor *= 10.0f;
                advance(lxer);
            }

            result.floatVal = intPart + fracPart;
            printf("TOKEN_FLOAT: %.3f\n", result.floatVal);
            return result;
        }

        intPart = intPart * 10 + (peek(lxer) - '0');
        advance(lxer);
    }

    if (!isFloat)
        result.intVal = intPart;

    printf("TOKEN_INT: %d\n", result.intVal);
    return result;
}


/* ============================================================
   ================ OPERATOR SUB-STATE MACHINE ================
   ============================================================ */

/*
    Handles first character of operator.
*/
void opStart(LexerInfo *lxer, OpState *st, TokenType *out)
{
    char c = advance(lxer);

    switch (c)
    {
        case '=': *st = OP_EQ; break;
        case '+': *out = TOKEN_PLUS;  *st = OP_DONE; break;
        case '-': *out = TOKEN_MINUS; *st = OP_DONE; break;
        case '*': *out = TOKEN_MUL;   *st = OP_DONE; break;
        case '/': *out = TOKEN_DIV;   *st = OP_DONE; break;
        default:  *st = OP_ERR;       break;
    }
}


/*
    Handles '=' and '==' tokens.
*/
void opEq(LexerInfo *lxer, OpState *st, TokenType *out)
{
    if (peek(lxer) == '=')
    {
        advance(lxer);
        *out = TOKEN_EQUAL;
    }
    else
    {
        *out = TOKEN_ASSIGN;
    }

    *st = OP_DONE;
}


/* ============================================================
   ===================== CHARACTER HELPERS ====================
   ============================================================ */

/*
    Returns current character and advances position.
*/
char advance(LexerInfo *lxer)
{
    char c = peek(lxer);

    if (c != '\0')
        lxer->pos++;

    return c;
}


/*
    Returns current character without advancing.
*/
char peek(LexerInfo *lxer)
{
    return lxer->input[lxer->pos];
}


/*
    Returns next character without advancing.
*/
char peekNext(LexerInfo *lxer)
{
    if (peek(lxer) == '\0')
        return '\0';

    return lxer->input[lxer->pos + 1];
}


/*
    Checks for end-of-file.
*/
bool peekEoF(LexerInfo *lxer)
{
    return peek(lxer) == '\0';
}


/* ============================================================
   ======================= DEBUG HELPERS ======================
   ============================================================ */

/*
    Debug utility to print current state name.
*/
void lexerState(LexerState state)
{
    switch (state)
    {
        case STATE_START:  printf("State: start\n"); break;
        case STATE_NUMBER: printf("State: number\n"); break;
        case STATE_OP:     printf("State: op\n"); break;
        case STATE_EOF:    printf("State: EOF\n"); break;
        case STATE_ERR:    printf("State: error\n"); break;
        default: break;
    }
}