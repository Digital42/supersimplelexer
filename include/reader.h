/******************************************************************************
* File:        lexer.h   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: Header file for the lexer
******************************************************************************/
#ifndef READER_H
#define READER_H
 
#include <stddef.h>   // size_t
#include <stdio.h>
#include <stdbool.h>


typedef struct {
    const char *input;  /* Input string to tokenize */
    bool ownsInput;     /* tracks if you need to delete buffer or not     */
    size_t sourceLen;
    size_t pos;    /* Current position in input */
    size_t cols;
    size_t lines;
} Reader;

/* =======================
          Prototypes         FIX THESE PROTOTYPES A LOT OF THIS STUFF SHOULDNT BE EXPOSED FROM THIS HEADER FILE I WAS JUST DOING IT FOR UNIT TESTING BECAUSE IM LE NOT SMART
   ======================= */
 
/* reader entry point */
void readerInit(Reader *reader, const char *inputString);
bool readerInitFromFile(Reader *reader, const char *filename);
void readerDestroy(Reader *reader);

const char *readerCurrentPtr(Reader *reader);

//reader helpers
char readerPeek(Reader *reader);
char readerPeekNext(Reader *reader);
char readerPeekPrev(Reader *reader);
char readerAdvance(Reader *reader);
bool readerPeekEoF(Reader *reader);

size_t readerGetCols(Reader *reader);
size_t readerGetLines(Reader *reader);

#endif 