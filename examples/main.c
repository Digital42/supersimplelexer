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
    //const char *inputString = "var int test = 1; var float val = 4 + 3 / 5 * 7 + 5.66";
    //const char *inputString = "2.234;";


    /*const char *inputString =   "LET start() BE\n"
                                "{\n"
                                "    LET a = 6.1\n"
                                "    LET b = 3\n"
                                "    LET sum = a + b\n"
                                "\n"
                                "    LET msg = \"done\""
                                "}\n";

    LexerInfo *lxer = lexerCreate(inputString);
    */
    const char *filename = "test.bcpl";

    LexerInfo *lxer = lexerCreateFromFile(filename);
    Token t;

    do
    {
        if (!lxer)
        {
            printf("no file found\n");
            break;
        }
        
        t = nextToken(lxer);
        if (t.type != TOKEN_DELIM)
        {
            printTokenType(t);
        }
             
        //do parser stuff here

    } while (t.type != TOKEN_EOF);

    return 0;
}

