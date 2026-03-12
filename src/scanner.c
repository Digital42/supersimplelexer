/******************************************************************************
* File:        scanner.c
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: scanner strickly deals getting current or next character from the
*        reader and proccesing the source input into spans of like characters
*        to pass to the tokenizer to further validate and classify tokens
******************************************************************************/
#include "scanner.h"
#include "reader.h"
#include <ctype.h>


Span scanDigitsWhile(Reader *reader, int (*digitPredicate)(int))
{
    const char *start = scanCurrentPtr(reader); 
    
    while (!readerPeekEoF(reader) && digitPredicate(readerPeek(reader)))
        readerAdvance(reader);
    
    return scanMakeSpan(start, reader);       
}

Span scanDigits(Reader *reader)         {return scanDigitsWhile(reader, isdigit);}
Span scanHexDigits(Reader *reader)      {return scanDigitsWhile(reader, isxdigit);}
Span scanOctalDigits(Reader *reader)    {return scanDigitsWhile(reader, isoctal);}
Span scanBinaryDigits(Reader *reader)   {return scanDigitsWhile(reader, isbinary);}

Span scanEof(Reader *reader)
{

    const char *start = scanCurrentPtr(reader);

    if (!readerPeekEoF(reader)){
        Span span = {0};
        return span;
    }
    return scanMakeSpan(start, reader);

}

Span scanOperator(Reader *reader)
{
    const char *start = scanCurrentPtr(reader);
    //char c = readerPeek(reader);
    while (!readerPeekEoF(reader) && isOperator((reader))) {
        readerAdvance(reader);
    }
        
    return scanMakeSpan(start, reader);
}

Span scanWhitespace(Reader *reader, char *lastChar)
{
    const char *start = scanCurrentPtr(reader);

    while (!readerPeekEoF(reader) && isspace(readerPeek(reader))){
        *lastChar = readerPeek(reader);
        //printf("last char:%c:\n", lastChar);

        readerAdvance(reader);
    }
    
    return scanMakeSpan(start, reader);
}

Span scanQuotedSequence(Reader *reader, char delimiter)
{
    printf("delimiter =  %c\n", delimiter);
    const char *start = scanCurrentPtr(reader);   
    
    while (!readerPeekEoF(reader)) {
        readerAdvance(reader);
	}

    return scanMakeSpan(start, reader);
}

Span scanIdentifier(Reader *reader)
{
    const char *start = scanCurrentPtr(reader); 
    
    while (!readerPeekEoF(reader) && (isalpha(readerPeek(reader)) || isdigit(readerPeek(reader)) || readerPeek(reader) == '_')) {
        readerAdvance(reader);
	}

    return scanMakeSpan(start, reader);
}


/*
 * Returns a pointer to the current position in the input buffer.
 */
const char *scanCurrentPtr(Reader *reader)
{
    return readerCurrentPtr(reader);
}

/*
 * Builds a span from a start pointer to the current reader position.
 */
Span scanMakeSpan(const char *start, Reader *reader)
{
    Span span;
    span.start = start;
    span.length = (size_t)(readerCurrentPtr(reader) - start);

    return span;
}


bool scanIsWhitespace(Reader *reader)
{
    return isspace(readerPeek(reader));
}

bool scanIsNumberStart(Reader *reader)
{
    char c = readerPeek(reader);

    return isdigit(c) && (c == '.' && isdigit(readerPeekNext(reader)));
}


bool scanIsIdentifierStart(Reader *reader)
{
    char c = readerPeek(reader);

    return (isalpha(c) || c == '_');   
}

bool scanIsSlash(Reader *reader)
{
    char c = readerPeek(reader);

    return c == '/';      
}
bool scanIsString(Reader *reader)
{
    char c = readerPeek(reader);

    return c == '"'; 
}
bool scanIsChar(Reader *reader)
{
    char c = readerPeek(reader);

    return c == '\''; 
}
bool scanIsEof(Reader *reader)
{
    return readerPeekEoF(reader); 
}

// these functions are really bad i need to replace them just keeping them for placement right now
int isoctal(int c)
{
    return ((c) >= '0' && (c) <= '7');
}

int isbinary(int c)
{
    return ((c) >= '0' && (c) <= '1');    
}

bool scanMatchChar(Reader *reader, char expected)
{
    if (readerPeek(reader) == expected){
        readerAdvance(reader);

        return true;
    }else
        return false;
}

size_t ScannerGetCols(Reader *reader)
{
    return readerGetCols(reader);
}
size_t ScannerGetLines(Reader *reader)
{
    return readerGetLines(reader);
}

int isOperator(Reader *reader)
{
    char c = readerPeek(reader);

    switch (c)
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '<':
        case '>':
        case '&':
        case '|':
        case '~':
        case ':':
            return true;
        default:
            return false;
    }
}