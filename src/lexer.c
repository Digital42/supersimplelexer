/******************************************************************************
* File:        lexer.c   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Definition file for the lexer
******************************************************************************/
#include "lexer.h"
#include "hash.h"
//fix this later this double indluce od stdio
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
 
/* =======================
     BCPL Keyword List
   ======================= */ 
 
static char *keyWords[] = {
    "LET",
    "BE",
    "IF",
    "UNLESS",
    "WHILE",
    "UNTIL",
    "DO",
    "RETURN",
    "BREAK",
    "LOOP",
    "SWITCH",
    "CASE",
    "DEFAULT",
    "GLOBAL",
    "STATIC",
    "MANIFEST"
};
 

 
/* ============================================================
   =====================   CREATE LEXER   =====================
   ============================================================ */
 
/*
   Sandard create lexer return pointer to lexer structure 
*/

Token *createToken(TokenType type)
{
    Token *tok = malloc(sizeof(Token));
    memset(tok, 0, sizeof(Token));

    if (!tok)
    {
        return NULL;
    }
    
    switch(type) {
        case TOKEN_IDEN_GENERIC:
            tok->type = type;
            tok->stringVal = NULL;
            return tok;
    
        case TOKEN_KEYWORD:
            tok->type = type;
            tok->stringVal = NULL;
            return tok;
    
        case TOKEN_INT:
            tok->type = type;
            tok->intVal = 0;
            return tok;
    
        case TOKEN_FLOAT:
            tok->type = type;
            tok->floatVal = 0;
            return tok;
    
        case TOKEN_PLUS:
            // Handle plus operator '+'
            return (Token*) &(const Token){ .type = TOKEN_SEMICOL, .value = ';' };
    
        case TOKEN_MINUS:
            // Handle minus operator '-'
            return (Token*) &(const Token){ .type = TOKEN_MINUS, .value = '-' };
    
        case TOKEN_MUL:
            // Handle multiplication operator '*'
            return (Token*) &(const Token){ .type = TOKEN_MUL, .value = '*' };
    
        case TOKEN_DIV:
            // Handle division operator '/'
            return (Token*) &(const Token){ .type = TOKEN_DIV, .value = '/' };
    
        case TOKEN_ASSIGN:
            // Handle assignment operator '='
            return (Token*) &(const Token){ .type = TOKEN_ASSIGN, .value = '=' };
    
        case TOKEN_EQUAL:
            // Handle equality operator '=='
            return (Token*) &(const Token){ .type = TOKEN_EQUAL, .value = '=' };
    
        case TOKEN_NOTEQ:
            // Handle not-equal operator '!='
            break;
    
        case TOKEN_LT:
            // Handle less-than operator '<'
            break;
    
        case TOKEN_LTE:
            // Handle less-than-or-equal operator '<='
            break;
    
        case TOKEN_GT:
            // Handle greater-than operator '>'
            break;
    
        case TOKEN_GTE:
            // Handle greater-than-or-equal operator '>='
            break;
    
        case TOKEN_SEMICOL:
            // Handle semicolon ';'
            return (Token*) &(const Token){ .type = TOKEN_SEMICOL, .value = ';' };
    
        case TOKEN_EOF:
            // Handle end-of-file
            return (Token*) &(const Token){ .type = TOKEN_EOF, .value = '\0' };
    
        case TOKEN_ERR:
            return (Token*) &(const Token){ .type = TOKEN_ERR, .value = 'e' };

    
        case TOKEN_LBRACE:
            return (Token*) &(const Token){ .type = TOKEN_LBRACE, .value = '{' };
    
        case TOKEN_RBRACE:
            // Handle right brace '}'
            return (Token*) &(const Token){ .type = TOKEN_RBRACE, .value = '}' };
    
        case TOKEN_LPAREN:
            // Handle left parenthesis '('
            return (Token*) &(const Token){ .type = TOKEN_LPAREN, .value = '(' };
    
        case TOKEN_RPAREN:
            // Handle right parenthesis ')'
            return (Token*) &(const Token){ .type = TOKEN_RPAREN, .value = ')' };
    
        case TOKEN_STRING:
            tok->type = type;
            tok->stringVal = NULL;
            return tok;

    
        case TOKEN_DELIM:
            return (Token*) &(const Token){ .type = TOKEN_DELIM, .value = ' ' };

        default:
            return (Token*) &(const Token){ .type = TOKEN_UNKOWN, .value = '?' };
    }
}

LexerInfo *lexerCreate(const char *inputString)
{
    LexerInfo *lex = malloc(sizeof(LexerInfo));
    if (!lex)
    {
        return NULL;
    }
    
    lex->input = inputString;
    lex->pos = 0;

    return lex;
 
}

LexerInfo *lexerCreateFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    // Seek to end to get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate buffer to hold the file + null terminator
    char *buffer = malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // Read file into buffer
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';  // null-terminate
    fclose(file);

    // Use existing lexerCreate to initialize lexer
    LexerInfo *lex = lexerCreate(buffer);
    return lex;
}
 
/* ============================================================
   =====================  Get next token  ===================== 
   ============================================================ */

