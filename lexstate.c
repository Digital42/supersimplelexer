#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

typedef uint8_t bool;
#define true 1
#define false 0

typedef enum {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOTEQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_EOF,
    TOKEN_ERR
} TokenType;

typedef enum {
    STATE_START,
    STATE_NUMBER,
    //STATE_FLOAT,
    STATE_OP,
    STATE_EOF,
    STATE_ERR
} LexerState;

typedef enum {
    OP_START,
    OP_EQ,
    OP_BANG,
    OP_LT,
    OP_GT,
    OP_DONE,
    OP_ERR
} OpState;

typedef enum {
    NUM_START,
    NUM_INT,
    NUM_FLOAT,
    NUM_DONE,
    NUM_ERR
} NumState;

typedef union{
    int intVal;
    float floatVal;   
} numTypeRtrn;

typedef struct {
    TokenType type;
    union {
        int tokenIntVal;
        float tokenFloatVal;
    } value;
} Token;

typedef struct {
    const char *input;
    size_t pos;
} LexerInfo;



// function pointer type for states for top level state machine
typedef void (*StateHandler)(LexerInfo *lxer, LexerState *state);


// function pointer type for states for lower level state machine
typedef void (*OpHandler)(LexerInfo *lxer, LexerState *state, TokenType *tok);
typedef void (*NumHandler)(LexerInfo *lxer, LexerState *state, TokenType *tok);



//protos
char peek(LexerInfo *lxer);
char peekNext(LexerInfo *lxer);
char advance(LexerInfo *lxer);
bool peekEoF(LexerInfo *lxer);
void lexer( const char *inputString);
void lexerState(LexerState state);
numTypeRtrn numHandler(LexerInfo *lxer);
//function pointers prototypes
void startState(LexerInfo *lxer, LexerState *state);
void numberState(LexerInfo *lxer, LexerState *state);
void opState(LexerInfo *lxer, LexerState *state);
void errState(LexerInfo *lxer, LexerState *state);
void eofState(LexerInfo *lxer, LexerState *state);


void opEq(LexerInfo *lxer, OpState *st, TokenType *out);
void opStart(LexerInfo *lxer, OpState *st, TokenType *out);

void numStart(LexerInfo *lxer, NumState *st, TokenType *out);
void numInt(LexerInfo *lxer, NumState *st, TokenType *out);
void numFloat(LexerInfo *lxer, NumState *st, TokenType *out);

//this is the function pointer table for the operator handler sub state
OpHandler opHandlers[] = {
    opStart,
    opEq,
    //opBang,
    //opLt,
   // opGt,
    //NULL,
    //NULL
};

NumHandler numHandlers[] = {
    numStart,
    numInt,
    numFloat,
    //opLt,
   // opGt,
    //NULL,
    //NULL
};


int main()
{
    const char *inputString = "1.1+467+36";
    lexer(inputString);  

    return 0;
}

void lexer(const char  *inputString)
{
    LexerState state = STATE_START;
    LexerInfo lex = {inputString, 0};

    // state function pointer array
    StateHandler handlers[] = {
        startState,
        numberState,
        opState,
        eofState,
        errState
    };
    // this loop rips thru top level states and to enter a lower level state you call its funciton and go into loop stat
    while (true) {
        handlers[state](&lex, &state);
        
        if (state == STATE_EOF) {
            handlers[STATE_EOF](&lex, &state);
            break;
        }else if (state == STATE_ERR)
        {
            handlers[STATE_ERR](&lex, &state);
            //break;
        }
        
    }

}


void startState(LexerInfo *lxer, LexerState *state)
{

    char c = peek(lxer);

    if (isdigit(c))
    {
        *state = STATE_NUMBER;
    } 
    else if (isspace(c)) 
    {
        advance(lxer);
    }
    else if (c == '\0')
    {
        *state = STATE_EOF;
    }
    else if (strchr("+-*/=!<>", c)){
        *state = STATE_OP;
    }
    else{
        *state = STATE_ERR;
    }
}

void numberState(LexerInfo *lxer, LexerState *state){

    /*char c = advance(lxer);
    printf("number: %c\n", c);
    *state = STATE_START;*/
   NumState numState = NUM_START;
   TokenType tok = TOKEN_ERR;

   while (numState != NUM_DONE &&  numState != NUM_ERR)
   {
        numHandlers[numState](lxer, &numState, &tok);
   }
   if (numState == NUM_ERR)
   {
       printf("TOKEN_ERR at %zu\n", lxer->pos);
   }

   *state = STATE_START;
}

void numStart(LexerInfo *lxer, NumState *st, TokenType *out)
{
    //NumState numState;

    char c = peek(lxer);
    if (isdigit(c))
    {
        //printf("numStart: %c\n", c);
        *st = NUM_INT;
    }else if (c =='.' && isdigit(peekNext(lexer)) && *st == NUM_INT){
        *st = NUM_FLOAT;
    }else{
        *st = NUM_ERR;
    }

}

void numInt(LexerInfo *lxer, NumState *st, TokenType *out)
{
    NumState numState = NUM_START;
    TokenType tok = TOKEN_ERR;
    int intPart = 0;

    while (isdigit(peek(lxer))) {
        if (peekNext(lxer) == '.')
        {
            intPart = intPart * 10 + (peek(lxer) - '0');
            //printf("called");
            advance(lxer);
            *st = NUM_FLOAT;

            return;
        }
            /* code */
            intPart = intPart * 10 + (peek(lxer) - '0');
            advance(lxer);
        
           
    }

    printf("TOKEN_INT: %d\n", intPart);
    *st = NUM_DONE;


}

