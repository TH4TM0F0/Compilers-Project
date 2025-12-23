#ifndef QUADRUPLE_H
#define QUADRUPLE_H

#define MAX_QUADS 1000

typedef enum {

    // Arithmetic Operator(s):

    OP_ADD, // Addition [+]
    OP_SUB, // Subtraction [-]
    OP_MUL, // Multiplication [*]
    OP_DIV, // Division [/]
    OP_MOD, // Modulus [%]
    OP_EXP, // Exponential [**]
    OP_UMIN, // <---
    OP_INC, // Increment [++]
    OP_DEC, // Decrement [--]


    // Assignment Operator(s):

    OP_ASSN, // Assignment [=]


    // Relational Operator(s):

    OP_LT, // Less Than [<]
    OP_GT, // Greater Than [>]
    OP_LTE, // Less Than Or Equal [<=]
    OP_GTE, // Greater Than Or Equal [>=]
    OP_EQ, // Equal [==]
    OP_NEQ, // NOT Equal [!=]


    // Logical Operator(s):

    OP_AND, // And [&&]
    OP_OR, // Or [||]
    OP_NOT, // Not [!]


    // Control Flow Operator(s):

    OP_GOTO, // Go To (Label) [GO_TO]
    OP_IFGOTO, // If (condition) Go To (Label) [IF_GO_TO]
    OP_IFFALSE, // else Go To (Label) [IF_FALSE]
    OP_LABEL, // <---


    // Fuction Calls & Stack Handling Operator(s):

    OP_CALL, // Call Function [CALL]
    OP_PARM, // Parameters [PARM]
    OP_RETURN, // Return [RETURN]


    // Type Conversion Operator(s):

    OP_ITOF, // Convert Integer To Float [INT_TO_FLOAT]
    OP_ITOB, // Convert Integer To Boolean [INT_TO_BOOL]
    OP_FTOI, // Convert Float To Integer [FLOAT_TO_INT]
    OP_FTOB, // Convert Float To Boolean [FLOAT_TO_BOOL]
    /* OP_CTOI, // Convert Character To Integer (ASCII) [CHAR_TO_INT] */
    OP_BTOI, // Convert Boolean To Integer [BOOL_TO_INT]
    OP_BTOF // Convert Boolean To Float [BOOL_TO_FLOAT]

} Quad_Op;

typedef struct {
    Quad_Op Operator;
    char* Argument1;
    char* Argument2;
    char* Result;
} Quadruple;

extern Quadruple Quadruples[MAX_QUADS];

extern int quadruplesCount;

char* createTemp ();

char* createLabel ();

const char* operatorToString (Quad_Op Operator);

void addQuadruple (Quad_Op Operator, const char* Argument1, const char* Argument2, const char* Result);

void printQuadruples ();

void printQuadruplesToFile(const char* filename);

void freeQuadruples ();

#endif