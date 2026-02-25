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
#include "hash.h"
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

/* token list defined in header file lexer.h*/
static const char *tokenTypeNames[TOKEN_COUNT] = {
    #define X(name) [name] = #name,
        TOKEN_LIST
     #undef X
 };

 #define isxblurf(c) \
 ((c) == 'x' || (c) == 'X' || \
  (c) == 'b' || (c) == 'B' || \
  (c) == 'u' || (c) == 'U' || \
  (c) == 'l' || (c) == 'L' || \
  (c) == 'f' || (c) == 'F' || \
  (c) == '.' || (c) == 'o' || \
  (c) == 'O' )



typedef enum {
    STATE_START,
    STATE_INT,
    STATE_FLOAT,
    STATE_LITERAL,
    STATE_HEX,
    STATE_BIN,
    STATE_OCT,
    STATE_ERR
} NumState;

/* ============================================================
   ===================== LEXER FUNCTIONS  =====================
   ============================================================ */

/*
 * Frees a lexer structure allocated via lexerCreate.
 */
void lexerDestroy(LexerInfo *lex)
{
	if (!lex)
		return;

	if (lex->ownsInput == true) {
		/*
		 * this cast is kinda bad i think the lexer struct store the
		 * input originally as a const char *pointer but when you read
		 * the source from a file it allocated a buffer and malloc
		 * returns char *pointer not const char *pointer
		 */
		free((char *)lex->input);
	}
	free(lex);
}

/*
 * Creates a lexer from a given input string.
 * Returns a pointer to a LexerInfo structure.
 */
LexerInfo *lexerCreate(const char *inputString)
{
	LexerInfo *lex = malloc(sizeof(LexerInfo));

	if (!lex)
		return NULL;

	lex->input = inputString;
	lex->pos = 0;
    lex->lines = 1;
    lex->cols = 1;
	lex->ownsInput = false;

	return lex;
}

/*
 * Creates a lexer from the contents of a file.
 * Reads the entire file into memory, null-terminates it,
 * and returns a LexerInfo pointer.
 */
LexerInfo *lexerCreateFromFile(const char *filename)
{
	FILE *file = fopen(filename, "r");
	LexerInfo *lex;
	long fileSize;
	char *buffer;
    size_t bytesRead = 0;
	if (!file)
		return NULL;

	/* Determine file size */
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

    
    if (fileSize < 0) {
        /* Error: ftell returns -1 on error */
        fclose(file);
        return NULL;
    }

	/* Allocate buffer for file contents + null terminator */
	buffer = malloc(fileSize + 1);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

    // use bytesRead becuase on windws  text mode 'r' translates \r\n into \n so the bytes 
    // read by fread can be les than what ftell reports
	bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';
	fclose(file);

	/* Use lexerCreate to initialize lexer */
	lex = lexerCreate(buffer);

	/*
	 * turn on tracking if the lexer owners the string and then return
	 * it so you can deallocated buffer later
	 */
	if (!lex) {
		free(buffer);
		return NULL;
	}
	lex->ownsInput = true;
	return lex;
}

/* ============================================================
   +====================  TOKEN DRIVERS  ======================
   ============================================================ */

/*
 * Returns the next token from the input.
 * Dispatches \n to appropriate handlers based on character type.
 */
