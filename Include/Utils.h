#ifndef UTILS_H
#define UTILS_H

#include<stdbool.h>

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
    int intData;        /* 0 */
    bool boolData;      /* 1 */
    float floatData;    /* 2 */
    char charData;      /* 3 */
    char* stringData;   /* 4 */
    void* voidData;     /* 5 */
} value;

#endif