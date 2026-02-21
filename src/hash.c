/******************************************************************************
* File:        lexer.c   
* Date:        02-18-26
* 
* Description: Lexer project
*
* Notes: Definition file for the extremly simple hash stuff. since the keyword
*        list is so short just use the enum list and switch statment to match
*        each token to a value vbetween 0-15. This isnt easily extendable but 
*        its bcpl no one has added new keywords to it since the cold war
******************************************************************************/
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "hash.h"
/* =======================
     BCPL Keyword List
   ======================= */ 

struct KeywordEntry {
    const char *name;
} keywordTable[] = {
    [KEYWRD_LET]      = {"LET"},
    [KEYWRD_BE]       = {"BE"},
    [KEYWRD_IF]       = {"IF"},
    [KEYWRD_UNLESS]   = {"UNLESS"},
    [KEYWRD_WHILE]    = {"WHILE"},
    [KEYWRD_UNTIL]    = {"UNTIL"},
    [KEYWRD_DO]       = {"DO"},
    [KEYWRD_RETURN]   = {"RETURN"},
    [KEYWRD_BREAK]    = {"BREAK"},
    [KEYWRD_LOOP]     = {"LOOP"},
    [KEYWRD_SWITCH]   = {"SWITCH"},
    [KEYWRD_CASE]     = {"CASE"},
    [KEYWRD_DEFAULT]  = {"DEFAULT"},
    [KEYWRD_GLOBAL]   = {"GLOBAL"},
    [KEYWRD_STATIC]   = {"STATIC"},
    [KEYWRD_MANIFEST] = {"MANIFEST"},
};

Keyword perfectHash(const char *keyword)
{
    switch (keyword[0]) {
        case 'B': return (keyword[1] == 'R') ? KEYWRD_BREAK  : KEYWRD_BE;
        case 'C': return KEYWRD_CASE;
        case 'D': return (keyword[1] == 'E') ? KEYWRD_DEFAULT : KEYWRD_DO;
        case 'G': return KEYWRD_GLOBAL;
        case 'I': return KEYWRD_IF;
        case 'L': return (keyword[1] == 'O') ? KEYWRD_LOOP   : KEYWRD_LET;
        case 'M': return KEYWRD_MANIFEST;
        case 'R': return KEYWRD_RETURN;
        case 'S': return (keyword[1] == 'W') ? KEYWRD_SWITCH : KEYWRD_STATIC;
        case 'U': return (keyword[2] == 'T') ? KEYWRD_UNTIL  : KEYWRD_UNLESS;
        case 'W': return KEYWRD_WHILE;
        default:  return KEYWRD_UNKNOWN;
    }
}

bool lookUp(const char *keyword)
{
    Keyword kw = perfectHash(keyword);
    if (kw == KEYWRD_UNKNOWN)
        return false;
    return strcmp(keywordTable[kw].name, keyword);
}
