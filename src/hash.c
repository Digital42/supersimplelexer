/*
* File:        lexer.c
* Date:        02-18-26
*
* Description: Lexer project
*
* Notes: keyword perfect hash produced by gperf version 3.1
* 
***********************************************************************/

#include "hash.h"
#include <string.h>
 
/* maximum key range = 66, duplicates = 0 */
 
static unsigned int hash(const char *str, size_t len) 
{
    static unsigned char asso_values[] = {
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 5, 40, 25, 40, 15,
        0, 10, 20, 30, 68, 68, 0, 0, 30, 0,
        68, 50, 0, 15, 5, 5, 25, 25, 68, 10,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
        68, 68, 68, 68, 68, 68
    };
 
    return len +
        asso_values[(unsigned char)str[1]] +
        asso_values[(unsigned char)str[0]];
}
 
bool lookUp(const char *str, size_t len) 
{
    static const char *wordlist[] = {
        "", "",
        "OR",
        "FOR",
        "LOOP",
        "", "",
        "TO",
        "",
        "TRUE",
        "FALSE",
        "",
        "GR",
        "MANIFEST",
        "GOTO",
        "TABLE",
        "GLOBAL",
        "LS",
        "LET",
        "ELSE",
        "LEVEL",
        "RETURN",
        "RESULTS",
        "RESULTIS",
        "TEST",
        "",
        "STATIC",
        "GE",
        "GET",
        "THEN",
        "",
        "WRITEF",
        "IF",
        "NOT",
        "CASE",
        "VALOF",
        "",
        "SECTION",
        "AND",
        "",
        "UNTIL",
        "UNLESS",
        "DO",
        "VEC",
        "",
        "BREAK",
        "SWITCH",
        "NE",
        "SWITCHON",
        "",
        "WHILE",
        "",
        "BY",
        "", "", "", "",
        "BE",
        "", "", "", "",
        "DEFAULT",
        "",
        "INTO",
        "", "",
        "EQ"
    };
 
    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        unsigned int key = hash(str, len);
 
        if (key <= MAX_HASH_VALUE) {
            const char *s = wordlist[key];
 
            //chnage the orignal gperf struct to store the length of the string
            //see what the performance impact of hardcoded len vs calling strlen
            if (strncmp(wordlist[key], s, strlen(wordlist[key])) == 0)
                /*
                * Originally:
                * if (*str == *s && !strcmp(str + 1, s + 1))
                * might need to fix this later on. Needs testing
                */
                return 1;
        }
    }
 
    return 0;
}