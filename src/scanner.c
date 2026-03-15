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
/**
 * scanDigitsWhile() - Consume characters for which @pred returns true.
 * @reader: Reader positioned at the first character to test.
 * @pred:   A ctype-style predicate (e.g. isdigit, isxdigit).
 *
 * Advances the Reader until EOF or until @pred returns false for the
 * next character.  Used as the shared implementation for all numeric
 * digit scanners.
 *
 * Note: readerPeek() returns a char which is passed to @pred as int.
 * This is safe because all ANSI characters fit in a single byte and
 * the implicit promotion is value-preserving.
 *
 * Return: Span covering the consumed digit sequence.
 */
Span scanDigitsWhile(Reader *reader, digitPredicate pred)
{
    const char *start = scanCurrentPtr(reader); 
    CharCoord startCoord;
    CharCoord endCoord;
 
    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
 
    while (!readerPeekEoF(reader) && pred(readerPeek(reader)))
        readerAdvance(reader);
 
    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
    
    return scanMakeSpan(start, reader, startCoord, endCoord);       
}
 
/* Convenience wrappers: each selects the appropriate digit predicate. */
Span scanDigits(Reader *reader)         {return scanDigitsWhile(reader, isdigit);}
Span scanHexDigits(Reader *reader)      {return scanDigitsWhile(reader, isxdigit);}
Span scanOctalDigits(Reader *reader)    {return scanDigitsWhile(reader, isoctal);}
Span scanBinaryDigits(Reader *reader)   {return scanDigitsWhile(reader, isbinary);}
 
/**
 * scanEof() - Produce a zero-length Span at the end-of-file position.
 * @reader: Reader that has reached EOF.
 *
 * If the reader is not actually at EOF an empty zeroed Span is returned
 * instead, signalling to the caller that no EOF token should be emitted.
 *
 * Return: Span at the EOF position, or a zeroed Span if not at EOF.
 *
 * TODO: Reconsider whether returning a zeroed Span is the right sentinel;
 *       a boolean out-parameter or a tagged union might be clearer.
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
 
/**
 * scanCommentSequence() - Consume a line comment or a block comment.
 * @reader: Reader positioned at the leading '/'.
 *
 * After consuming the initial '/', the function inspects the next character:
 *   '/'  - line comment:  advances until '\n' (newline is not consumed).
 *   '*'  - block comment: advances until the closing '*' '/' pair.
 *   else - lone slash (treated as a zero-length tail; the '/' was already
 *          consumed by the caller's readerAdvance call below).
 *
 * Note: The block-comment loop condition uses && instead of ||, so it
 * stops when '*' is the current character AND '/' follows — be aware that
 * a '*' not followed by '/' will terminate the loop early.
 *
 * Return: Span covering the entire comment including its delimiters.
 */
Span scanCommentSequence(Reader *reader)
{
    const char *start = scanCurrentPtr(reader);
    CharCoord startCoord;
    CharCoord endCoord;
    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
 
    /* Consume the opening '/'. */
    readerAdvance(reader);
 
    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
 
    if (readerPeek(reader) == '/') {
        /* Line comment: consume everything up to (but not including) '\n'. */
        readerAdvance(reader);
 
        while (readerPeek(reader) != '\n') {
            readerAdvance(reader);
            
        }
        endCoord.column = ScannerGetCols(reader);
        endCoord.row = ScannerGetLines(reader);
        return scanMakeSpan(start, reader, startCoord, endCoord);
 
    }else if (readerPeek(reader) == '*') {
        /* Block comment: consume everything up to the closing '* /'. */
        readerAdvance(reader);
    
        while (readerPeek(reader) != '*' && readerPeekNext(reader) != '/') {
            readerAdvance(reader);
        }
        /* Consume the closing '*' and '/'. */
        readerAdvance(reader);
        readerAdvance(reader);
        endCoord.column = ScannerGetCols(reader);
        endCoord.row = ScannerGetLines(reader);
        return scanMakeSpan(start, reader, startCoord, endCoord);
    }
 
    /* Lone '/' — return a span covering just that character. */
    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
 
    return scanMakeSpan(start, reader, startCoord, endCoord);   
} 
 
/**
 * scanOperator() - Consume one operator token from the input.
 * @reader: Reader positioned at the first character of an operator.
 *
 * Single-character operators are consumed as-is.  Several operators may
 * form two-character sequences when followed by a specific second character:
 *
 * Return: Span covering the one- or two-character operator lexeme.
 */
