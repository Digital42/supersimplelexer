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

/*
 * Returns a span for an number literal depending on what funciton pointer you pass in
 */
Span scanDigitsWhile(Reader *reader, digitPredicate pred)
{
    const char *start = scanCurrentPtr(reader); 
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
    // this function pointer set up might run into issue later since i think tehcnically there is an implicit cast becuase readerPeek returns a character
    // but the predicate functions all take an intger but there might not be an issue since ansi character are all just 1 byte integer values anyway
    while (!readerPeekEoF(reader) && pred(readerPeek(reader)))
        readerAdvance(reader);

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
    
    return scanMakeSpan(start, reader, startCoord, endCoord);       
}

Span scanDigits(Reader *reader)         {return scanDigitsWhile(reader, isdigit);}
Span scanHexDigits(Reader *reader)      {return scanDigitsWhile(reader, isxdigit);}
Span scanOctalDigits(Reader *reader)    {return scanDigitsWhile(reader, isoctal);}
Span scanBinaryDigits(Reader *reader)   {return scanDigitsWhile(reader, isbinary);}

/*
 * Returns a span for athe EoF token. I dont think this is good i might change this later
 */
Span scanEof(Reader *reader)
{
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);

    const char *start = scanCurrentPtr(reader);
    if (!readerPeekEoF(reader)){
        Span span = {0};
        return span;
    }

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);

    return scanMakeSpan(start, reader, startCoord, endCoord);
}

/*
 * Returns a span for an operator.
 */
Span scanOperator(Reader *reader)
{
    const char *start = scanCurrentPtr(reader);
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);

    //char c = readerPeek(reader);
    while (!readerPeekEoF(reader) && isOperator((reader))) {
        readerAdvance(reader);
    }

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
        
    return scanMakeSpan(start, reader, startCoord, endCoord);
}

/*
 * Returns a span for white space delimiters.
 */
Span scanWhitespace(Reader *reader, char *lastChar)
{
    const char *start = scanCurrentPtr(reader);
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);

    while (!readerPeekEoF(reader) && isspace(readerPeek(reader))){
        *lastChar = readerPeek(reader);
        //printf("last char:%c:\n", lastChar);

        readerAdvance(reader);
    }

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
    
    return scanMakeSpan(start, reader, startCoord, endCoord);
}

/*
 * Returns a span for an string or character literal.
 */
Span scanQuotedSequence(Reader *reader, char delimiter)
{
    //printf("delimiter =  %c\n", delimiter);
    const char *start = scanCurrentPtr(reader); 
    bool isQouted = true; 
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);

    // maybe advance here passed the first " or ' character te
    readerAdvance(reader);

    while (!readerPeekEoF(reader) && isQouted == true) {
        char c = readerAdvance(reader);
        if (c == '\\' && readerPeek(reader) == delimiter) {
            readerAdvance(reader);
        }else if (c == delimiter) {
            isQouted = false;
        }else if (c == '\n') {
            isQouted = false;
        }
        
        //readerAdvance(reader);
	}

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);

    return scanMakeSpan(start, reader, startCoord, endCoord);
}

/*
 * Returns a span for an identifier.
 */
Span scanIdentifier(Reader *reader)
{
    const char *start = scanCurrentPtr(reader); 
    CharCoord startCoord;
    CharCoord endCoord;

    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
    
    while (!readerPeekEoF(reader) && (isalpha(readerPeek(reader)) || isdigit(readerPeek(reader)) || readerPeek(reader) == '_')) {
        readerAdvance(reader);
	}

    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);

    return scanMakeSpan(start, reader, startCoord, endCoord);
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
Span scanMakeSpan(const char *start, Reader *reader, CharCoord startCoord, CharCoord endCoord)
{
    Span span;
    span.start = start;
    span.length = (size_t)(readerCurrentPtr(reader) - start);
    span.startCoord = startCoord;
    span.endCoord = endCoord;

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

