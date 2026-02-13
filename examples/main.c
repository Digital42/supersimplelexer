/******************************************************************************
 * File:        main.c   
 * Date:        02-11-26
 * 
 * Description: lexer project
 *
 * Notes: main.c is main :)
 ******************************************************************************/

#include "lexer.h"

//fix this global later its reason to live to to pass the stored ccharacter from int when a float is detected
uint32_t intPart = 0;


int main()
{

    const char *inputString = "113  + 42;/6- 57";
    lexer(inputString);  

    return 0;
}

