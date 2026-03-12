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
#include <stdbool.h>

#define TOTAL_KEYWORDS 44
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 67

/* =======================
    Basic Types
   ======================= */


#define true  1
#define false 0
#define MAX_IDENT_LEN 10


bool lookUp(const char *keyword, size_t len);
     
#endif /* HASH_H */