Token nextToken(LexerInfo *lxer)
{
    
	Token tok = {0};
	char c = peek(lxer);

    // this comment support is wonky and needs to be changed later. throw it into a function or something

    if (c == '/' && peekNext(lxer) == '/' )
    {
        tok.start = lxer->input + lxer->pos;
        tok.type = TOKEN_COMMENT;
        //commnet state
        advance(lxer);
        advance(lxer);
        tok.length++;
        tok.length++;

        while (!peekEoF(lxer)) {
            /* code */
            //advance(lxer);
            if (peek(lxer) == '\n') {

                advance(lxer);
                tok.length++;

                return tok;
            }
            tok.length++;
            advance(lxer);
        }

        return tok;
    }
    

    /* Handle comments */
    if(c == '/' && peekNext(lxer) == '*' ){
        tok.start = lxer->input + lxer->pos;
        tok.type = TOKEN_COMMENT;
        //commnet state
        advance(lxer);
        advance(lxer);
        tok.length++;
        tok.length++;

        while (!peekEoF(lxer)) {
            /* code */
            //advance(lxer);
            if (peek(lxer) == '*' && peekNext(lxer) == '/') {

                advance(lxer);
                advance(lxer);
                tok.length++;
                tok.length++;

                return tok;
            }
            tok.length++;
            advance(lxer);
        }
        // restrt tokenization? I dunno if this is good or not i probably need to fix this nonsense typing issue here
        reportLexerError(lxer, "Unterminated string literal");
        tok.type = TOKEN_ERR;
        return tok;
    }

	/* Whitespace or delimiter */
	if (isspace(c))
		return delimHandler(lxer);

	/* Numeric literal (int or float) */
	if (isdigit(c) || (c == '.' && isdigit(peekNext(lxer))))
		return numHandler(lxer);

	/* Identifier or keyword */
	if (isalpha(c) || c == '_')
		return identHandler(lxer);

	/* Single-character or multi-character tokens */
	switch (peek(lxer)) {
        
	case '\0':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_EOF;
		tok.length = 1;
		advance(lxer);
		return tok;

	case '(':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_LPAREN;
		tok.length = 1;
		advance(lxer);
		return tok;

	case ')':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_RPAREN;
		tok.length = 1;
		advance(lxer);
		return tok;

	case '{':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_LBRACE;
		tok.length = 1;
		advance(lxer);
		return tok;

	case '}':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_RBRACE;
		tok.length = 1;
		advance(lxer);
		return tok;

	case ';':
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_SEMICOL;
		tok.length = 1;
		advance(lxer);
		return tok;

	case '+':
		return opHandler(lxer);

    case '!':
		return opHandler(lxer);    

	case '=':
		return opHandler(lxer);

	case '*':
		return opHandler(lxer);

	case '/':
		return opHandler(lxer);

	case '<':
		return opHandler(lxer);

	case '>':
		return opHandler(lxer);

	case '"':
		return stringHandler(lxer);

	default:

        reportLexerError(lxer, "Out of place character");
		tok.start = lxer->input + lxer->pos;
		tok.type = TOKEN_ERR;
		tok.length = 1;
		advance(lxer);
		return tok;
	}
}

/* ============================================================
   ===================== TOKEN HANDLERS =======================
   ============================================================ */

/*
 * Handles spaces and delimiter characters.
 * Returns a single token representing a contiguous block of whitespace.
 */
Token delimHandler(LexerInfo *lxer)
{
	Token tok = {0};
	char c = peek(lxer);

	tok.start = lxer->input + lxer->pos;
	tok.length = 0;

	do {
		switch (c) {
		case ' ':  tok.type = TOKEN_DELIM_S; break;
		case '\t': tok.type = TOKEN_DELIM_T; break;
		case '\n': tok.type = TOKEN_DELIM_N; break;
		case '\v': tok.type = TOKEN_DELIM_V; break;
		case '\f': tok.type = TOKEN_DELIM_F; break;
		case '\r': tok.type = TOKEN_DELIM_R; break;
		default:   tok.type = TOKEN_DELIM_U; break;
		}

		tok.length++;
		advance(lxer);

	} while (isspace(peek(lxer)) && !peekEoF(lxer));

	return tok;
}

/*
 * Handles string literals enclosed in double quotes.
 * Supports escape sequences like \" inside strings.
 */
Token stringHandler(LexerInfo *lxer)
{
	Token tok = {0};
	char c;

	tok.start = lxer->input + lxer->pos;
	tok.length = 1;
	tok.type = TOKEN_STRING;
	c = advance(lxer);

	do {
		if (peek(lxer) == '\0') {
			/* Unterminated string; return as-is */
			return tok;
		}

		tok.length++;
		c = advance(lxer);

		if (c == '\\') {
			tok.length++;
			advance(lxer);
			if (peek(lxer) == '"') {
				tok.length++;
				advance(lxer);
			}
            // need to handle other escape characters here
		}
        // need to handle un terminated strings around here 
	} while (c != '"');

	return tok;
}