Span scanOperator(Reader *reader)
{
    const char *start = scanCurrentPtr(reader);
    CharCoord startCoord;
    CharCoord endCoord;
 
    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
 
    char c = readerPeek(reader);
 
    switch (c) {
 
        /* Single-character operators — consume and stop. */
        case '+':
        case '*':
        case '/':
        case '%':
        case '@':
        case '~':
        case '!':
            readerAdvance(reader);
            break;
 
        /* '-' or '->' */
        case '-':
            readerAdvance(reader);
            if (readerPeek(reader) == '>') {
                readerAdvance(reader);
            }
            break;
 
        /* ':' or ':=' */
        case ':':
            readerAdvance(reader);
            if (readerPeek(reader) == '=') {
                readerAdvance(reader);
            }
            break;
 
        /* '=' or '=>' */
        case '=':
            readerAdvance(reader);
            if (readerPeek(reader) == '>') {
                readerAdvance(reader);
            }
            break;
 
        /* '<', '<=', or '<<' */
        case '<':
            readerAdvance(reader);
            if (readerPeek(reader) == '=' || readerPeek(reader) == '<') {
                readerAdvance(reader);
            }
            break;
 
        /* '>', '>=', or '>>' */
        case '>':
            readerAdvance(reader);
            if (readerPeek(reader) == '=' || readerPeek(reader) == '>') {
                readerAdvance(reader);
            }
            break;
            
 
    }
 
    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
        
    return scanMakeSpan(start, reader, startCoord, endCoord);
}
 
/**
 * scanWhitespace() - Consume a contiguous run of whitespace characters.
 * @reader:   Reader positioned at the first whitespace character.
 * @lastChar: Out-parameter; set to the last whitespace character consumed.
 *            Useful when the caller needs to distinguish the kind of
 *            whitespace that ended the run (e.g. newline vs. space).
 *
 * Stops at EOF or the first non-whitespace character.
 *
 * Return: Span covering the consumed whitespace run.
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
 
        readerAdvance(reader);
    }
    
    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
    
    return scanMakeSpan(start, reader, startCoord, endCoord);
}
 
/**
 * scanQuotedSequence() - Consume a string or character literal.
 * @reader:    Reader positioned at the opening delimiter character.
 * @delimiter: The quote character that opens and closes the literal
 *             ('"' for strings, '\'' for character literals).
 *
 * The scanner handles escaped delimiters (backslash followed immediately
 * by @delimiter) by skipping both characters without treating the escaped
 * delimiter as the closing quote.  Scanning also terminates on newline to
 * catch unterminated literals; the caller (string_handler / charHandler)
 * is responsible for reporting the resulting error.
 *
 * Return: Span from the opening delimiter up to and including the closing
 *         delimiter, or up to the newline/EOF that terminated the literal
 *         prematurely.
 */
Span scanQuotedSequence(Reader *reader, char delimiter)
{
    const char *start = scanCurrentPtr(reader); 
    bool isQouted = true;   /* true while still inside the literal */
    CharCoord startCoord;
    CharCoord endCoord;
 
    startCoord.column = ScannerGetCols(reader);
    startCoord.row = ScannerGetLines(reader);
 
    /* Consume the opening delimiter. */
    readerAdvance(reader);
 
    while (!readerPeekEoF(reader) && isQouted == true) {
        char c = readerAdvance(reader);
        if (c == '\\' && readerPeek(reader) == delimiter) {
            /* Escaped delimiter — skip the following quote character. */
            readerAdvance(reader);
        }else if (c == delimiter) {
            /* Closing delimiter reached — literal is complete. */
            isQouted = false;
        }else if (c == '\n') {
            /* Newline inside literal — unterminated; stop and let the
             * caller report the error. */
            isQouted = false;
        }
        
	}
 
    endCoord.column = ScannerGetCols(reader);
    endCoord.row = ScannerGetLines(reader);
 
    return scanMakeSpan(start, reader, startCoord, endCoord);
}
 
/**
 * scanIdentifier() - Consume an identifier (letters, digits, underscores).
 * @reader: Reader positioned at the first character of the identifier.
 *
 * BCPL identifiers may contain letters, decimal digits, and underscores.
 * The caller must ensure the first character is a valid identifier start
 * (see scanIsIdentifierStart()) before calling this function.
 *
 * Return: Span covering the full identifier lexeme.
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
 
/**
 * scanCurrentPtr() - Return a pointer to the current input position.
 * @reader: The active Reader.
 *
 * Thin wrapper around readerCurrentPtr() used to establish the @start
 * pointer before a scan function begins advancing the Reader.
 *
 * Return: Pointer into the source buffer at the current read position.
 */
const char *scanCurrentPtr(Reader *reader)
{
    return readerCurrentPtr(reader);
}
 