void numFloat(LexerInfo *lxer, NumState *st, TokenType *out)
{

    // this part is where the . is skipped in number state
    char c = advance(lxer);
    printf("char: %c", c);
    if (isdigit(peekNext(lxer)))
    {
        printf("TOKEN_FLOAT\n"); 
        *st = NUM_DONE;
        return;   
    }else{
        //*st = NUM_ERR;
        return;
    }
    
    //printf("TOKEN_FLOAT\n");

}


void opState(LexerInfo *lxer, LexerState *state){
    OpState opState = OP_START;
    TokenType tok = TOKEN_ERR;

    while (opState != OP_DONE &&  opState != OP_ERR)
    {
        opHandlers[opState](lxer, &opState, &tok);
    }
    if (opState == OP_DONE)
    {
        printf("TOKEN_OP: %d\n", tok);
    }else{
        printf("TOKEN_ERR at %zu\n", lxer->pos);
    }

    *state = STATE_START;
}

void opStart(LexerInfo *lxer, OpState *st, TokenType *out)
{
    char c = advance(lxer);
    switch (c) {
        case '=': *st = OP_EQ;
                  break;

        case '+': *out = TOKEN_PLUS; 
                  *st = OP_DONE;
                  break;

        case '-': *out = TOKEN_MINUS; 
                  *st = OP_DONE;
                  break;

        case '*': *out = TOKEN_MUL;
                  *st = OP_DONE;
                  break;

        case '/': *out = TOKEN_DIV;
                  *st = OP_DONE;
                  break;

        default:  *st = OP_ERR;
                  break;
    }
}

void opEq(LexerInfo *lxer, OpState *st, TokenType *out)
{
    if (peek(lxer) == '=') {
        advance(lxer);
        *out = TOKEN_EQUAL;
    } else {
        *out = TOKEN_ASSIGN;
    }
    *st = OP_DONE;
}

void opBang(LexerInfo *lxer, OpState *st, TokenType *out)
{
    if (peek(lxer) == '=') {
        advance(lxer);
        *out = TOKEN_NOTEQ;
        *st = OP_DONE;
    } else {
        *st = OP_ERR;
    }
}

void opLt(LexerInfo *lxer, OpState *st, TokenType *out)
{
    if (peek(lxer) == '=') {
        advance(lxer);
        *out = TOKEN_LTE;
    } else {
        *out = TOKEN_LT;
    }
    *st = OP_DONE;
}

void opGt(LexerInfo *lxer, OpState *st, TokenType *out)
{
    if (peek(lxer) == '=') {
        advance(lxer);
        *out = TOKEN_GTE;
    } else {
        *out = TOKEN_GT;
    }
    *st = OP_DONE;
}


void eofState(LexerInfo *lxer, LexerState *state)
{
    printf("EOF\n");  
    *state = STATE_EOF;      
}

void errState(LexerInfo *lxer, LexerState *state)
{
    char c = advance(lxer);
    printf("ERR");
    *state = STATE_START;    
}

numTypeRtrn numHandler(LexerInfo *lxer){
    numTypeRtrn result = {0};
    int intPart = 0;
    int magnitude = 0;
    bool isFloat = false;

    while (isdigit(peek(lxer)) || peek(lxer) == '.') {
        if (peek(lxer) == '.') {
            isFloat = true;
            advance(lxer);
            float fracPart = 0.0f;
            float divisor = 10.0f;
            while (isdigit(peek(lxer))) {
                fracPart += (peek(lxer) - '0') / divisor;
                divisor *= 10.0f;
                advance(lxer);
            }
            result.floatVal = intPart + fracPart;
            printf("TOKEN_FLOAT: %.3f\n", result.floatVal);
            return result;
        }
        intPart = intPart * 10 + (peek(lxer) - '0');
        advance(lxer);
    }

    if (!isFloat) result.intVal = intPart;
    printf("TOKEN_INT: %d\n", result.intVal);
    return result;
}

//use lexer info struc to get the vaalue at whaterv the poistion is right now
// aand retrn that char
char advance(LexerInfo *lxer)
{
    char c = peek(lxer);

    if(c != '\0')
    {
        lxer->pos++;
    }
    return c;
}

char peekNext(LexerInfo *lxer)
{
    if(peek(lxer) == '\0')
    {
        return '\0';
    }else 
        return lxer->input[lxer->pos + 1];

}

char peek(LexerInfo *lxer)
{
    return lxer->input[lxer->pos]; 
}

bool peekEoF(LexerInfo *lxer)
{
    return peek(lxer) == '\0';
}

void lexerState(LexerState state)
{
    switch (state)
    {
    case 0:
        printf("State: start\n");
        break;

    case 1:
        printf("State: number\n");
        break;

    case 2:
        printf("State: float\n");
        break;

    case 3:
        printf("State: op\n");
        break;

    case 4:
        printf("State: End of File\n");
        break;

    case 5:
        printf("State: Error\n");
        break;

    default:
        break;
    }
}