/*
 * Handles identifiers and keywords.
 * Identifiers start with a letter and may contain digits.
 * Returns TOKEN_KEYWORD if found in keyword table.
 */
Token identHandler(LexerInfo *lxer)
{
	Token tok = {0};

	tok.start = lxer->input + lxer->pos;
	tok.length = 0;

	while (!peekEoF(lxer) && (isalpha(peek(lxer)) || isdigit(peek(lxer)) || peek(lxer) == '_')) {
		tok.length++;
		advance(lxer);
	}

	if (lookUp(tok.start, tok.length))
		tok.type = TOKEN_KEYWORD;
	else
		tok.type = TOKEN_IDEN_GENERIC;

	return tok;
}

/*
 * Handles single- and multi-character operators like +, ++, +=, *, /, etc.
 */
Token opHandler(LexerInfo *lxer)
{
	Token tok = {0};
	char c;

	tok.start = lxer->input + lxer->pos;
	tok.length = 1;
	c = advance(lxer);

	switch (c) {
	case '+':
		if (peek(lxer) == '+') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_INCR;
		} else if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_PLUS_EQ;
		} else {
			tok.type = TOKEN_PLUS;
		}
		break;

	case '-':
		if (peek(lxer) == '-') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_DECR;
		} else if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_MINUS_EQ;
		} else {
			tok.type = TOKEN_MINUS;
		}
		break;

	case '*':
		if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_MUL_EQ;
		} else {
			tok.type = TOKEN_MUL;
		}
		break;

	case '/':
		if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_DIV_EQ;
		} else {
			tok.type = TOKEN_DIV;
		}
		break;

    case '%':
		tok.type = TOKEN_MOD;

		break;
	case '!':
		tok.type = TOKEN_NEGATION;

		break;

	case '=':
		if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_EQ_EQ;
		} else {
			tok.type = TOKEN_ASSIGN;
		}
		break;

	case '<':
		if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_LTE;
		} else {
			tok.type = TOKEN_LT;
		}
		break;

	case '>':
		if (peek(lxer) == '=') {
			advance(lxer);
			tok.length++;
			tok.type = TOKEN_GTE;
		} else {
			tok.type = TOKEN_GT;
		}
		break;

	default:
		tok.type = TOKEN_UNKNOWN;
		break;
	}

	return tok;
}

/*
 * Handles integer and floating-point literals in one pass.
 * later on the string thats been index can use stdlib funcs
 * to safely convert strings to ints or floats no need for me
 * to roll my own
 */
