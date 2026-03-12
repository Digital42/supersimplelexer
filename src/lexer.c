/******************************************************************************
* File:        lexer.c
* Date:        02-11-26
*
* Description: Lexer project
*
* Notes: lexer is repossible for initializing the reader and scanner so the
*		the lexer can get the next token and pass it to main so it can be feed
*		to the parser
******************************************************************************/
#include "lexer.h"
#include "tokenizer.h"
#include "reader.h"
#include "hash.h"
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


bool  lexerInit(Lexer *lex, const char *input, TokenErrorCallback errorFn, void *userData)
{
	userData = NULL;
	//printf("not file\n");
	if (!lex || !input || !errorFn)
	{
		return false;
	}

	readerInit(&lex->reader, input);
	// init tokenizer
	tokenizerInit(&lex->tokenizer, &lex->reader, errorFn, userData);

	return true;
}

bool lexerInitFromFile(Lexer *lex, const char *filename, TokenErrorCallback errorFn, void *userData)
{
	userData = NULL;
	//printf("file\n");
	if (!lex || !filename || !errorFn){
		return false;
	}
	//printf("file2\n");
	if(readerInitFromFile(&lex->reader, filename)){
		tokenizerInit(&lex->tokenizer, &lex->reader, errorFn, userData);
		return true;
	}else {
		//printf("file3\n");
	// init tokenizer
	//tokenizerInit(&lex->tokenizer, errorFn, userData);
	//printf("file4\n");
	return false;
	}

}


Token lexerNextToken(Lexer *lex)
{
	//printf("lexer next call\n");
	return tokNextToken(&lex->tokenizer);
}

void lexerCleanUp(Lexer *lex)
{
	if (!lex)
		return;

	readerDestroy(&lex->reader);
	
}