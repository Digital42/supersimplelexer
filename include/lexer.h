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

typedef struct {
	Reader    reader;
	Tokenizer tokenizer;
} Lexer;

bool  lexerInit(Lexer *lex, const char *input);
bool  lexerInitFromFile(Lexer *lex, const char *filename);
void  lexerSetErrorHandler(Lexer *lex,
			   void (*errorFn)(size_t line, size_t col,
					   const char *msg,
					   void *userData,
					   const char *src),
			   void *userData);
Token lexerNextToken(Lexer *lex);
void  lexerCleanup(Lexer *lex);

#endif