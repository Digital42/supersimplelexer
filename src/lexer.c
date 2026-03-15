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

/**
 * lexerInit() - Initialise a Lexer from a null-terminated input string.
 * @lex:      Lexer instance to initialise.
 * @input:    Null-terminated source string to lex.
 * @errorFn:  Error callback invoked on malformed tokens; must not be NULL.
 * @userData: Opaque pointer forwarded to @errorFn.
 *
 * Return: true on success, false if any required argument is NULL.
 */
bool  lexerInit(Lexer *lex, const char *input, TokenErrorCallback errorFn, void *userData)
{
	userData = NULL;
	if (!lex || !input || !errorFn)
	{
		return false;
	}

	readerInit(&lex->reader, input);
	tokenizerInit(&lex->tokenizer, &lex->reader, errorFn, userData);

	return true;
}

/**
 * lexerInitFromFile() - Initialise a Lexer from a source file.
 * @lex:      Lexer instance to initialise.
 * @filename: Path to the source file to lex.
 * @errorFn:  Error callback invoked on malformed tokens; must not be NULL.
 * @userData: Opaque pointer forwarded to @errorFn.
 *
 * Opens and maps @filename via readerInitFromFile().  If the file cannot
 * be opened or read, the function returns false and the Lexer is left
 * uninitialised.
 *
 * Return: true on success, false if any required argument is NULL or if
 *         the file could not be opened.
 */
bool lexerInitFromFile(Lexer *lex, const char *filename, TokenErrorCallback errorFn, void *userData)
{
	userData = NULL;
	if (!lex || !filename || !errorFn){
		return false;
	}
	if(readerInitFromFile(&lex->reader, filename)){
		tokenizerInit(&lex->tokenizer, &lex->reader, errorFn, userData);
		return true;
	}

	return false;
}

/**
 * lexerNextToken() - Return the next token from the input.
 * @lex: Initialised Lexer.
 *
 * Delegates directly to the underlying Tokenizer.  Call repeatedly
 * until the returned token's type is TOKEN_EOF.
 *
 * Return: The next Token in the input stream.
 */
Token lexerNextToken(Lexer *lex)
{
	return tokNextToken(&lex->tokenizer);
}

/**
 * lexerCleanUp() - Release resources held by the Lexer.
 * @lex: Lexer to clean up.  Does nothing if NULL.
 *
 * Destroys the underlying Reader (freeing any file-backed buffer).
 * The Tokenizer holds no resources of its own so it does not need
 * an explicit teardown call.
 */
void lexerCleanUp(Lexer *lex)
{
	if (!lex)
		return;

	readerDestroy(&lex->reader);
}