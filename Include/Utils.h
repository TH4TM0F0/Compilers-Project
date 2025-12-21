#ifndef UTILS_H
#define UTILS_H

#include<stdbool.h>
#include<string.h>
#include "ErrorHandler.h"
extern int previousValidLine;

typedef enum 
{
    INT_TYPE,    /* 0 */
    BOOL_TYPE,   /* 1 */
    FLOAT_TYPE,  /* 2 */
    CHAR_TYPE,   /* 3 */
    STRING_TYPE, /* 4 */
    VOID_TYPE    /* 5 */
} type;

typedef enum
{
    SYMBOL_VARIABLE,   /* 0 */
    SYMBOL_FUNCTION    /* 1 */
} variableORfunction;

typedef union 
{
    int INT_Data;        /* 0 */
    bool BOOL_Data;      /* 1 */
    float FLOAT_Data;    /* 2 */
    char CHAR_Data;      /* 3 */
    char* STRING_Data;   /* 4 */
    void* VOID_Data;     /* 5 */
} value;

typedef struct Expression
{
    type expressionType;
    value expressionValue;
    char *temp_var;
} Expression;


const char* typeToString(type t);

bool isTypeCompatible(type lhsType , type rhsType);

char** split(const char* str, const char* delimiter, int* count);

void free_split_result(char** result, int count);

#endif