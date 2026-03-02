/******************************************************************************
* File:        lexer.c   
* Date:        02-18-26
* 
* Description: Lexer project
*
* Notes: header file for the hash stuff
******************************************************************************/
#ifndef HASH_H
#define HASH_H

#include <stddef.h> 
#include <stdint.h>  

/* =======================
    Basic Types
   ======================= */
 
typedef uint8_t bool;
#define true  1
#define false 0
#define MAX_IDENT_LEN 10

typedef enum {
    KEYWRD_AND = 0,
    KEYWRD_BE,
    KEYWRD_BREAK,
    KEYWRD_BY,
    KEYWRD_CASE,
    KEYWRD_DEFAULT,
    KEYWRD_DO,
    KEYWRD_ELSE,
    KEYWRD_EQ,
    KEYWRD_FALSE,
    KEYWRD_FOR,
    KEYWRD_GE,
    KEYWRD_GET,
    KEYWRD_GLOBAL,
    KEYWRD_GOTO,
    KEYWRD_GR,
    KEYWRD_IF,
    KEYWRD_INTO,
    KEYWRD_LET,
    KEYWRD_LEVEL,
    KEYWRD_LOOP,
    KEYWRD_LS,
    KEYWRD_MANIFEST,
    KEYWRD_NE,
    KEYWRD_NOT,
    KEYWRD_OR,
    KEYWRD_RESULTIS,
    KEYWRD_RESULTS,
    KEYWRD_RETURN,
    KEYWRD_SECTION,
    KEYWRD_STATIC,
    KEYWRD_SWITCH,
    KEYWRD_SWITCHON,
    KEYWRD_TABLE,
    KEYWRD_TEST,
    KEYWRD_THEN,
    KEYWRD_TO,
    KEYWRD_TRUE,
    KEYWRD_UNLESS,
    KEYWRD_UNTIL,
    KEYWRD_VALOF,
    KEYWRD_VEC,
    KEYWRD_WHILE,
    KEYWRD_WRITEF,
    KEYWRD_UNKNOWN = -1
} Keyword;

Keyword perfectHash(const char *keyword, size_t len);
bool lookUp(const char *keyword, size_t len);
     
#endif /* HASH_H */