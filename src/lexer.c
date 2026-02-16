/******************************************************************************
* File:        lexer.c   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Definition file for the lexer
******************************************************************************/
#include "lexer.h"
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
Token nextToken(LexerInfo *lxer)
{
    char c = peek(lxer);

    if (isspace(c))
    {
        
        Token delimTok = delimHandler(lxer);
        return delimTok;
    }
 
    if (isdigit(c))
    {
        Token numTok = numHandler(lxer);
        return numTok;
    }
 
    if (isalpha(c))
    {
        Token identTok = identHandler(lxer);
        //printf("TOKEN_IDENT: %s\t", identTok.value.identifier);
        //printf("TYPE: %d\n", identTok.type);        
        return identTok;
    } 
    
    if (c == '"')
    {
        Token stringTok = stringHandler(lxer);
        return stringTok;
    }
    
 
    if (c == ';')
    {
        Token semTok = { .type = TOKEN_SEMICOL, .value.value = ';' };
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
        Token lParen = { .type = TOKEN_LPAREN, .value.value = '(' };
        //printf("TOKEN_LPAREN: %c\n", lParen.value);
        advance(lxer);
        return lParen;
    }

    if (c == ')')
    {
        Token rParen = { .type = TOKEN_RPAREN, .value.value = ')' };
        //printf("TOKEN_RPAREN: %c\n", rParen.value);
        advance(lxer);
        return rParen;
    }   

    if (c == '{')
    {
        Token lBrackTok = { .type = TOKEN_LBRACK, .value.value = '{' };
        //printf("TOKEN_LBRACK: %c\n", lBrackTok.value);
        advance(lxer);
        return lBrackTok;
    }

    if (c == '}')
    {
        Token rBrackTok = { .type = TOKEN_RBRACK, .value.value = '}' };
        //printf("TOKEN_LBRACK: %c\n", rBrackTok.value);
        advance(lxer);
        return rBrackTok;
    }
 
    if (c == '\0')
    {
        Token eof = { .type = TOKEN_EOF, .value.value = '\0' };

        return eof;
    }
 
    if (strchr("+-*/=!<>", c))
    {
        Token opTok = opHandler(lxer);
        //printf("TOKEN_OP: %c\n", opTok.value.operator);
        return opTok;
    }
 
    Token err = { .type = TOKEN_ERR, .value.value = 'e' };
    return err;
}
 

 
/* ============================================================
   ===================== TOKEN HANDLERS =======================
   ============================================================ */

/*
    Handles spaces and delimiters
*/

Token delimHandler(LexerInfo *lxer)
{
    int count = 0;
    Token result = {0};
    do
    {
        
        advance(lxer); // skip extra chars beyond buffer
        count++;
    
    } while (isspace(peek(lxer)) && !peekEoF(lxer));
    
    result.type = TOKEN_DELIM;
    result.value.value = count;

    return result;
}

/*
    Handles strings that start and end with "."
*/

Token stringHandler(LexerInfo *lxer)
{
    size_t i = 0;
    Token result = {0};
    //have to advance here to skip the string fix this latter maybe using peekNext?
    advance(lxer);
    //printf("TOKEN_STRING: ");
    while(peek(lxer) != '"' && !peekEoF(lxer))
    {
        if (i < MAX_IDENT_LEN - 1) {
            result.value.identifier[i++] = advance(lxer);
        } else {
            advance(lxer);
        }
    }

    result.value.identifier[i] = '\0';
    //printf("\n");
    //have to advance here to skip the string fix this latter maybe using peekNext?   
    advance(lxer);
    result.type = TOKEN_STRING;
    return result;
}

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
        if (count > MAX_IDENT_LEN - 1)
        {
            printf("Identifier too long, truncating.\n");
            if (isKeyWord(&result))
            {
                result.type = TOKEN_KEYWORD;
                return result;
            }else{
                result.type = TOKEN_IDEN_GENERIC;
                return result;
            }
            
        }
 
        char c = advance(lxer);
        result.value.identifier[count] = c;
        count++;
    } while (!peekEoF(lxer) && (isalpha(peek(lxer)) || isdigit(peek(lxer))));
 
    if (isKeyWord(&result))
    {
        result.type = TOKEN_KEYWORD;
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
 
            result.type = TOKEN_FLOAT;
            result.value.tokenFloatVal = intPart + fracPart;           
            //printf("TOKEN_FLOAT: %.5f\n", result.value.tokenFloatVal);
            return result;
        }
        intPart = intPart * 10 + (c - '0');
    }
 
    if (!isFloat)
    {
        result.type = TOKEN_INT;
        result.value.tokenIntVal = intPart;
        //printf("TOKEN_INT: %d\n", result.value.tokenIntVal);
    }
 
    return result;
}
 
/*
    Checks if an identifier matches a reserved keyword.
*/
bool isKeyWord(const Token *tok)
{
    size_t numElms = sizeof(keyWords) / sizeof(keyWords[0]);
 
    for (size_t i = 0; i < numElms; i++)
    {
        if (strcmp(tok->value.identifier, keyWords[i]) == 0)
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
    Debug utility to print current state name.
*/

void printTokenType(Token tok) {
    switch (tok.type) {
        case TOKEN_IDEN_GENERIC: 
            printf("TOKEN_IDEN_GENERIC\n -> %s\n", tok.value.identifier); 
            break;
        case TOKEN_KEYWORD:      
            printf("TOKEN_KEYWORD\n -> %s\n", tok.value.identifier);
            break;
        case TOKEN_INT:          
            printf("TOKEN_INT\n -> %d\n", tok.value.tokenIntVal);
            break;

        case TOKEN_FLOAT:        
            printf("TOKEN_FLOAT\n -> %.f\n", tok.value.tokenFloatVal);
            break;

        case TOKEN_PLUS:         
            printf("TOKEN_PLUS\n -> %c\n", tok.value.value);
            break;

        case TOKEN_MINUS:        
            printf("TOKEN_MINUS\n -> %c\n", tok.value.operator); 
            break;

        case TOKEN_MUL:          
            printf("TOKEN_MUL\n -> %c\n", tok.value.operator); 
            break;
        case TOKEN_DIV:          
            printf("TOKEN_DIV\n -> %c\n", tok.value.operator); 
            break;

        case TOKEN_ASSIGN:       
            printf("TOKEN_ASSIGN\n -> %c\n", tok.value.operator);
            break;

        case TOKEN_EQUAL:        
            printf("TOKEN_EQUAL\n -> %c\n", tok.value.operator); 
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
            printf("TOKEN_SEMICOL\n -> %c\n", tok.value.value);
            break;
        
        case TOKEN_EOF:          
            printf("TOKEN_EOF\n"); 
            break;

        case TOKEN_ERR:          
            printf("TOKEN_ERR\n"); 
            break;

        case TOKEN_LBRACK:       
            printf("TOKEN_LBRACK\n -> %c\n", tok.value.value);
            break;

        case TOKEN_RBRACK:       
            printf("TOKEN_RBRACK\n -> %c\n", tok.value.value);
            break;

        case TOKEN_LPAREN:
            printf("TOKEN_LPAREN\n -> %c\n", tok.value.value);
            break;

        case TOKEN_RPAREN:       
            printf("TOKEN_RPAREN\n -> %c\n", tok.value.value); 
            break;

        case TOKEN_STRING:       
            printf("TOKEN_STRING\n -> %s\n", tok.value.identifier); 
            break;

        case TOKEN_DELIM:        
            printf("TOKEN_DELIM\n");
            break;

        default:                 
            printf("UNKNOWN_TOKEN\n"); 
            break;
    }
}
