/******************************************************************************
* File:        scanner.c
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: source file for the scanner
******************************************************************************/
#include "scanner.h"
#include "reader.h"
#include <ctype.h>


Span scanDigitsWhile(Reader *reader, bool (*digitPredicate)(char))
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


Span scanWhitespace(Reader *reader, char *lastChar)
{
    const char *start = scanCurrentPtr(reader);

    while (!readerPeekEoF(reader) && isspace(readerPeek(reader)))
        *lastChar = readerPeek(reader);
        readerAdvance(reader);
    
    return scanMakeSpan(start, reader);
}

Span scanQuotedSequence(Reader *reader, char delimiter)
{
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
bool isoctal(char c)
{
    return ((c) >= '0' && (c) <= '7');
}

bool isbinary(char c)
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