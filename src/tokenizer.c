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
/* ============================================================
 * Initialisation
 * ============================================================ */
 
/**
 * tokenizer_init() - Initialise a Tokenizer instance.
 * @tok:      Pointer to the Tokenizer to initialise.
 * @reader:   Reader that wraps the source character stream.
 * @error_fn: Callback invoked whenever a malformed token is detected.
 *            May be NULL if error reporting is not required.
 * @userdata: Opaque pointer forwarded verbatim to every @error_fn call.
 *
 * The caller is responsible for ensuring that @reader remains valid for
 * the entire lifetime of @tok.
 */
void tokenizerInit(Tokenizer *tok, Reader *reader, TokenErrorCallback errorFn, void *userData)
{
    tok->reader = reader;
    tok->errorFn = errorFn;
    tok->errorUserData = userData;

	return;
}

/**
 * tok_next_token() - Consume and return the next token from the input.
 * @tok: Initialised Tokenizer.
 *
 * Leading whitespace is discarded before each token is classified.
 * The function dispatches to the appropriate handler based on the first
 * character(s) peeked from the underlying Reader.
 *
 * Return: The next Token.  On end-of-file, a TOKEN_EOF token is returned.
 *         On an unrecognised sequence, behaviour is currently undefined
 *         (the function falls off the end); callers should ensure the
 *         input is well-formed.
 */
Token tokNextToken(Tokenizer *tok)
{
	if (scanIsWhitespace(tok->reader)){
		delimHandler(tok);
    }

	//if (scanIsNumberStart(tok->reader))
		//return numHandler(tok);

	if (scanIsIdentifierStart(tok->reader)) {
		return identHandler(tok);
    }

	if (scanIsComment(tok->reader)) {
		return commentHandler(tok);
    }

	if (scanIsString(tok->reader)) {

		return stringHandler(tok);
    }

	if (scanIsChar(tok->reader)) {
		return charHandler(tok);
    }

	if (scanIsEof(tok->reader)) {
		return eofHandler(tok);
    }

    if (isOperator(tok->reader)) {
		return opHandler(tok);
    }

    // just adding this so the compiler stops complainign about control reaching end of non void function
	Token token = {0};
    return token;
  
}

/* ============================================================
 *                         Token handlers
 * ============================================================ */
 
/**
 * commentHandler() - Consume a comment and return a TOKEN_COMMENT token.
 * @tok: Initialised Tokenizer positioned at the start of a comment.
 *
 * Delegates to scanCommentSequence() for the actual character scanning.
 *
 * Return: A TOKEN_COMMENT token whose @start and @length describe the
 *         comment lexeme (including any comment delimiters).
 */
Token commentHandler(Tokenizer *tok)
{
	Token token = {0};
	Span span = scanCommentSequence(tok->reader);
    //char c = *span.start;
    token.type = TOKEN_COMMENT;
    token.start = span.start;
    token.length = span.length;

    //printFormatted(span.start, span.length);
    
    return token;
}

/**
 * opHandler() - Consume an operator sequence and return a token.
 * @tok: Initialised Tokenizer positioned at the start of an operator.
 *
 * Return: A token whose @start and @length describe the operator lexeme.
 *
 * TODO: Classify operators into distinct TOKEN_OP_* types.
 */
