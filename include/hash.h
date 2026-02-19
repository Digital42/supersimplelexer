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
    KEYWRD_LET      = 0,
    KEYWRD_BE       = 1,
    KEYWRD_IF       = 2,
    KEYWRD_UNLESS   = 3,
    KEYWRD_WHILE    = 4,
    KEYWRD_UNTIL    = 5,
    KEYWRD_DO       = 6,
    KEYWRD_RETURN   = 7,
    KEYWRD_BREAK    = 8,
    KEYWRD_LOOP     = 9,
    KEYWRD_SWITCH   = 10,
    KEYWRD_CASE     = 11,
    KEYWRD_DEFAULT  = 12,
    KEYWRD_GLOBAL   = 13,
    KEYWRD_STATIC   = 14,
    KEYWRD_MANIFEST = 15,
    KEYWRD_UNKNOWN  = -1
} Keyword;




Keyword perfectHash(const char *keyword, size_t len);
bool lookUp(const char *keyword);
     
#endif /* HASH_H */