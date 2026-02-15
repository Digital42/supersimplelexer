/******************************************************************************
* File:        main.c   
* Date:        02-11-26
* 
* Description: Lexer project
*
* Notes: main.c is main :)
******************************************************************************/

#include "lexer.h"


int main()
{
    //random test strings so i dont have to keep commenting out stuff
    //const char *inputString = "v1234567892";
    const char *inputString = "var int test = 1; var float val = 4 + 3 / 5 * 7 + 5.66";
    //const char *inputString = "2.234;";
    lexer(inputString);  

    return 0;
}

