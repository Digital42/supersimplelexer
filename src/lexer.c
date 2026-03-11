/******************************************************************************
* File:        lexer.c
* Date:        02-11-26
*
* Description: Lexer project
*
* Notes: Definition file for the lexer. Standard lexer that read source file
*        into a buffer and then slices out tokens in the source string. Token
*        are just indexs into the main string. So every toke has a starting
*        position and and ending position in the character array
******************************************************************************/
#include "lexer.h"
#include "reader.h"
#include "hash.h"
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


bool  lexerInit(Lexer *lex, const char *input)
{
	if (!lex || !input)
	{
		return false;
	}

	readerInit(&lex->reader, input);
	// init tokenizer
	//tokenizerInit(&lex->reader, input);

	return true;
}

bool  lexerInitFromFile(Lexer *lex, const char *filename)
{
	if (!lex || !filename){
		return false;
	}

	if(!readerInitFromFile(&lex->reader, filename)){
		return false;
	}

	readerInit(&lex->reader, filename);
	// init tokenizer
	//tokenizerInit(&lex->reader, input);
	return true;

}

void  lexerSetErrorHandler(Lexer *lex, void (*errorFn)(size_t line, size_t col, const char *msg, void *userData, const char *src), void *userData)
{

}

Token lexerNextToken(Lexer *lex)
{
	return tokNextToken(&lex->tokenizer);
}

void  lexerCleanup(Lexer *lex)
{
	if (!lex)
		return;

	readerDestroy(&lex->reader);
	
}