#ifndef QUADRUPLE_H
#define QUADRUPLE_H

#define MAX_QUADS 1000

typedef enum {

    // Arithmetic Operator(s):

    ADD, // Addition [+]
    SUB, // Subtraction [-]
    MUL, // Multiplication [*]
    DIV, // Division [/]
    MOD, // Modulus [%]
    EXP, // Exponential [**]
    UMIN, // <---
    INC, // Increment [++]
    DEC, // Decrement [--]


    // Assignment Operator(s):

    ASSN, // Assignment [=]


    // Relational Operator(s):

    LT, // Less Than [<]
    GT, // Greater Than [>]
    LTE, // Less Than Or Equal [<=]
    GTE, // Greater Than Or Equal [>=]
    EQ, // Equal [==]
    NEQ, // NOT Equal [!=]


    // Logical Operator(s):

    AND, // And [&&]
    OR, // Or [||]
    NOT, // Not [!]


    // Control Flow Operator(s):

    GOTO, // Go To (Label) [GO_TO]
    IFGOTO, // If (condition) Go To (Label) [IF_GO_TO]
    IFFALSE, // else Go To (Label) [IF_FALSE]
    LABEL, // <---


    // Fuction Calls & Stack Handling Operator(s):

    CALL, // Call Function [CALL]
    PARM, // Parameters [PARM]
    RETURN, // Return [RETURN]


    // Type Conversion Operator(s):

    ITOF, // Convert Integer To Float [INT_TO_FLOAT]
    FTOI, // Convert Float To Integer [FLOAT_TO_INT]
    CTOI, // Convert Character To Integer (ASCII) [CHAR_TO_INT]
    ITOB // Convert Integer to Boolean [INT_TO_BOOL]

} Op;

typedef struct {
    Op Operator;
    char* Argument1;
    char* Argument2;
    char* Result;
} Quadruple;

extern Quadruple Quadruples[MAX_QUADS];

extern int quadruplesCount;

char* createTemp ();

char* createLabel ();

const char* operatorToString (Op Operator);

void addQuadruple (Op Operator, const char* Argument1, const char* Argument2, const char* Result);

void printQuadruples ();

void freeQuadruples ();

#endif