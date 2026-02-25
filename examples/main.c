/******************************************************************************
* File:        main.c
* Date:        02-11-26
*
* Description: Lexer project
*
* Notes: main.c is main :)
******************************************************************************/
#include "lexer.h"

void errorHandler(int line, int col, const char *msg, void *userData, const char *errChar) {
    printf("Lexer error at %d:%d: %s @ %c\n", line, col, msg, errChar);
}


int main()
{
	/* random test strings so i dont have to keep commenting out stuff */
	/* const char *inputString = "v1234567892"; */
	/* const char *inputString = "var int test = 1; var float val = 4 + 3 / 5 * 7 + 5.66"; */
	/* const char *inputString = "2.234;"; */

	const char *filename = "test.bcpl";
	LexerInfo *lxer = lexerCreateFromFile(filename);
    lxer->errorFn = errorHandler;
    //make sure to set the void pointer for whatever main will pass to the lexer error handler 
    lxer->errorUserData = NULL;
	Token t;

	do {
		if (!lxer) {
			printf("no file found\n");
			break;
		}
		t = nextToken(lxer);
		if (t.type != TOKEN_DELIM_S)
			printTokenType(t);
	} while (t.type != TOKEN_EOF);

    printf("Lines: %d\n", lxer->lines);
    printf("Col @ end: %d\n", lxer->cols);
	lexerDestroy(lxer);
	return 0;
}