Token opHandler(Tokenizer *tok)
{
	Token token = {0};
	Span span = scanOperator(tok->reader);
    char c = *span.start;

    switch (c) {
 
        /* Single-character operators — consume and stop. */
        case '+':
            token.type = TOKEN_PLUS;
            break;

        case '*':
            token.type = TOKEN_MUL;
            break;
                
        case '/':
            token.type = TOKEN_DIV;
            break;

        case '%':
            token.type = TOKEN_MOD;
            break;

        case '@':
            token.type = TOKEN_ADDRESS_OF;
            break;

        case '~':
            token.type = TOKEN_NEGATION;
            break;        

        case '!':
            token.type = TOKEN_INDIRECTION;
            break;
 
        /* '-' or '->' */
        case '-':
            if (span.start[span.length -1] == '>') {
                token.type = TOKEN_OBJ_INDREC;
                break;
            }
            token.type = TOKEN_MINUS;
            break;
 
        /* ':' or ':=' */
        case ':':
            if (span.start[span.length -1] == '=') {
                token.type = TOKEN_ASSIGN;
                break;
            }
            token.type = TOKEN_COLON;
            break;
 
        /* '=' or '=>' */
        case '=':
            if (span.start[span.length -1] == '>') {
                token.type = TOKEN_STRUC_REF;
                break;
            }
            token.type = TOKEN_EQ_EQ;
            break;
 
        /* '<', '<=', or '<<' */
        case '<':
            if (span.start[span.length -1] == '=') {
                token.type = TOKEN_LTE;
                break;
            }else if (span.start[span.length -1] == '<') {
                token.type = TOKEN_BITWISEL;
                break;
            }
        
            token.type = TOKEN_LT;
            break;
 
        /* '>', '>=', or '>>' */
        case '>':
            if (span.start[span.length -1] == '=') {
                token.type = TOKEN_LTE;
                break;
            }else if (span.start[span.length -1] == '<') {
                token.type = TOKEN_BITWISEL;
                break;
            }
        
            token.type = TOKEN_LT;
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


/**
 * stringHandler() - Consume a double-quoted string literal.
 * @tok: Initialised Tokenizer positioned at the opening '"'.
 *
 * The following error conditions are detected and reported via
 * reportTokenError():
 *   - A newline before the closing '"' (unterminated string).
 *   - A backslash at the very end of the scanned span (truncated escape).
 *
 * When an error is found the token type is set to TOKEN_ERR and scanning
 * of the current token stops immediately.
 *
 * Return: A TOKEN_STRING token on success, or a TOKEN_ERR token if the
 *         string literal is malformed.
 */
Token stringHandler(Tokenizer *tok)
{
    char delimiter = '"';
	Token token = {0};
	Span span = scanQuotedSequence(tok->reader, delimiter);
    //char c = *span.start;
    //printf("span len %zu\n", span.length);
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

    return token;
}


/**
 * charHandler() - Consume a single-quoted character literal.
 * @tok: Initialised Tokenizer positioned at the opening '\''.
 *
 * Character literals are scanned with the same quoted-sequence scanner
 * used for strings; no additional validation is performed here.
 *
 * Return: A TOKEN_CHAR token (reported as TOKEN_STRING internally).
 *
 * TODO: Validate that the literal contains exactly one (possibly escaped)
 *       character and set token.type = TOKEN_CHAR.
 */
Token charHandler(Tokenizer *tok)
{
    char delimiter = '\'';
	Token token = {0};
	Span span = scanQuotedSequence(tok->reader, delimiter);
    //printf("span len %zu\n", span.length);
    token.type = TOKEN_CHAR;
    // start the loop at index 1 to skipp the initial opening '
    for (size_t i = 1; i < span.length; i++) {
        char c = span.start[i];
        if (c == '\\') {
            printf("fires1\n");
            //skip esdcape sequnce for now. I need to figure out how to handle warning and all that nonsense
            i++;
            if (i > 2){
                printf("fires2\n");
                token.type = TOKEN_ERR;
                reportTokenError(tok, span.endCoord.row, span.endCoord.column, "Escae sequence too long", span.start);                
            }  
        }else if (span.start[1] == delimiter){
            printf("fires3\n");
            token.type = TOKEN_ERR;
            reportTokenError(tok, span.endCoord.row, span.endCoord.column, "Empty Character literal", span.start);
        }else if (span.start[1] == '\n'){
            printf("fires4\n");
            token.type = TOKEN_ERR;
            reportTokenError(tok, span.endCoord.row, span.endCoord.column, "Empty Character literal", span.start);
        }
        
    }
    
    

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

/**
 * eofHandler() - Produce a TOKEN_EOF sentinel token.
 * @tok: Initialised Tokenizer positioned at end-of-file.
 *
 * Return: A TOKEN_EOF token.  Callers should treat this as the termination
 *         signal and stop requesting further tokens.
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

/**
 * delimHandler() - Consume a run of whitespace characters.
 * @tok: Initialised Tokenizer positioned at a whitespace character.
 *
 * Whitespace is not surfaced to the caller as a token; this function
 * simply advances the Reader past all consecutive whitespace so that the
 * next tokNextToken() call begins at a non-whitespace character.
 */
void delimHandler(Tokenizer *tok)
{
	//Token token = {0};
    char lastChar = '\0';
	//Span span = scanWhitespace(tok->reader, &lastChar);
    scanWhitespace(tok->reader, &lastChar);
    /*
	switch (lastChar) {
		case ' ':  token.type = TOKEN_DELIM_S; break;
		case '\t': token.type = TOKEN_DELIM_T; break;
		case '\n': token.type = TOKEN_DELIM_N; break;
		case '\v': token.type = TOKEN_DELIM_V; break;
		case '\f': token.type = TOKEN_DELIM_F; break;
		case '\r': token.type = TOKEN_DELIM_R; break;
        default:   token.type = TOKEN_DELIM_U; break;
    }
    */

}

/**
 * identHandler() - Consume an identifier or keyword.
 * @tok: Initialised Tokenizer positioned at the first character of an
 *       identifier (as determined by scanIsIdentifierStart()).
 *
 * After scanning the full identifier lexeme, a hash-table lookup decides
 * whether the lexeme is a reserved keyword (TOKEN_KEYWORD) or a plain
 * user-defined identifier (TOKEN_IDEN_GENERIC).
 *
 * Return: A TOKEN_KEYWORD or TOKEN_IDEN_GENERIC token.
 */
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
   ===================== error reporting ======================
   ============================================================ */

/**
 * printTokenType() - Print the token type name and lexeme to stdout.
 * @tok: Token to describe.
 *
 * Intended for debugging only.  Output format:
 *   TYPE_NAME -> lexeme
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

 /**
 * reportTokenError() - Forward a lexer error to the registered callback.
 * @tok:     Tokenizer whose @errorFn will be called.
 * @line:    Source line number where the error begins (1-based).
 * @col:     Source column number where the error begins (1-based).
 * @msg:     Human-readable description of the error.
 * @current: Pointer into the source buffer at the error location.
 *
 * Does nothing if no error callback was registered at initialisation time.
 */
void reportTokenError(Tokenizer *tok, size_t line, size_t row, const char *msg, const char *current) {
    if (tok->errorFn) {
        tok->errorFn(line, row, msg, tok->errorUserData, current);
    }
}

/**
 * printFormatted() - Print a length-delimited string to stdout.
 * @tok_str: Pointer to the first character of the string.
 * @length:  Number of characters to print.
 *
 * Convenience wrapper around printf("%.*s") for use during debugging.
 *
 * TODO: Remove once all call sites have been migrated to print_token_type().
 */
void printFormatted(const char *tokStr, int length) {
    // %. *s tells printf to take an integer for length, then the pointer
    printf("%.*s\n", length, tokStr);
}