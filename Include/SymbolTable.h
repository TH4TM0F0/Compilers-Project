#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "Utils.h"
#include "Parameter.h"

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

typedef struct scope
{
    symbolTable table;
    struct scope* parentScope;
    int scopeDepth;
} scope;

typedef struct 
{
    scope* topScope;
} scopeStack;

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
// void printEntry(singleEntryNode* entryToPrint , FILE* printTo);
void freeEntry(singleEntryNode* entryToDelete);

/* Table ops */
void symbolTableInit(symbolTable* symTable);
singleEntryNode* searchFor(symbolTable* symTable, const char* identifier);
bool insertSymbolTableEntry(symbolTable* symTable, singleEntryNode* newEntry);
bool updateVariableValue(symbolTable* symTable, const char* identifier, type valueType , value newValue);

/* Freeing */
void freeSymbolTable(symbolTable* symTable);

/* Scope ops */
void scopeStackInit(scopeStack* s);
bool enterScope(scopeStack* s);
bool exitScope(scopeStack* s);          
scope* getCurrentScope(scopeStack* s);
scope* getGlobalScope(scopeStack* s);
singleEntryNode* lookupCurrentScope(scopeStack* s, const char* identifierName);
singleEntryNode* lookupAllScopes(scopeStack* s, const char* identifierName);
singleEntryNode* functionLookup(scopeStack* s, const char* identifierName);
bool insertInCurrentScope(scopeStack* s, singleEntryNode* entry);
bool updateVariableValueScoped(scopeStack* s, const char* identifierName, type rhsType, value rhsVal);

void dumpScopeStackToFile(scopeStack* s, const char* filename);

#endif