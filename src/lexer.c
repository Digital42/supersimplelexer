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
        eofState,
        errState
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
        Token numTok = numHandler(lxer);
        return;
    }

    if (c == ';')
    {

        Token numTok = {.type = TOKEN_SEMICOL, .value = ';'};
        printf("TOKEN_SEMICOL: %c\n", numTok.value);
        advance(lxer);
        return;
    }

    if (isspace(c))
    {
        advance(lxer);
        return;
    }
    
    if (c == '\0')
    {
        *state = STATE_EOF;
        return;
    }
    
    if (strchr("+-*/=!<>", c))
    {
        Token opTok = opHandler(lxer);
        printf("TOKEN_OP: %c\n", opTok.value.operator);
        return;

    }
 
    *state = STATE_ERR;
    return;

}


/*
    Handles end-of-file condition. ill probably take this out and handle end of file inside if logic
*/
void eofState(LexerInfo *lxer, LexerState *state)
{
    printf("EOF\n");
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

Token opHandler(LexerInfo *lxer)
{
    Token result = {0};

    while (strchr("+-*/=!<>", peek(lxer)))
    {
        char c = advance(lxer);
        switch (c)
        {
        case '+':
            result.type = TOKEN_PLUS;
            result.value.operator = '+';
            break;

        case '-':
            result.type = TOKEN_MINUS;
            result.value.operator = '-';
            break;

        case '*':
            result.type = TOKEN_MUL;
            result.value.operator = '*';
            break;

        case '/':
            result.type = TOKEN_DIV;
            result.value.operator = '/';
            break;            
            
        default:
            break;
        }
    }
    return result;

}
/*
    Parses full integer or float in one pass. This is much vetter then the over engineer nonsese with
    high level states and sub states for every part of a number 
*/
Token numHandler(LexerInfo *lxer)
{
    Token result = {0};
    uint32_t intPart = 0;
    bool isFloat = false;


    while (isdigit(peek(lxer)) || peekNext(lxer) == '.')
    {
        char c = advance(lxer);

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

            result.type = TOKEN_FLOAT;
            result.value.tokenFloatVal = intPart + fracPart;
            printf("TOKEN_FLOAT: %.3f\n", result.value.tokenFloatVal);
            return result;
        }

        intPart = intPart * 10 + (c - '0');
    }

    if (!isFloat)
    {
        result.type = TOKEN_INT;
        result.value.tokenIntVal = intPart;
    }

    printf("TOKEN_INT: %d\n", result.value.tokenIntVal);
    return result;
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

        case STATE_EOF:    printf("State: EOF\n"); break;
        case STATE_ERR:    printf("State: error\n"); break;
        default: break;
    }
}