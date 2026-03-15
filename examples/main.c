/******************************************************************************
* File:        main.c
* Date:        02-11-26
*
* Description: Lexer project
*
* Notes: main.c is main :). Right now only the lexer is implimented. Its a 
*		 standard lexer split up between a lexer module that controls and 
*		 initiates a reader, scanner, and tokenier to do normal reading,  
*  		 scanning, and tokenizing
******************************************************************************/
#include "lexer.h"
#include "tokenizer.h"

void errorHandler(size_t line, size_t col, const char *msg, void *userData, const char *errChar) {
	if (userData == NULL) {
		printf("No user data supplied\n");
	}

	// gotta cast the void pointer to a span object this is likly bad pratice and needs to be fixed
	Span *newSpan = (Span*)userData;
	
	printf("Lexer error at %zu:%zu: %s @ %c -> ", line, col, msg, *errChar);
	//printf("%.*s\n", newSpan->length, newSpan->start);
	//fwrite avoids having to cast newSpan->length to an int
	fwrite(newSpan->start, 1, newSpan->length, stdout);
	putchar('\n');
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
	
	// get next token and print for now
	do
	{
		t = lexerNextToken(&lexer);

		printf("Token type: %d\n", t.type);
		printFormatted(t.start, t.length);
	
	} while (t.type != TOKEN_EOF);
	

	// lexer clean up TODO: make sure you deallocate the buffer for the source string 
	//if the reader owns it and set all the other structs fields to null just incase

	return 0;
}

