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
 
/**
 * readerDestroy() - Release resources owned by a Reader.
 * @reader: Reader to destroy.  Does nothing if NULL.
 *
 * Only frees the input buffer when ownsInput is true, which is the case
 * when the buffer was allocated by readerInitFromFile().  Caller-supplied
 * strings (initialised via readerInit()) are never freed here.
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

/**
 * readerInit() - Initialise a Reader over a caller-supplied string.
 * @reader:      Reader instance to initialise.
 * @inputString: Null-terminated source string; must remain valid for the
 *               lifetime of the Reader.
 *
 * The Reader does not take ownership of @inputString; the caller is
 * responsible for keeping it alive and freeing it if necessary.
 * ownsInput is set to false to reflect this.
 *
 * Note: sourceLen is left at 0 here.  Callers that need an accurate
 * character count (e.g. for bounds checks) should set it explicitly after
 * this call, or use readerInitFromFile() which sets it from the file size.
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

/**
 * readerInitFromFile() - Initialise a Reader by loading a file into memory.
 * @reader:   Reader instance to initialise.
 * @filename: Path to the source file to read.
 *
 * Opens @filename in text mode, allocates a heap buffer large enough for
 * the entire contents, reads the file, and null-terminates the buffer.
 * The Reader takes ownership of the buffer (ownsInput = true), so
 * readerDestroy() will free it.
 *
 * Note: ftell() reports the byte offset in the file, but on Windows text
 * mode translates \r\n to \n during fread(), so bytesRead may be smaller
 * than fileSize.  The null terminator is therefore placed at buffer[bytesRead]
 * rather than buffer[fileSize].
 *
 * Note: sourceLen reflects bytes read, not Unicode characters.  This is
 * correct for ANSI/ASCII source but will give wrong character counts for
 * multi-byte encodings such as UTF-8.
 *
 * Return: true on success; false if the file could not be opened, if
 *         ftell() fails, if malloc() fails, or if @reader is NULL.
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

/**
 * readerAdvance() - Return the current character and move the position forward.
 * @reader: Active Reader.
 *
 * Updates the line and column counters before advancing:
 *   - A '\n' character increments lines and resets cols to 0.
 *   - Any other character increments cols.
 *
 * Return: The character at the current position, or '\0' if already at EOF.
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

/**
 * readerCurrentPtr() - Return a pointer to the current character in the buffer.
 * @reader: Active Reader.
 *
 * Used by the scanner to record the start of a lexeme before advancing,
 * so that a Span can be formed by comparing this pointer with the position
 * after scanning completes.
 *
 * Return: Pointer into the source buffer at the current read position.
 */
const char *readerCurrentPtr(Reader *reader)
{
	return &reader->input[reader->pos];    
}

/**
 * readerPeek() - Return the current character without advancing.
 * @reader: Active Reader.
 *
 * Return: The character at the current position, or '\0' at EOF.
 */
char readerPeek(Reader *reader)
{
	if (reader->pos >= reader->sourceLen)
		return '\0';
	
	return reader->input[reader->pos];
}

/**
 * readerPeekNext() - Return the character one position ahead without advancing.
 * @reader: Active Reader.
 *
 * Used for two-character lookahead (e.g. distinguishing // from /,
 * or -> from -).
 *
 * Return: The character one position ahead, or '\0' if at or past EOF.
 */
char readerPeekNext(Reader *reader)
{
	if (reader->pos + 1 >= reader->sourceLen)
		return '\0';

	return reader->input[reader->pos + 1];
}

/**
 * readerPeekPrev() - Return the most recently consumed character.
 * @reader: Active Reader.
 *
 * Useful for context-sensitive checks after an advance.  Note that if the
 * caller saves the return value of readerAdvance() there is generally no
 * need to call this function.
 * TODO: delete this whole function as I dont think I have used it since I 
 *       added it in for this version of the lexer
 * Return: The character before the current position, or the first character
 *         of the buffer if the position is at the beginning.
 */
char readerPeekPrev(Reader *reader)
{
	if (reader->pos == 0)
		return reader->input[0];
	return reader->input[reader->pos - 1];
}

/**
 * readerPeekEoF() - Return true if the Reader has reached end-of-file.
 * @reader: Active Reader.
 *
 * Delegates to readerPeek(); EOF is signalled by a '\0' at the current
 * position, which readerPeek() returns when pos >= sourceLen.
 *
 * Return: true if at EOF, false otherwise.
 */
bool readerPeekEoF(Reader *reader)
{
	return readerPeek(reader) == '\0';
}

/* Return the current column number (1-based distance from the last newline). */
size_t readerGetCols(Reader *reader)
{
    return reader->cols;
}

/* Return the current line number (1-based, incremented on each '\n'). */
size_t readerGetLines(Reader *reader)
{
    return reader->lines;
}