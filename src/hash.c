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
#include "hash.h"
#include <string.h>
#include <stdio.h>

/* =======================
     BCPL Keyword List
   ======================= */
   struct KeywordEntry {
    const char *name;
} keywordTable[] = {

    [KEYWRD_AND]      = {"AND"},
    [KEYWRD_BE]       = {"BE"},
    [KEYWRD_BREAK]    = {"BREAK"},
    [KEYWRD_BY]       = {"BY"},
    [KEYWRD_CASE]     = {"CASE"},
    [KEYWRD_DEFAULT]  = {"DEFAULT"},
    [KEYWRD_DO]       = {"DO"},
    [KEYWRD_ELSE]     = {"ELSE"},
    [KEYWRD_EQ]       = {"EQ"},
    [KEYWRD_FALSE]    = {"FALSE"},
    [KEYWRD_FOR]      = {"FOR"},
    [KEYWRD_GE]       = {"GE"},
    [KEYWRD_GET]      = {"GET"},
    [KEYWRD_GLOBAL]   = {"GLOBAL"},
    [KEYWRD_GOTO]     = {"GOTO"},
    [KEYWRD_GR]       = {"GR"},
    [KEYWRD_IF]       = {"IF"},
    [KEYWRD_INTO]     = {"INTO"},
    [KEYWRD_LET]      = {"LET"},
    [KEYWRD_LEVEL]    = {"LEVEL"},
    [KEYWRD_LOOP]     = {"LOOP"},
    [KEYWRD_LS]       = {"LS"},
    [KEYWRD_MANIFEST] = {"MANIFEST"},
    [KEYWRD_NE]       = {"NE"},
    [KEYWRD_NOT]      = {"NOT"},
    [KEYWRD_OR]       = {"OR"},
    [KEYWRD_RESULTIS] = {"RESULTIS"},
    [KEYWRD_RESULTS]  = {"RESULTS"},
    [KEYWRD_RETURN]   = {"RETURN"},
    [KEYWRD_SECTION]  = {"SECTION"},
    [KEYWRD_STATIC]   = {"STATIC"},
    [KEYWRD_SWITCH]   = {"SWITCH"},
    [KEYWRD_SWITCHON] = {"SWITCHON"},
    [KEYWRD_TABLE]    = {"TABLE"},
    [KEYWRD_TEST]     = {"TEST"},
    [KEYWRD_THEN]     = {"THEN"},
    [KEYWRD_TO]       = {"TO"},
    [KEYWRD_TRUE]     = {"TRUE"},
    [KEYWRD_UNLESS]   = {"UNLESS"},
    [KEYWRD_UNTIL]    = {"UNTIL"},
    [KEYWRD_VALOF]    = {"VALOF"},
    [KEYWRD_VEC]      = {"VEC"},
    [KEYWRD_WHILE]    = {"WHILE"},
    [KEYWRD_WRITEF]   = {"WRITEF"},
};

// this is unsafe need to likely put this in a loop r something and not blindly reach up to 3 character deep into a string 
// the loop idea was bad so  im trying this instead this doesnt seem extendable to me but again no one has added a new thing to
// bcpl since the cold was active
Keyword perfectHash(const char *keyword, size_t len)
{
	switch (keyword[0]) {
	case 'B': return (len > 2 && keyword[1] == 'R') ? KEYWRD_BREAK   : KEYWRD_BE;
	case 'C': return KEYWRD_CASE;
	case 'D': return (len > 2 && keyword[1] == 'E') ? KEYWRD_DEFAULT : KEYWRD_DO;
	case 'G': return (len > 2 && keyword[1] == 'O') ? KEYWRD_GOTO    : KEYWRD_GLOBAL;
	case 'I': return KEYWRD_IF;
	case 'L': return (len > 2 && keyword[1] == 'O') ? KEYWRD_LOOP    : KEYWRD_LET;
	case 'M': return KEYWRD_MANIFEST;
	case 'R': return (len > 3 && keyword[2] == 'S') ? KEYWRD_RESULTS    : KEYWRD_RETURN;
	case 'S': return (len > 3 && keyword[1] == 'W') ? KEYWRD_SWITCH  : KEYWRD_STATIC;
	case 'U': return (len > 3 && keyword[1] == 'N' && keyword[2] == 'T') ? KEYWRD_UNTIL   : KEYWRD_UNLESS;
	case 'W': return (len > 2 && keyword[1] == 'R') ? KEYWRD_WRITEF    : KEYWRD_WHILE;
    case 'V': return KEYWRD_VALOF;
    case 'T': return KEYWRD_TEST;
	default:  return KEYWRD_UNKNOWN;
	}
}

bool lookUp(const char *keyword, size_t len)
{
	Keyword kywrd = perfectHash(keyword, len);


	if (kywrd == KEYWRD_UNKNOWN)
		return false;
    // originally i had strcmp here and that potentially could read passed what i want it to in the keyword string as its just
    // an index into the original immutable source string
    return strncmp(keywordTable[kywrd].name, keyword, strlen(keywordTable[kywrd].name)) == 0;
}
