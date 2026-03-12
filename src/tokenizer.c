/******************************************************************************
* File:        tokenizer.c
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: tokenizer strickly deals with figuring out the next token by taking
*        a span from the scanner and classify what token it is
******************************************************************************/

#include "tokenizer.h"
#include "scanner.h"
#include "hash.h"
void tokenizerInit(Tokenizer *tok, Reader *reader, TokenErrorCallback errorFn, void *userData)
{
    tok->reader = reader;
    tok->errorFn = errorFn;
    tok->errorUserData = userData;

	return;
}


Token tokNextToken(Tokenizer *tok)
{
	if (scanIsWhitespace(tok->reader)){
		return delimHandler(tok);}

	//if (scanIsNumberStart(tok->reader))
		//return numHandler(tok);

	if (scanIsIdentifierStart(tok->reader))
		return identHandler(tok);

	//if (scanIsSlash(tok->reader))
		//return (tok);

	//if (scanIsString(tok->reader))
		//return stringHandler(tok);

	//if (scanIsChar(tok->reader))
		//return charHandler(tok);

	if (scanIsEof(tok->reader))
		return eofHandler(tok);
  
	return opHandler(tok);
    //printf("fail\n");
}


Token opHandler(Tokenizer *tok)
{
	Token token = {0};
	Span span = scanOperator(tok->reader);
    char c = *span.start;

	switch (c) {
		case '+':
            token.type = TOKEN_PLUS;
            break;

        default:
            token.type = TOKEN_UNKNOWN;
            break;

    }

    token.start = span.start;
    token.length = span.length;

	return token;
}

/*
Token numHandler(Tokenizer *tok)
{
    Token token = {0};
    return token;   
}

Token opHandler(Tokenizer *tok)
{
    Token token = {0};
    return token;   
}

Token stringHandler(Tokenizer *tok)
{
    Token token = {0};
    return token;   
}


Token charHandler(Tokenizer *tok)
{
    Token token = {0};
    return token;   
}
*/
Token eofHandler(Tokenizer *tok)
{
    Token token = {0};
    token.type = TOKEN_EOF;
    Span span = scanEof(tok->reader);

    token.start = span.start;
    token.length = span.length;
    
    return token;
}


/*
 * Handles spaces and delimiter characters.
 * Returns a single token representing a contiguous block of whitespace.
 */
Token delimHandler(Tokenizer *tok)
{
	Token token = {0};
    char lastChar = '\0';
	Span span = scanWhitespace(tok->reader, &lastChar);
    
	switch (lastChar) {
		case ' ':  token.type = TOKEN_DELIM_S; break;
		case '\t': token.type = TOKEN_DELIM_T; break;
		case '\n': token.type = TOKEN_DELIM_N; break;
		case '\v': token.type = TOKEN_DELIM_V; break;
		case '\f': token.type = TOKEN_DELIM_F; break;
		case '\r': token.type = TOKEN_DELIM_R; break;
        default:   token.type = TOKEN_DELIM_U; break;
    }

    token.start = span.start;
    token.length = span.length;

	return token;
}

Token identHandler(Tokenizer *tok)
{
    Token token = {0};
    Span span = scanIdentifier(tok->reader);

    if (lookUp(span.start, span.length))
        token.type = TOKEN_KEYWORD;
    else
        token.type = TOKEN_IDEN_GENERIC;
    
    return token;
}   

/* ============================================================
   ======================= DEBUG HELPERS ======================
   ============================================================ */

/*
 * Prints the token type and lexeme for debugging purposes.
 */
void printTokenType(Token tok)
{
	const char *name = "INVALID TOKEN PASSED: NO TYPE OR NO LENGTH";

	if (tok.type < TOKEN_COUNT && tok.type >= 0)
		//name = tokenTypeNames[tok.type];

	printf("%-18s", name);

	if (tok.start && tok.length > 0)
		printf(" -> %.*s", (int)tok.length, tok.start);

	printf("\n");
}

void reportTokenError(Tokenizer *tok, const char *msg) {
    if (tok->errorFn) {
        tok->errorFn(ScannerGetLines(tok->reader), ScannerGetCols(tok->reader), msg, tok->errorUserData, NULL);
    }
}