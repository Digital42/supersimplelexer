/******************************************************************************
* File:        lexer.h   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Header file for the lexer
******************************************************************************/
#ifndef LEXER_H
#define LEXER_H

#include "reader.h"
#include "tokenizer.h"
#include <stdbool.h>

/* =======================
        Lexer Struct
  ======================= */
typedef struct {
	Reader    reader;
	Tokenizer tokenizer;
} Lexer;

bool  lexerInit(Lexer *lex, const char *filename, TokenErrorCallback errorFn, void *userData);
bool  lexerInitFromFile(Lexer *lex, const char *filename, TokenErrorCallback errorFn, void *userData);
Token lexerNextToken(Lexer *lex);
void  lexerCleanUp(Lexer *lex);

#endif /* LEXER_H */