/******************************************************************************
* File:        lexer.c   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Definition file for the lexer. Standard lexer that read source file 
         into a buffer and then slices out tokens in the source string. Token
         are just indexs into the main string. So every toke has a starting 
         position and and ending position in the character array
******************************************************************************/
#include "lexer.h"
#include "hash.h"
// TODO: Fix double include of stdio
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

/* ============================================================
   ===================== LEXER FUNCTIONS  =====================
   ============================================================ */

/*
    Frees a lexer structure allocated via lexerCreate.
*/
void lexerDestroy(LexerInfo *lex)
{
    free(lex);
}

/*
    Creates a lexer from a given input string.
    Returns a pointer to a LexerInfo structure.
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

/*
    Creates a lexer from the contents of a file.
    Reads the entire file into memory, null-terminates it,
    and returns a LexerInfo pointer.
*/
LexerInfo *lexerCreateFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    // Determine file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Allocate buffer for file contents + null terminator
    char *buffer = malloc(fileSize + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';  // null-terminate
    fclose(file);

    // Use lexerCreate to initialize lexer
    LexerInfo *lex = lexerCreate(buffer);
    return lex;
}
 
/* ============================================================
   +====================  TOKEN DRIVERS  ====================== 
   ============================================================ */

/*
    Returns the next token from the input.
    Dispatches to appropriate handlers based on character type.
*/
Token nextToken(LexerInfo *lxer)
{
    Token tok = {0};
    char c = peek(lxer);

    // Whitespace or delimiter
    if (isspace(c))
    {
        tok = delimHandler(lxer);
        return tok;
    }

    // Numeric literal (int or float)
    if (isdigit(c) || (c == '.' && isdigit(peekNext(lxer))))
    {
        Token numTok = numHandler(lxer);
        return numTok;
    }

    // Identifier or keyword
    if (isalpha(c))
    {
        Token identTok = identHandler(lxer);
        return identTok;
    } 
    
    // Single-character or multi-character tokens
    switch (c)
    {
        case '\0':
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_EOF;
            tok.length = 1;
            advance(lxer);
            return tok;

        case '(':
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_LPAREN;
            tok.length = 1;
            advance(lxer);
            return tok;

        case ')': 
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_RPAREN;
            tok.length = 1;
            advance(lxer);
            return tok;

        case '{': 
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_LBRACE;
            tok.length = 1;
            advance(lxer);
            return tok;

        case '}': 
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_RBRACE;
            tok.length = 1;
            advance(lxer);
            return tok;

        case ';':
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_SEMICOL;
            tok.length = 1;
            advance(lxer);
            return tok;

        case '+':
            return opHandler(lxer);

        case '=':
            return opHandler(lxer);

        case '*':
            return opHandler(lxer);

        case '/':
            return opHandler(lxer);

        case '<':
            return opHandler(lxer);

        case '>':
            return opHandler(lxer);

        case '"':
            tok = stringHandler(lxer);
            return tok;

        default:
            tok.start = lxer->input + lxer->pos;
            tok.type = TOKEN_ERR;
            tok.length = 1;
            advance(lxer);
            return tok;
    }
}

/* ============================================================
   ===================== TOKEN HANDLERS =======================
   ============================================================ */

/*
    Handles spaces and delimiter characters.
    Returns a single token representing a contiguous block of whitespace.
*/
Token delimHandler(LexerInfo *lxer)
{
    Token tok = {0};
    tok.start = lxer->input + lxer->pos;
    tok.length = 0;
    char c = peek(lxer);

    do
    {
        switch (c) {
            case ' ':  tok.type = TOKEN_DELIM_S; break;
            case '\t': tok.type = TOKEN_DELIM_T; break;            
            case '\n': tok.type = TOKEN_DELIM_N; break; 
            case '\v': tok.type = TOKEN_DELIM_V; break;
            case '\f': tok.type = TOKEN_DELIM_F; break; 
            case '\r': tok.type = TOKEN_DELIM_R; break;
            default: tok.type = TOKEN_DELIM_U; break;
        }

        tok.length++;
        advance(lxer);
    
    } while (isspace(peek(lxer)) && !peekEoF(lxer));

    return tok;
}

/*
    Handles string literals enclosed in double quotes.
    Supports escape sequences like \" inside strings.
*/
Token stringHandler(LexerInfo *lxer)
{
    Token tok = {0};
    tok.start = lxer->input + lxer->pos;
    tok.length = 1;
    tok.type = TOKEN_STRING;
    char c = advance(lxer);
    
    do
    {
        if (peek(lxer) == '\0')
        {
            // Unterminated string; return as-is
            return tok;
        }
        
        tok.length++;
        c = advance(lxer);

        if (c == '\\')
        {
            tok.length++;
            advance(lxer);
            if (peek(lxer) == '"')
            {
                tok.length++;
                advance(lxer);
            }
        }
    } while (c != '"');

    return tok;
}

