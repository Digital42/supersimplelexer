/*******************************************************************************
* File:        test_lexer.c
* Description: Unit tests for nextToken() using Unity framework
******************************************************************************/

#include <unity.h>
#include "lexer.h"
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

static void assertTokenType(Token tok, TokenType expected)
{
    TEST_ASSERT_EQUAL(expected, tok.type);
}

/* ======================
   Tests
   ====================== */

void test_nextToken_semicolon(void)
{
    LexerInfo *lx = lexerCreate(";");

    Token tok = nextToken(lx);

    assertTokenType(tok, TOKEN_SEMICOL);
    TEST_ASSERT_EQUAL(';', tok.value.value);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_nextToken_semicolon);

    return UNITY_END();
}