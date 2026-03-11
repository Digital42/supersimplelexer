/******************************************************************************
* File:        scanner.c
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: source file for the scanner
******************************************************************************/
#ifndef SCANNER_H
#define SCANNER_H

#include <reader.h>
#include <stddef.h>   // size_t
#include <stdio.h>
#include <stdbool.h>


// i dont like this seems like its bad practice make sure to figure this out later
bool isoctal(char c);
bool isbinary(char c);

typedef struct {
    const char *start;  /* star of span */
    size_t length;
} Span;


/* =======================
          Prototypes         
   ======================= */

/* digits and numeric looking things */
Span scanDigits(Reader *reader);
Span scanHexDigits(Reader *reader);
Span scanBinaryDigits(Reader *reader);
Span scanOctalDigits(Reader *reader);

Span scanDigitsWhile(Reader *reader, bool (*digitPredicate)(char));

/* identifier shaped things */
Span scanIdentifier(Reader *reader);

/* string and character shaped things */
Span scanQuotedSequence(Reader *reader, char delimiter);

/* whitespace */
Span scanWhitespace(Reader *reader, char *lastChar);

/* single utility */
bool scanMatchChar(Reader *reader, char expected);
bool scanEscapeSequence(Reader *reader);

const char *scanCurrentPtr(Reader *reader);
Span scanMakeSpan(const char *start, Reader *reader);

/*  functions for making tonizkers easier to read  */
bool scanIsWhitespace(Reader *reader);
bool scanIsNumberStart(Reader *reader);
bool scanIsIdentifierStart(Reader *reader);
bool scanIsSlash(Reader *reader);
bool scanIsString(Reader *reader);
bool scanIsChar(Reader *reader);
bool scanIsEof(Reader *reader);

static Span scanDigitsWhile(Reader *reader, bool (*predicate)(char));

#endif 