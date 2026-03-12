/******************************************************************************
* File:        scanner.c
* Date:        03-10-26
* 
* Description: Lexer project
*
* Notes: Reader strictly handles functions related to dealing with where and 
*        what the currect character is for the lexer
******************************************************************************/
#include "reader.h"
#include <stdlib.h>

/* ============================================================
   ==================== READER FUNCTIONS  =====================
   ============================================================ */

/*
 * Frees a reader structure allocated via lexerCreate.
 */
void readerDestroy(Reader *reader)
{
	if (!reader)
		return;

	if (reader->ownsInput == true) {
		/*
		 * this cast is kinda bad i think the lexer struct stores the
		 * input originally as a const char *pointer but when you read
		 * the source from a file it allocated a buffer and malloc
		 * returns char *pointer not const char *pointer
		 */
		free((char *)reader->input);
	}
	free(reader);
}

/*
 * Creates a reader from a given input string.
 * Returns a pointer to a Reader structure.
 */
void readerInit(Reader *reader, const char *inputString)
{
	reader->input = inputString;
	reader->sourceLen = 0;
	reader->pos = 0;
	reader->lines = 1;
	reader->cols = 0;
	reader->ownsInput = false;

}

/*
 * Creates a reader from the contents of a file.
 * Reads the entire file into memory, null-terminates it,
 * and returns a Reader pointer.
 */
bool readerInitFromFile(Reader *reader, const char *filename)
{
	FILE *file = fopen(filename, "r");
	long fileSize;
	char *buffer;
	size_t bytesRead = 0;

	if (!file)
		return false;

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	if (fileSize < 0) {
		fclose(file);
		return false;
	}

	buffer = malloc(fileSize + 1);
	if (!buffer) {
		fclose(file);
		return false;
	}

	/* use bytesRead becuase on windws  text mode 'r' translates \r\n into \n
	 * so the bytes read by fread can be les than what ftell reports
	 */
	bytesRead = fread(buffer, 1, fileSize, file);
	buffer[bytesRead] = '\0';
	fclose(file);

	if (!reader) {
		free(buffer);
		return false;
	}

	readerInit(reader, buffer);

	/*
	 * if the lexer owners the string and then return
	 * it so you can deallocated buffer later
	 */
	reader->ownsInput = true;
	// this will only work when characters are encoded in 8 bits like ansi so this wont return 
	// the correct numbers of charcters in things like utf-8
	reader->sourceLen = bytesRead;
	return true;
}

/* ============================================================
   =========== READER POSITION CHARACTER HELPERS ==============
   ============================================================ */

/*
 * Returns the current character and advances the reader position.
 */
char readerAdvance(Reader *reader)
{
	if (reader->pos >= reader->sourceLen)
		return '\0';

	if (readerPeek(reader) == '\n') {
		reader->lines++;
		reader->cols = 0;
	} else {
		reader->cols++;
	}

	return reader->input[reader->pos++];
}

/*
 * Returns the address of the current character. Used to slice up spans
 */
const char *readerCurrentPtr(Reader *reader)
{
	return &reader->input[reader->pos];    
}

/*
 * Returns the current character without advancing the lexer.
 */
char readerPeek(Reader *reader)
{
	if (reader->pos >= reader->sourceLen)
		return '\0';
	
	return reader->input[reader->pos];
}

/*
 * Returns the next character without advancing the lexer.
 */
char readerPeekNext(Reader *reader)
{
	if (reader->pos + 1 >= reader->sourceLen)
		return '\0';

	return reader->input[reader->pos + 1];
}

/*
 * Returns the previous character without advancing the lexer might
 * be useless if you do somethin like char c = advance(lxer) in loops
 */
char readerPeekPrev(Reader *reader)
{
	if (reader->pos == 0)
		return reader->input[0];
	return reader->input[reader->pos - 1];
}

/*
 * Returns true if the lexer has reached the end of the input.
 */
bool readerPeekEoF(Reader *reader)
{
	return readerPeek(reader) == '\0';
}

size_t readerGetCols(Reader *reader)
{
    return reader->cols;
}
size_t readerGetLines(Reader *reader)
{
    return reader->lines;
}