Token numHandler(LexerInfo *lxer)
{
	Token tok = {0};
	//bool isFloat = false;
	tok.start = lxer->input + lxer->pos;
	tok.length = 0;
    NumState state = STATE_START;

    while ( ( isxdigit(peek(lxer)) || isxblurf(peek(lxer)) ) && !peekEoF(lxer)  ){
        char c = advance(lxer);
        switch (state) {
            case STATE_START:
                if (c == '0' && isxblurf(peek(lxer))){

                        state = STATE_LITERAL;
                        tok.length++;
                
                }else if (c == '0' && isdigit(peek(lxer)))
                {
                    
                    state = STATE_ERR;
                    tok.length++;

                    reportLexerError(lxer, "Malformed numeric Literal");  
                }else if (c == '.'){
                    state = STATE_FLOAT;
                    //advance();
                    tok.length++;
                }else if (isdigit(c)){
                    state = STATE_INT;
                    tok.length++;
                }else{
                    state = STATE_ERR;
                    //advance();
                    tok.length++;
                    reportLexerError(lxer, "Malformed unnamed numeric Literal");                  
                }break;
                
                
             case STATE_LITERAL:
                if (isoctal(c) ){
                    state = STATE_OCT;
                    tok.length++;
                    
                }else if (c == 'x' || c == 'X'){
                    state = STATE_HEX;
                    //advance();
                    tok.length++;
                }else if (c == 'b' || c == 'B'){
                    state = STATE_BIN;
                    //advance();
                    tok.length++;
                }else if (c == 'o' || c == 'O'){
                    state = STATE_OCT;
                    //advance();
                    tok.length++;
                }else{
                    state = STATE_ERR;
                    //advance();
                    tok.length++;
                    reportLexerError(lxer, "Malformed Literal General");                  
                }break;               
                
                
            case STATE_INT:  
                if (isdigit(c)){
                    //advance();
                    tok.length++;
                }else if (c == '.'){
                    tok.length++;
                    state = STATE_FLOAT;
                }else{
                   // advance(); 
                    tok.length++;
                    state = STATE_ERR;
                    reportLexerError(lxer, "Malformed Integer Literal");      
                }break;
                
            case STATE_FLOAT:  
                if (isdigit(c)){
                    //advance();
                    tok.length++;
                }else{
                   // advance(); 
                    tok.length++;
                    state = STATE_ERR;  
                    reportLexerError(lxer, "Malformed Float Literal");    
                }break;
                
            case STATE_HEX:
                if (isxdigit(c)){
                    //advance();
                    tok.length++;
                }else{
                    //advance(); 
                    tok.length++;
                    state = STATE_ERR;
                    reportLexerError(lxer, "Malformed Hex Literal");      
                }break; 
                
            case STATE_BIN:
                if (isbinary(c)){
                    //advance();
                    tok.length++;
                }else{
                    //advance(); 
                    tok.length++;
                    state = STATE_ERR;   
                    reportLexerError(lxer, "Malformed Binary Literal");   
                }break;   
                
            case STATE_OCT:
                if (isoctal(c)){
                    //advance();
                    tok.length++;
                }else{
                    //advance(); 
                    tok.length++;
                    state = STATE_ERR; 
                    reportLexerError(lxer, "Malformed Octal Literal");   
                }break; 

            case STATE_ERR:
                //advance();
                tok.length++;
                //advance(); 
                //reportLexerError(lxer, "Gneral Malform Number Token");
                break; 
        }
    }
    
    switch (state){
        case STATE_ERR:
            tok.type = TOKEN_ERR;
            break;
            
        case STATE_INT:
            tok.type = TOKEN_INT;
            break;   
            
        case STATE_FLOAT:
            tok.type = TOKEN_FLOAT;
            break;
            
        case STATE_HEX:
            tok.type = TOKEN_HEX;
            break;   
            
        case STATE_OCT:
            tok.type = TOKEN_OCT;
            break; 
            
        case STATE_BIN:
            tok.type = TOKEN_BIN;
            break;  
            
        case STATE_LITERAL:
            tok.type = TOKEN_LITERAL;
            break;       
    }

    return tok;
}

/* ============================================================
   =========== LEXER POSITION CHARACTER HELPERS ===============
   ============================================================ */

/*
 * Returns the current character and advances the lexer position.
 */
char advance(LexerInfo *lxer)
{
	if (peekEoF(lxer))
		return '\0';

    if (peek(lxer) == '\n'){
        lxer->lines++;
        lxer->cols = 1;
    }else{
        lxer->cols++;
    }

	return lxer->input[lxer->pos++];
}

/*
 * Returns the current character without advancing the lexer.
 */
char peek(LexerInfo *lxer)
{
	return lxer->input[lxer->pos];
}

/*
 * Returns the next character without advancing the lexer.
 */
char peekNext(LexerInfo *lxer)
{
	if (peek(lxer) == '\0')
		return '\0';
	return lxer->input[lxer->pos + 1];
}

/*
 * Returns true if the lexer has reached the end of the input.
 */
bool peekEoF(LexerInfo *lxer)
{
	return peek(lxer) == '\0';
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
		name = tokenTypeNames[tok.type];

	printf("%-18s", name);

	if (tok.start && tok.length > 0)
		printf(" -> %.*s", (int)tok.length, tok.start);

	printf("\n");
}



void reportLexerError(LexerInfo *lex, const char *msg) {
    if (lex->errorFn) {
        lex->errorFn(lex->lines, lex->cols, msg, lex->errorUserData, lex->input[lex->pos-1]);
    }
}