/*
    Determines the type of token starting at the current position.
    Transitions into NUMBER, OP, EOF, or ERR states.
*/
Token *nextToken(LexerInfo *lxer)
{
    char c = peek(lxer);

    if (isspace(c))
    {
        
        Token *delimTok = delimHandler(lxer);
        return delimTok;
    }
 
    if (isdigit(c))
    {
        Token *numTok = numHandler(lxer);
        return numTok;
    }
 
    if (isalpha(c))
    {
        Token *identTok = identHandler(lxer);
        //printf("TOKEN_IDENT: %s\t", identTok.value.identifier);
        //printf("TYPE: %d\n", identTok.type);        
        return identTok;
    } 
    
    if (c == '"')
    {
        Token *stringTok = stringHandler(lxer);
        return stringTok;
    }
    
 
    if (c == ';')
    {
        Token *semTok = createToken(TOKEN_SEMICOL);
        //printf("TOKEN_SEMICOL: %c\n", semTok.value);
        advance(lxer);
        return semTok;
    }
    

    /*if (c == '\n')
    {
        //keep track of new lines for some reason later
        advance(lxer);
        return;
    }*/

    if (c == '(')
    {
        Token *lParen = createToken(TOKEN_LPAREN);
        //printf("TOKEN_LPAREN: %c\n", lParen.value);
        advance(lxer);
        return lParen;
    }

    if (c == ')')
    {
        Token *rParen = createToken(TOKEN_RPAREN);
        //printf("TOKEN_LPAREN: %c\n", lParen.value);
        advance(lxer);
        return rParen;
    }   

    if (c == '{')
    {
        Token *lBraceTok = createToken(TOKEN_LBRACE);
        //printf("TOKEN_LBRACK: %c\n", lBrackTok.value);
        advance(lxer);
        return lBraceTok;
    }

    if (c == '}')
    {
        Token *rBraceTok = createToken(TOKEN_RBRACE);
        //printf("TOKEN_LBRACK: %c\n", lBrackTok.value);
        advance(lxer);
        return rBraceTok;
    }
 
    if (c == '\0')
    {
        Token *eof = createToken(TOKEN_EOF);

        return eof;
    }
 
    if (strchr("+-*/=!<>", c))
    {
        Token *opTok = opHandler(lxer);
        //printf("TOKEN_OP: %c\n", opTok.value.operator);
        return opTok;
    }
 
    Token *err = createToken(TOKEN_ERR);
    return err;
}
 

 
/* ============================================================
   ===================== TOKEN HANDLERS =======================
   ============================================================ */

/*
    Handles spaces and delimiters
*/

Token *delimHandler(LexerInfo *lxer)
{
    int count = 0;
    Token *tok = createToken(TOKEN_DELIM); 
    do
    {
        
        advance(lxer); // skip extra chars beyond buffer
        count++;
    
    } while (isspace(peek(lxer)) && !peekEoF(lxer));
    
    tok->value = count;

    return tok;
}

/*
    Handles strings that start and end with "."
*/

Token *stringHandler(LexerInfo *lxer)
{
    size_t buffer = 64;
    size_t i = 0;
    Token *tok = createToken(TOKEN_STRING);
    tok->stringVal = malloc(buffer);
    //have to advance here to skip the string fix this latter maybe using peekNext?
    advance(lxer);
    //printf("TOKEN_STRING: ");
    while(peek(lxer) != '"' && !peekEoF(lxer))
    {
        if (i+1 >= buffer) {
            buffer *= 2;
            char *newBuf = realloc(tok->stringVal, buffer);
            if (!newBuf)
            {
                free(tok->stringVal); 
                free(tok); 
                return NULL;                 
            }
            
            tok->stringVal = newBuf;
        }
        tok->stringVal[i++] = advance(lxer);
    }

    tok->stringVal[i] = '\0';
    //printf("\n");
    advance(lxer);
    //have to advance here to skip the string fix this latter maybe using peekNext?   
  

    return tok;
}

/*
    Handles identifiers (alphanumeric tokens starting with a letter).
*/

Token *identHandler(LexerInfo *lxer)
{
    size_t buffer = 32;
    Token *tok = createToken(TOKEN_IDEN_GENERIC);
    tok->stringVal = malloc(buffer);
    size_t i = 0;

    if (!tok)
    {
        return NULL;
    }

    while(!peekEoF(lxer) && (isalpha(peek(lxer)) || isdigit(peek(lxer))))
    {
        if (i+1 >= buffer) {
            buffer *= 2;
            char *newBuf = realloc(tok->stringVal, buffer);
            if (!newBuf)
            {
                free(tok->stringVal); 
                free(tok); 
                return NULL;                 
            }
            
            tok->stringVal = newBuf;
        }
        tok->stringVal[i++] = advance(lxer);
    }

    tok->stringVal[i] = '\0';
    //printf("\n");
    //advance(lxer);
    //have to advance here to skip the string fix this latter maybe using peekNext?   
  
    if (lookUp(tok->stringVal))
    {
        tok->type = TOKEN_KEYWORD;
    }else{
        tok->type = TOKEN_IDEN_GENERIC;
    }
    
    return tok;
    
}
 
