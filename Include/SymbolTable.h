#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "Parameter.h"
#include "ErrorHandler.h"
#include "Utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef TABLE_SIZE
#define TABLE_SIZE 211
#endif

extern int previousValidLine;


typedef struct singleEntryNode
{
    type identifierType;                /* BOTH */
    char* identifierName;               /* BOTH */
    value currentValue;                 /* VARS */

    variableORfunction varOrFunc;       /* BOTH */

    bool isInitialised;                 /* VARS */
    bool isReadOnly;                    /* VARS */
    bool isUsed;                        /* BOTH */

    Parameter* parameterList;           /* FUNCS */
    struct singleEntryNode* nextEntry;  /* chaining */
} singleEntryNode;

typedef struct
{
    singleEntryNode* buckets[TABLE_SIZE];
} symbolTable;

/* Entry creation */
singleEntryNode* createNewEntry(
    type identifierType,
    const char* identifierName,
    variableORfunction varOrFunc,
    value currentValue,
    bool isInitialised,
    bool isReadOnly,
    Parameter* parameterList
);
void printEntry(singleEntryNode* entryToPrint);
void freeEntry(singleEntryNode* entryToDelete);

/* Table ops */
void symbolTableInit(symbolTable* symTable);
singleEntryNode* searchFor(symbolTable* symTable, const char* identifier);
bool insertSymbolTableEntry(symbolTable* symTable, singleEntryNode* newEntry);
bool updateVariableValue(symbolTable* symTable, const char* identifier, value newValue);

/* Table Utils*/
void printTable(symbolTable* symTable);

/* Freeing */
void symbolTableFree(symbolTable* symTable);
void freeEntry(singleEntryNode* entry);

#endif