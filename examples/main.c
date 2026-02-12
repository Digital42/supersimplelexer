/******************************************************************************
 * File:        main.c   
 * Date:        02-11-26
 * 
 * Description: lexer project
 *
 * Notes: main.c is main :)
 ******************************************************************************/

#include "lexer.h"


int main()
{
    const char *inputString = "11+46+7";
    lexer(inputString);  

    return 0;
}

