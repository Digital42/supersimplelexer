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

	if (scanIsString(tok->reader))
		return stringHandler(tok);

	if (scanIsChar(tok->reader))
		return charHandler(tok);

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
        
    //printFormatted(token.start, token.length);

	return token;
}

Token stringHandler(Tokenizer *tok)
{
    char delimiter = '"';
	Token token = {0};
	Span span = scanQuotedSequence(tok->reader, delimiter);
    //char c = *span.start;
 
    token.type = TOKEN_STRING;

    for (size_t i = 0; i < span.length; i++){
        char c = span.start[i];
        //printf("string char : %c\n", c);
        if (c == '\n') {
            token.type = TOKEN_ERR;
            tok->errorUserData = &span;
            reportTokenError(tok, span.startCoord.row, span.startCoord.column + i,"Unterminated string", span.start);
            break;
        }else if (c == '\\'){
            if (i >= span.length){
                token.type = TOKEN_ERR;
                reportTokenError(tok, span.endCoord.row, span.endCoord.column, "Unterminated escape sequence in string", span.start);
                break;
            }
            i++;
        } 
    }
    

    token.start = span.start;
    token.length = span.length;

    //printFormatted(token.start, token.length);

    return token;
}

Token charHandler(Tokenizer *tok)
{
    char delimiter = '\'';
	Token token = {0};
	Span span = scanQuotedSequence(tok->reader, delimiter);
    //char c = *span.start;

    token.type = TOKEN_STRING;

    token.start = span.start;
    token.length = span.length;

    //printFormatted(token.start, token.length);

    return token; 
}

/*
Token numHandler(Tokenizer *tok)
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
    
    token.start = span.start;
    token.length = span.length;

    //printFormatted(token.start, token.length);

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
        printf("%-18s", name);


	if (tok.start && tok.length > 0)
		printf(" -> %.*s", (int)tok.length, tok.start);

	printf("\n");
}

void reportTokenError(Tokenizer *tok, size_t line, size_t row, const char *msg, const char *current) {
    if (tok->errorFn) {
        tok->errorFn(line, row, msg, tok->errorUserData, current);
    }
}

// i forgot i already ahd a function that did this delete later if no used
void printFormatted(const char *tokStr, int length) {
    // %. *s tells printf to take an integer for length, then the pointer
    printf("%.*s\n", length, tokStr);
}