/**
 * scanMakeSpan() - Construct a Span from a saved start pointer.
 * @start:      Pointer into the source buffer at the beginning of the lexeme.
 * @reader:     Reader whose current position marks the end of the lexeme.
 * @startCoord: Source coordinate recorded before scanning began.
 * @endCoord:   Source coordinate recorded after scanning finished.
 *
 * The length is computed as the byte distance from @start to the Reader's
 * current position, which is valid as long as both pointers refer to the
 * same underlying buffer.
 *
 * Return: Populated Span.
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
 
/* ------------------------------------------------------------------
 *          Predicate functions — peek without consuming
 * ------------------------------------------------------------------ */
 
/* Returns true if the current character is any whitespace character. */
bool scanIsWhitespace(Reader *reader)
{
    return isspace(readerPeek(reader));
}
 
/**
 * scanIsNumberStart() - Return true if the input looks like a number literal.
 * @reader: Reader to peek from.
 *
 * Matches a leading digit, or a '.' followed immediately by a digit
 * (e.g. ".5" as a floating-point literal).
 *
 * TODO: Currently the two conditions are OR'd incorrectly with &&; this
 *       means only a bare digit (not a leading '.') is detected.  Fix
 *       the logic to: isdigit(c) || (c == '.' && isdigit(next)).
 */
bool scanIsNumberStart(Reader *reader)
{
    char c = readerPeek(reader);
 
    return isdigit(c) && (c == '.' && isdigit(readerPeekNext(reader)));
}
 
/* Returns true if the current character can start an identifier
 * (an ASCII letter or underscore). */
bool scanIsIdentifierStart(Reader *reader)
{
    char c = readerPeek(reader);
 
    return (isalpha(c) || c == '_');   
}
 
/* Returns true if the current position begins a line (//) or
 * block (/ *) comment. */
bool scanIsComment(Reader *reader)
{
    char c = readerPeek(reader);
 
    if (c == '/' && (readerPeekNext(reader) == '/' || readerPeekNext(reader) == '*')) {
        return true;
    }
    
    return false;      
}
 
/* Returns true if the current character is a forward slash '/'.
 * Note: use scanIsComment() to distinguish a comment opener from a
 * division operator. */
bool scanIsSlash(Reader *reader)
{
    char c = readerPeek(reader);
 
    return c == '/';      
}
 
/* Returns true if the current character is the opening '"' of a
 * string literal. */
bool scanIsString(Reader *reader)
{
    char c = readerPeek(reader);
 
    return c == '"'; 
}
 
/* Returns true if the current character is the opening '\'' of a
 * character literal. */
bool scanIsChar(Reader *reader)
{
    char c = readerPeek(reader);
 
    return c == '\''; 
}
 
/* Returns true if the Reader has reached end-of-file. */
bool scanIsEof(Reader *reader)
{
    return readerPeekEoF(reader); 
}
 
/* ------------------------------------------------------------------
 *              Supplementary character-class predicates
 * ------------------------------------------------------------------ */
 
/**
 * isoctal() - Return non-zero if @c is a valid octal digit (0–7).
 * @c: Character value to test (as int, matching ctype convention).
 *
 * TODO: Replace with a proper ctype-style implementation or a macro
 *       once the number scanner is fully implemented.
 */
int isoctal(int c)
{
    return ((c) >= '0' && (c) <= '7');
}
 
/**
 * isbinary() - Return non-zero if @c is a valid binary digit (0 or 1).
 * @c: Character value to test (as int, matching ctype convention).
 *
 * TODO: Replace with a proper ctype-style implementation or a macro
 *       once the number scanner is fully implemented.
 */
int isbinary(int c)
{
    return ((c) >= '0' && (c) <= '1');    
}
 
/* ------------------------------------------------------------------
 *                      Utility functions
 * ------------------------------------------------------------------ */
 
/**
 * scanMatchChar() - Consume the next character if it equals @expected.
 * @reader:   Reader to peek from and conditionally advance.
 * @expected: The character to match against.
 *
 * Return: true if the character matched and was consumed; false otherwise.
 */
bool scanMatchChar(Reader *reader, char expected)
{
    if (readerPeek(reader) == expected){
        readerAdvance(reader);
 
        return true;
    }else
        return false;
}
 
/* Thin wrappers that expose the Reader's column and line counters
 * through the scanner's naming convention. */
size_t ScannerGetCols(Reader *reader)
{
    return readerGetCols(reader);
}
size_t ScannerGetLines(Reader *reader)
{
    return readerGetLines(reader);
}
 
/**
 * isOperator() - Return true if the current character can begin an operator.
 * @reader: Reader to peek from.
 *
 * Covers the full set of operator-starting characters recognised by
 * scanOperator().  Note that '&' and '|' are listed here but are not
 * yet handled in scanOperator(); they will produce a zero-length span
 * until that function is extended.
 */
bool isOperator(Reader *reader)
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