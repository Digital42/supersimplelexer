/******************************************************************************
* File:        main.c
* Date:        02-11-26
*
* Description: Lexer project
*
* Notes: main.c is main :). Right now only the lexer is implimented. Its a 
*		 standard lexer split up with a lexer module that controls and initiates
*		 a reader, scanner, and tokenier to do normal reading, scanning, and 
*  		 tokenizing
******************************************************************************/
#include "lexer.h"
#include "tokenizer.h"

void errorHandler(size_t line, size_t col, const char *msg, void *userData, const char *errChar) {
	if (userData == NULL) {
		printf("No user data supplied\n");
	}
	Span *newSpan = (Span*)userData;
	
	printf("Lexer error at %zu:%zu: %s @ %c -> ", line, col, msg, *errChar);
	printf("%.*s\n", newSpan->length, newSpan->start);
}


int main()
{
	const char *sourceFile = "test.bcpl";
	// initiate lexer
	Lexer lexer;
	Token t;

	// pretty sure this sets up error handler
	if (lexerInitFromFile(&lexer, sourceFile, errorHandler, NULL) == false) {
        printf("Something Went wrong during lexer initiation.\n");
        return 1;
    }
	
	do
	{
		t = lexerNextToken(&lexer);

		/*if (t.type == TOKEN_DELIM_S)
		{
			printf(" space\n");
		}else if (t.type == TOKEN_KEYWORD)
		{
			printf(" keyword\n");
		}else if (t.type == TOKEN_IDEN_GENERIC)
		{
			printf(" identifier\n");
		}else if (t.type == TOKEN_PLUS)
		{
			printf(" token plus\n");
		}else if (t.type == TOKEN_STRING)
		{
			printf(" token string\n");
		}*/
		if (t.type == TOKEN_EOF)
		{
			// do nothing 
		}else if (t.type == TOKEN_ERR)
		{
			printf("Token Error\n");
		}
		
		else{
			printFormatted(t.start, t.length);
		}
		
		
		
		
	} while (t.type != TOKEN_EOF);
	

	// get next token and print for now

	// lexer clean up

	return 0;
}