/*
    Handles operators: +, -, *, /, =, !, <, >
*/
Token *opHandler(LexerInfo *lxer)
{
    
 
    while (strchr("+-*/=!<>", peek(lxer)))
    {
        Token *tok;
        char c = advance(lxer);
        switch (c)
        {
            case '+': tok = createToken(TOKEN_PLUS);    return tok;

            case '-': tok = createToken(TOKEN_MINUS);   return tok;

            case '*': tok = createToken(TOKEN_MUL);     return tok;   

            case '/': tok = createToken(TOKEN_DIV);     return tok;

            case '=': tok = createToken(TOKEN_ASSIGN);  return tok;

            default:  tok = createToken(TOKEN_ERR);     return tok;
        }
    }
}
 
/*
    Parses a full integer or floating-point number in one pass.
    This avoids over-engineered sub-states for numbers.
*/
Token *numHandler(LexerInfo *lxer)
{
    Token *tok = createToken(TOKEN_INT);
    uint32_t intPart = 0;
    bool isFloat = false;
 
    while (!peekEoF(lxer) && (isdigit(peek(lxer)) || peekNext(lxer) == '.'))
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
 
            tok->type = TOKEN_FLOAT;
            tok->floatVal = intPart + fracPart;           
            //printf("TOKEN_FLOAT: %.5f\n", result.value.tokenFloatVal);
            return tok;
        }
        intPart = intPart * 10 + (c - '0');
    }
 
    if (!isFloat)
    {
        tok->type = TOKEN_INT;
        tok->intVal = intPart;
        //printf("TOKEN_INT: %d\n", result.value.tokenIntVal);
    }
 
    return tok;
}
 
 
/* ============================================================
   ===================== CHARACTER HELPERS ====================
   ============================================================ */
 
/*
    Returns current character and advances the position.
*/
char advance(LexerInfo *lxer)
{
    if (peekEoF(lxer))
    {
        return '\0';
    }

    return lxer->input[lxer->pos++];
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
    Debug utility to print current state name. delete as they 
    are no longer needed
*/

void printTokenType(Token *tok) {
    switch (tok->type) {
        case TOKEN_IDEN_GENERIC: 
            printf("TOKEN_IDEN_GENERIC\n -> %s\n", tok->stringVal); 
            break;
        case TOKEN_KEYWORD:      
            printf("TOKEN_KEYWORD\n -> %s\n", tok->stringVal);
            break;
        case TOKEN_INT:          
            printf("TOKEN_INT\n -> %d\n", tok->intVal);
            break;

        case TOKEN_FLOAT:        
            printf("TOKEN_FLOAT\n -> %.f\n", tok->floatVal);
            break;

        case TOKEN_PLUS:         
            printf("TOKEN_PLUS\n -> %c\n", tok->value);
            break;

        case TOKEN_MINUS:        
            printf("TOKEN_MINUS\n -> %c\n", tok->value); 
            break;

        case TOKEN_MUL:          
            printf("TOKEN_MUL\n -> %c\n", tok->value); 
            break;
        case TOKEN_DIV:          
            printf("TOKEN_DIV\n -> %c\n", tok->value); 
            break;

        case TOKEN_ASSIGN:       
            printf("TOKEN_ASSIGN\n -> %c\n", tok->value);
            break;

        case TOKEN_EQUAL:        
            printf("TOKEN_EQUAL\n -> %c\n", tok->value); 
            break;
        case TOKEN_NOTEQ:        
            printf("TOKEN_NOTEQ\n"); 
            break;

        case TOKEN_LT:           
            printf("TOKEN_LT\n"); 
            break;

        case TOKEN_LTE:          
            printf("TOKEN_LTE\n");
            break;

        case TOKEN_GT:           
            printf("TOKEN_GT\n");
            break;

        case TOKEN_GTE:          
            printf("TOKEN_GTE\n"); 
            break;

        case TOKEN_SEMICOL:      
            printf("TOKEN_SEMICOL\n -> %c\n", tok->value);
            break;
        
        case TOKEN_EOF:          
            printf("TOKEN_EOF\n"); 
            break;

        case TOKEN_ERR:          
            printf("TOKEN_ERR\n"); 
            break;

        case TOKEN_LBRACE:       
            printf("TOKEN_LBRACK\n -> %c\n", tok->value);
            break;

        case TOKEN_RBRACE:       
            printf("TOKEN_RBRACK\n -> %c\n", tok->value);
            break;

        case TOKEN_LPAREN:
            printf("TOKEN_LPAREN\n -> %c\n", tok->value);
            break;

        case TOKEN_RPAREN:       
            printf("TOKEN_RPAREN\n -> %c\n", tok->value); 
            break;

        case TOKEN_STRING:       
            printf("TOKEN_STRING\n -> %s\n", tok->stringVal); 
            break;

        case TOKEN_DELIM:        
            printf("TOKEN_DELIM\n");
            break;

        default:                 
            printf("UNKNOWN_TOKEN\n"); 
            break;
    }
}
