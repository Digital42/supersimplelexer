#include "reader.h"

/* ============================================================
   ===================== LEXER FUNCTIONS  =====================
   ============================================================ */

/*
 * Frees a lexer structure allocated via lexerCreate.
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
 * Creates a lexer from a given input string.
 * Returns a pointer to a Reader structure.
 */
Reader *readerInit(Reader *reader, const char *inputString)
{
	Reader *reader = malloc(sizeof(Reader));

	if (!reader)
		return NULL;

	reader->input = inputString;
	reader->sourceLen = 0;
	reader->pos = 0;
	reader->lines = 1;
	reader->cols = 0;
	reader->ownsInput = false;

	return reader;
}

/*
 * Creates a lexer from the contents of a file.
 * Reads the entire file into memory, null-terminates it,
 * and returns a Reader pointer.
 */
Reader *readerInitFromFile(Reader *reader, const char *filename)
{
	FILE *file = fopen(filename, "r");
	Reader *reader;
	long fileSize;
	char *buffer;
	size_t bytesRead = 0;

	if (!file)
		return NULL;

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);


	if (fileSize < 0) {
		fclose(file);
		return NULL;
	}

	buffer = malloc(fileSize + 1);
	if (!buffer) {
		fclose(file);
		return NULL;
	}

	/* use bytesRead becuase on windws  text mode 'r' translates \r\n into \n
	 * so the bytes read by fread can be les than what ftell reports
	 */
	bytesRead = fread(buffer, 1, fileSize, file);
	buffer[bytesRead] = '\0';
	fclose(file);

	reader = readerCreate(buffer);

	/*
	 * if the lexer owners the string and then return
	 * it so you can deallocated buffer later
	 */
	if (!reader) {
		free(buffer);
		return NULL;
	}
	reader->ownsInput = true;
	// this will only work when characters are encoded in 8 bits like ansi so this wont return 
	// the correct numbers of charcters in things like utf-8
	reader->sourceLen = bytesRead;
	return reader;
}

/* ============================================================
   =========== LEXER POSITION CHARACTER HELPERS ===============
   ============================================================ */

/*
 * Returns the current character and advances the lexer position.
 */
char readerAdvance(Reader *reader)
{
	if (reader->pos >= reader->sourceLen)
		return '\0';

	if (peek(reader) == '\n') {
		reader->lines++;
		reader->cols = 0;
	} else {
		reader->cols++;
	}

	return reader->input[reader->pos++];
}

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
char ReaderPeekNext(Reader *reader)
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
	return peek(reader) == '\0';
}
