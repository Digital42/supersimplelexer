#include "tokenizer.h"
#include "scanner.h"

Token tokNextToken(Tokenizer *tok)
{
	if (scanIsWhitespace(tok->reader))
		return delimHandler(tok);

	if (scanIsNumberStart(tok->reader))
		return numHandler(tok);

	if (scanIsIdentifierStart(tok->reader))
		return identHandler(tok);

	//if (scanIsSlash(tok->reader))
		//return (tok);

	if (scanIsString(tok->reader))
		return stringHandler(tok);

	if (scanIsChar(tok->reader))
		return charHandler(tok);

	//if (scanIsEof(tok->reader))
	//	return (tok);

	return opHandler(tok);
}


/*
 * Handles spaces and delimiter characters.
 * Returns a single token representing a contiguous block of whitespace.
 */
Token delimHandler(Tokenizer *tok)
{
	Token token = {0};
    char lastChar = 0;
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
