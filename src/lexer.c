/******************************************************************************
* File:        lexer.c   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Definition file for the lexer
******************************************************************************/
#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
 
/* =======================
       Keyword List
   ======================= */ 
 
static char *keyWords[] = {
    "int",
    "var",
    "float",
    "if",
    "else",
    "return",
    "while"
};
 
/* ============================================================
   ============== Function Tables for Handlers ==============
   ============================================================ */
 
/* Table of top-level state handlers */
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
    to the appropriate state handler until EOF.
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
    Determines the type of token starting at the current position.
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
 
    if (isalpha(c))
    {
        Token identTok = identHandler(lxer);
        printf("TOKEN_IDENT: %s\t", identTok.value.identifier);
        printf("TYPE: %d\n", identTok.type);        
        return;
    }   
 
    if (c == ';')
    {
        Token semTok = { .type = TOKEN_SEMICOL, .value = ';' };
        printf("TOKEN_SEMICOL: %c\n", semTok.value);
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
    Handles end-of-file condition.
    Currently just prints "EOF".
*/
void eofState(LexerInfo *lxer, LexerState *state)
{ 
    printf("EOF\n");
}
 
/*
    Handles invalid characters.
    Recovers by advancing one character and returning to START state.
*/
void errState(LexerInfo *lxer, LexerState *state)
{
    advance(lxer);
    printf("ERR\n");
    *state = STATE_START;
}
 
/* ============================================================
   ===================== TOKEN HANDLERS =======================
   ============================================================ */
 
/*
    Handles identifiers (alphanumeric tokens starting with a letter).
*/
Token identHandler(LexerInfo *lxer)
{
    Token result = {0};
    uint32_t count = 0;
 
    /* Hard limit of 10 characters for identifier gonna try and experiment 
       with dynamic allocation later                                       */
    do
    {
        if (count > 9)
        {
            printf("Identifier too long, truncating.\n");
            if (isKeyWord(result.value.identifier))
            {
                result.type = TOEKEN_KEYWORD;
                return result;
            }else{
                result.type = TOKEN_IDEN_GENERIC;
                return result;
            }
            
        }
 
        char c = advance(lxer);
        result.value.identifier[count] = c;
        count++;
    } while (isalpha(peek(lxer)) || isdigit(peek(lxer)));
 
    if (isKeyWord(result.value.identifier))
    {
        result.type = TOEKEN_KEYWORD;
        return result;
    }else{
        result.type = TOKEN_IDEN_GENERIC;
        return result;
    }
}
 
/*
    Handles operators: +, -, *, /, =, !, <, >
*/
Token opHandler(LexerInfo *lxer)
{
    Token result = {0};
 
    while (strchr("+-*/=!<>", peek(lxer)))
    {
        char c = advance(lxer);
        switch (c)
        {
            case '+': result.type = TOKEN_PLUS;  result.value.operator = '+'; break;
            case '-': result.type = TOKEN_MINUS; result.value.operator = '-'; break;
            case '*': result.type = TOKEN_MUL;   result.value.operator = '*'; break;
            case '/': result.type = TOKEN_DIV;   result.value.operator = '/'; break;
            case '=': result.type = TOKEN_EQUAL; result.value.operator = '='; break;
            default: break;
        }
    }
 
    return result;
}
 
/*
    Parses a full integer or floating-point number in one pass.
    This avoids over-engineered sub-states for numbers.
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
            intPart = intPart * 10 + (c - '0');
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
            printf("TOKEN_FLOAT: %.5f\n", result.value.tokenFloatVal);
            return result;
        }
        intPart = intPart * 10 + (c - '0');
    }
 
    if (!isFloat)
    {
        result.type = TOKEN_INT;
        result.value.tokenIntVal = intPart;
        printf("TOKEN_INT: %d\n", result.value.tokenIntVal);
    }
 
    return result;
}
 
/*
    Checks if an identifier matches a reserved keyword.
*/
bool isKeyWord(char *identifier)
{
    size_t numElms = sizeof(keyWords) / sizeof(keyWords[0]);
 
    for (size_t i = 0; i < numElms; i++)
    {
        if (strcmp(identifier, keyWords[i]) == 0)
        {
            return true;
        }
    }
 
    return false; 
}
 
/* ============================================================
   ===================== CHARACTER HELPERS ====================
   ============================================================ */
 
/*
    Returns current character and advances the position.
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
 