/*******************************************************************************
* File:        test_lexer.c
* Description: Unit tests for nextToken() using Unity framework
******************************************************************************/

#include <unity.h>
#include "lexer.h"
#include "hash.h"
#include <string.h>

/* ======================
   Unity Hooks
   ====================== */

void setUp(void)
{
    /* Runs before each test */
}

void tearDown(void)
{
    /* Runs after each test */
}

/* ======================
   Helper Function
   ====================== */

static void assertTokenType(Token *tok, TokenType expected)
{
    TEST_ASSERT_EQUAL(expected, tok->type);
}

/* ======================
   Tests
   ====================== */

void test_perfectHash(void)
   {
       static char *keyWords[] = {
           "LET",
           "BE",
           "IF",
           "UNLESS",
           "WHILE",
           "UNTIL",
           "DO",
           "RETURN",
           "BREAK",
           "LOOP",
           "SWITCH",
           "CASE",
           "DEFAULT",
           "GLOBAL",
           "STATIC",
           "MANIFEST"
       };
   
       Keyword hash = perfectHash(keyWords[0], strlen(keyWords[0]));
       TEST_ASSERT_EQUAL(hash, KEYWRD_LET);
   }
   



void test_nextToken_semicolon(void)
{
    LexerInfo *lx = lexerCreate(";");

    Token *tok = nextToken(lx);

    assertTokenType(tok, TOKEN_SEMICOL);
    TEST_ASSERT_EQUAL(';', tok->value);
}


void test_stringHandler_basic(void) {
    LexerInfo lx = { "\"hello\"", 0 };

    Token *tok = stringHandler(&lx);

    TEST_ASSERT_NOT_NULL(tok);               // Should not be NULL
    TEST_ASSERT_EQUAL(TOKEN_STRING, tok->type); // Type should be TOKEN_STRING
    TEST_ASSERT_NOT_NULL(tok->stringVal);    // stringVal should not be NULL
    TEST_ASSERT_EQUAL_STRING("hello", tok->stringVal); // stringVal matches input

    free(tok->stringVal);
    free(tok);
}

void test_stringHandler_empty_string(void) {
    LexerInfo lx = { "\"\"", 0 };

    Token *tok = stringHandler(&lx);

    TEST_ASSERT_NOT_NULL(tok);
    TEST_ASSERT_EQUAL(TOKEN_STRING, tok->type);
    TEST_ASSERT_NOT_NULL(tok->stringVal);
    TEST_ASSERT_EQUAL_STRING("", tok->stringVal);

    free(tok->stringVal);
    free(tok);
}

void test_stringHandler_long_string(void) {
    // This string will trigger buffer resizing
    char longInput[200];
    for (int i = 0; i < 199; i++) longInput[i] = 'a';
    longInput[199] = '\0';

    char buffer[201];
    snprintf(buffer, sizeof(buffer), "\"%s\"", longInput);

    LexerInfo lx = { buffer, 0 };

    Token *tok = stringHandler(&lx);

    TEST_ASSERT_NOT_NULL(tok);
    TEST_ASSERT_EQUAL(TOKEN_STRING, tok->type);
    TEST_ASSERT_NOT_NULL(tok->stringVal);
    TEST_ASSERT_EQUAL_STRING(longInput, tok->stringVal);

    free(tok->stringVal);
    free(tok);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_perfectHash);
    //RUN_TEST(test_stringHandler_basic);
    //RUN_TEST(test_stringHandler_empty_string);
    //RUN_TEST(test_stringHandler_long_string);
    //RUN_TEST(test_nextToken_semicolon);
    return UNITY_END();
}