/*
    Handles identifiers and keywords.
    Identifiers start with a letter and may contain digits.
    Returns TOKEN_KEYWORD if found in keyword table.
*/
Token identHandler(LexerInfo *lxer)
{
    Token tok = {0};
    tok.start = lxer->input + lxer->pos;
    tok.length = 0;
 
    while (!peekEoF(lxer) && (isalpha(peek(lxer)) || isdigit(peek(lxer))))
    {
        tok.length++;
        advance(lxer);
    }
  
    if (lookUp(tok.start))
        tok.type = TOKEN_KEYWORD;
    else
        tok.type = TOKEN_IDEN_GENERIC;

    return tok;
}

/*
    Handles single- and multi-character operators like +, ++, +=, *, /, etc.
*/
Token opHandler(LexerInfo *lxer)
{
    Token tok = {0};
    tok.start = lxer->input + lxer->pos;
    tok.length = 1;

    char c = advance(lxer); // consume current character

    switch (c) {
        case '+':
            if (peek(lxer) == '+') { advance(lxer); tok.length++; tok.type = TOKEN_INCR; }
            else if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_PLUS_EQ; }
            else tok.type = TOKEN_PLUS;
            break;

        case '-':
            if (peek(lxer) == '-') { advance(lxer); tok.length++; tok.type = TOKEN_DECR; }
            else if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_MINUS_EQ; }
            else tok.type = TOKEN_MINUS;
            break;

        case '*':
            if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_MUL_EQ; }
            else tok.type = TOKEN_MUL;
            break;

        case '/':
            if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_DIV_EQ; }
            else tok.type = TOKEN_DIV;
            break;

        case '%':
            tok.type = TOKEN_MOD;
            break;

        case '=':
            if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_EQ_EQ; }
            else tok.type = TOKEN_ASSIGN;
            break;

        case '<':
            if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_LTE; }
            else tok.type = TOKEN_LT;
            break;

        case '>':
            if (peek(lxer) == '=') { advance(lxer); tok.length++; tok.type = TOKEN_GTE; }
            else tok.type = TOKEN_GT;
            break;

        default:
            tok.type = TOKEN_UNKNOWN;
            break;
    }

    return tok;
}

/*
    Handles integer and floating-point literals in one pass.
    later on the string thats been index can use stdlib funcs
    to safely convert strings to ints or floats no need for me
    to roll my own
*/
Token numHandler(LexerInfo *lxer)
{
    Token tok = {0};
    tok.start = lxer->input + lxer->pos;
    tok.length = 0;
    tok.type = TOKEN_INT;
    bool isFloat = false;

    while (!peekEoF(lxer))
    {
        char c = peek(lxer);
        if (isdigit(c))
        {
            tok.length++;
            advance(lxer);
        } else if (c == '.' && !isFloat)
        {
            isFloat = true;
            tok.type = TOKEN_FLOAT;
            tok.length++;
            advance(lxer);
        } else {
            break;
        }
    }

    return tok;
}

/* ============================================================
   =========== LEXER POSITION CHARACTER HELPERS ===============
   ============================================================ */

/*
    Returns the current character and advances the lexer position.
*/
char advance(LexerInfo *lxer)
{
    if (peekEoF(lxer)) return '\0';
    return lxer->input[lxer->pos++];
}

/*
    Returns the current character without advancing the lexer.
*/
char peek(LexerInfo *lxer)
{
    return lxer->input[lxer->pos];
}

/*
    Returns the next character without advancing the lexer.
*/
char peekNext(LexerInfo *lxer)
{
    if (peek(lxer) == '\0') return '\0';
    return lxer->input[lxer->pos + 1];
}

/*
    Returns true if the lexer has reached the end of the input.
*/
bool peekEoF(LexerInfo *lxer)
{
    return peek(lxer) == '\0';
}

/* ============================================================
   ======================= DEBUG HELPERS ======================
   ============================================================ */

/*
    Prints the token type and lexeme for debugging purposes.
*/
void printTokenType(Token tok)
{
    const char *name = "INVALID TOKEN PASSED: NO TYPE OR NO LENGTH";

    if (tok.type < TOKEN_COUNT && tok.type >= 0)
        name = tokenTypeNames[tok.type];

    printf("%-18s", name);

    if (tok.start && tok.length > 0)
        printf(" -> %.*s", (int)tok.length, tok.start);

    printf("\n");
}