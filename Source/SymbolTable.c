#include "SymbolTable.h"
#include "ErrorHandler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static unsigned long long hashIdentifier(const char* key)
{
    unsigned long long hash = 1469598103934665603ULL;

    while (*key)
    {
        hash ^= (unsigned char)(*key);
        hash *= 1099511628211ULL;
        key++;
    }

    return hash;
}

static unsigned int getBucketIndex(const char* identifier)
{
    return (unsigned int)(hashIdentifier(identifier) % TABLE_SIZE);
}

static const char* typeToString(type t)
{
    switch (t)
    {
        case INT_TYPE:    return "int";
        case BOOL_TYPE:   return "bool";
        case FLOAT_TYPE:  return "float";
        case CHAR_TYPE:   return "char";
        case STRING_TYPE: return "string";
        case VOID_TYPE:   return "void";
        default:     return "unknown";
    }
}

static bool isTypeCompatible(type lhsType , type rhsType)
{
    char buffer[512] = "\0";

    switch (lhsType)
    {
    case INT_TYPE:
        if (rhsType == INT_TYPE || rhsType == FLOAT_TYPE)
        {
            return true;
        }
        else
        {
            strcat(buffer , "Type miss-match, attempt to assign ");
            strcat(buffer , typeToString(rhsType));
            strcat(buffer , " data into a variable of type {int}.\n");
            reportError(SEMANTIC_ERROR , buffer , previousValidLine);
            return false;
        }
        break;
    case FLOAT_TYPE:
        if (rhsType == INT_TYPE || rhsType == FLOAT_TYPE)
        {
            return true;
        }
        else
        {
            strcat(buffer , "Type miss-match, attempt to assign ");
            strcat(buffer , typeToString(rhsType));
            strcat(buffer , " data into a variable of type {float}.\n");
            reportError(SEMANTIC_ERROR , buffer , previousValidLine);
            return false;
        }
        break;
    case CHAR_TYPE:
        if (rhsType == CHAR_TYPE)
        {
            return true;
        }
        else 
        {
            strcat(buffer , "Type miss-match, attempt to assign ");
            strcat(buffer , typeToString(rhsType));
            strcat(buffer , " data into a variable of type {char}.\n");
            reportError(SEMANTIC_ERROR , buffer , previousValidLine);
            return false;
        }
        break;
    case BOOL_TYPE:
        if (rhsType == BOOL_TYPE)
        {
            return true;
        }
        else 
        {
            strcat(buffer , "Type miss-match, attempt to assign ");
            strcat(buffer , typeToString(rhsType));
            strcat(buffer , " data into a variable of type {bool}.\n");
            reportError(SEMANTIC_ERROR , buffer , previousValidLine);
            return false;
        }
        break;
    case STRING_TYPE:
        if (rhsType == STRING_TYPE)
        {
            return true;
        }
        else 
        {
            strcat(buffer , "Type miss-match, attempt to assign ");
            strcat(buffer , typeToString(rhsType));
            strcat(buffer , " data into a variable of type {string}.\n");
            reportError(SEMANTIC_ERROR , buffer , previousValidLine);
            return false;
        }
        break;
    case VOID_TYPE:
        strcat(buffer , "Void Data is not assignable\n");
        reportError(SEMANTIC_ERROR , buffer , previousValidLine);
        return false;
        break;
    default:
    return false;
        break;
    }
}

singleEntryNode* createNewEntry(type identifierType , const char* identifierName , variableORfunction varOrFunc , value currentValue , bool isInitialised , bool isReadOnly , Parameter* parameterList)
{
    if (identifierName == NULL)
    {
        perror("createNewEntry: identifierName is NULL");
        return NULL;
    }

    singleEntryNode* createdEntry = (singleEntryNode*)(malloc(sizeof(singleEntryNode)));

    
    if (createdEntry == NULL)
    {
        perror("Failed to allocate memory in createNewEntry func");
        return NULL;
    }
    
    /* init to NULL for safety */
    createdEntry->identifierType = identifierType;
    createdEntry->identifierName = NULL;
    createdEntry->varOrFunc = SYMBOL_VARIABLE;  
    createdEntry->currentValue.voidData = NULL;
    createdEntry->isInitialised = false;        
    createdEntry->isReadOnly = false;           
    createdEntry->isUsed = false;
    createdEntry->parameterList = NULL;
    createdEntry->nextEntry = NULL;


    /* Start Creating the Entry */
    createdEntry->identifierName = strdup(identifierName);

    if (createdEntry->identifierName == NULL)
    {
        perror("strdup failed to allocate memory");
        freeEntry(createdEntry);
        return NULL;
    }

    createdEntry->varOrFunc = varOrFunc;

    switch (varOrFunc)
    {
    case SYMBOL_FUNCTION:
        createdEntry->parameterList = parameterList;
        break;
    case SYMBOL_VARIABLE:
        createdEntry->parameterList = NULL;
        createdEntry->isInitialised = isInitialised;
        createdEntry->isReadOnly = isReadOnly;
        break;
    default:
        perror("Unexpected enum value FUNCTION or VARIABLE");
        freeEntry(createdEntry);
        return NULL;
        break;
    }

    if (createdEntry->isInitialised && createdEntry->varOrFunc == SYMBOL_VARIABLE)
    { /* Assigning the current value if it is initialised */
        if (identifierType == STRING_TYPE)
        {
            const char *src = currentValue.stringData ? currentValue.stringData : "";

            createdEntry->currentValue.stringData = strdup(src);
            if (createdEntry->currentValue.stringData == NULL)
            {
                freeEntry(createdEntry);
                return NULL;
            }
        }
        else
        {
            createdEntry->currentValue = currentValue;
        }
    }

    return createdEntry;
}

void printEntry(singleEntryNode* entryToPrint , FILE* printTo)
{
    // fprintf
    // (
    //     printTo , 
    //     "Name: %s , Type: %s , ",

    
    
    // );
}

void freeEntry(singleEntryNode* entryToDelete)
{
    if (entryToDelete == NULL)
    {
        return;
    }

    free(entryToDelete->identifierName);
    freeParameterList(entryToDelete->parameterList);
    
    if (entryToDelete->varOrFunc == SYMBOL_VARIABLE && entryToDelete->identifierType == STRING_TYPE && entryToDelete->isInitialised && entryToDelete->currentValue.stringData != NULL)
    {
        free(entryToDelete->currentValue.stringData);
    }

    free(entryToDelete);
    return;
}

void symbolTableInit(symbolTable* symTable)
{
    if (symTable == NULL)
    {
        return;
    }

    for (unsigned int i = 0; i < TABLE_SIZE; i++)
    {
        symTable->buckets[i] = NULL;
    }
}

singleEntryNode* searchFor(symbolTable* symTable, const char* identifier)
{
    if (symTable == NULL || identifier == NULL)
    {
        return NULL;
    }

    unsigned int index = getBucketIndex(identifier);
    singleEntryNode* bucket = symTable->buckets[index];

    while (bucket)
    {
        if (bucket->identifierName != NULL && !strcmp(bucket->identifierName , identifier))
        { /* strcmp returns 0 on true 8aba2 begad */
            return bucket;
        }
        bucket = bucket->nextEntry;
    }

    return NULL;
}

bool insertSymbolTableEntry(symbolTable* symTable, singleEntryNode* newEntry)
{
    if (symTable == NULL || newEntry == NULL)
    {
        return false;
    }

    if (searchFor(symTable , newEntry->identifierName) == NULL)
    {
        unsigned int index = getBucketIndex(newEntry->identifierName);
        newEntry->nextEntry = symTable->buckets[index];
        symTable->buckets[index] = newEntry;
        return true;
    }
    
    char buffer[512] = "\0";
    strcat(buffer , "Identifier (");
    strcat(buffer , newEntry->identifierName);
    strcat(buffer , ") is already declared in this scope.");

    reportError(SEMANTIC_ERROR , buffer , previousValidLine);
    freeEntry(newEntry);
    
    return false;
}

bool updateVariableValue(symbolTable* symTable, const char* identifier, type valueType , value newValue)
{
    if (symTable == NULL || identifier == NULL)
    { /* NULL Safety */
        return false;
    }

    char buffer[512] = "\0";
    singleEntryNode* temp = searchFor(symTable , identifier);

    if (temp == NULL)
    { /* Undeclared identifier */
        strcat(buffer, "Identifier (");
        strcat(buffer, identifier);
        strcat(buffer, ") is undelcared.\n");
        reportError(SEMANTIC_ERROR , buffer , previousValidLine);
        buffer[0] = "\0";
        return false;
    }

    if (temp->varOrFunc == SYMBOL_FUNCTION)
    { /* Attempt to assign data to function */
        strcat(buffer, "Identifier (");
        strcat(buffer, identifier);
        strcat(buffer, ") is a function, you cannot assign values to functions.\n");
        reportError(SEMANTIC_ERROR , buffer , previousValidLine);
        buffer[0] = "\0";
    }

    if (temp->isReadOnly)
    { /* Attempt to assign data to const identifiers */
        strcat(buffer, "Identifier (");
        strcat(buffer, identifier);
        strcat(buffer, ") is a read only, you cannot assign values to const identifiers.\n");
        reportError(SEMANTIC_ERROR , buffer , previousValidLine);
        buffer[0] = "\0";
    }

    return false;
    // go to the outer scope and search again

}

void freeSymbolTable(symbolTable* symTable)
{
    if (symTable == NULL)
    {
        return;
    }

    singleEntryNode* tempNode;
    singleEntryNode* next;
    for (unsigned int i = 0; i < TABLE_SIZE; i++)
    {
        tempNode = symTable->buckets[i];
        if (tempNode == NULL)
        { /* Empty Bucket */
            continue;
        }
        next = tempNode->nextEntry;
        while (next != NULL)
        {
            freeEntry(tempNode);
            tempNode = next;
            next = next->nextEntry;
        }
        freeEntry(tempNode);
    }
}

void printTable(symbolTable* symTable)
{
    FILE* symbolTableOutput;
    symbolTableOutput = fopen("Symbol_Table.txt" , "w");
    if (symbolTableOutput == NULL)
    {
        perror("Failed to open the symbol table's text file.");
        return;
    }

    fprintf(symbolTableOutput , "========== Symbol Table ==========\n\n");

    if (symTable == NULL)
    {
        fprintf(symbolTableOutput , "Empty Table");
        return;
    }

    singleEntryNode* tempNode;
    for (unsigned int i = 0; i < TABLE_SIZE; i++)
    {
        tempNode = symTable->buckets[i];
        if (tempNode == NULL)
        {
            continue;
        }
        
        while  (tempNode)
        {
            
        }
    }
}

void scopeStackInit(scopeStack* s)
{
    if (s == NULL)
    {
        return;
    }
    s->topScope = NULL;
    return;
}

bool enterScope(scopeStack* s)
{
    if (s == NULL)
    {
        return false;
    }

    scope* temp = (scope*)(malloc(sizeof(scope)));
    if (temp == NULL)
    {
        perror("Failed to allocate new memory for scopes.");
        return false;
    }

    temp->parentScope = s->topScope;
    temp->scopeDepth = temp->parentScope ? temp->parentScope->scopeDepth + 1 : 0;
    symbolTableInit(&temp->table);
    s->topScope = temp;
    return true;
}

bool exitScope(scopeStack* s)
{
    if (s == NULL)
    {
        return false;
    }

    if (s->topScope == NULL)
    {
        reportError(SYNTAX_ERROR ,  "Unexpected '}' without matching '{'." , previousValidLine);
        return false;
    }

    scope *top = s->topScope;
    scope *parent = top->parentScope;

    freeSymbolTable(&top->table);
    free(top);

    s->topScope = parent;
    return true;
}

scope* getCurrentScope(scopeStack* s)
{
    if (s == NULL || s->topScope == NULL)
    {
        return NULL;
    }

    return s->topScope;
}

scope* getGlobalScope(scopeStack* s)
{   /* Can be used for faster functions lookups */
    if (s == NULL)
    {
        return NULL;
    }
    scope* temp = s->topScope;
    
    while (temp != NULL && temp->parentScope != NULL)
    { /* if temp was null miss the loop and return null , else iterate on all scope and return the global one */
        temp = temp->parentScope;
    }
    return temp;
}

singleEntryNode* lookupCurrentScope(scopeStack* s, const char* identifierName)
{
    if (s == NULL || s->topScope == NULL || identifierName == NULL)
    {
        return NULL;
    }

    scope* searchScope = s->topScope;
    return searchFor(&searchScope->table , identifierName);
}

singleEntryNode* lookupAllScopes(scopeStack* s, const char* identifierName)
{
    if (s == NULL || identifierName == NULL)
    {
        return NULL;
    }

    singleEntryNode* temp;
    for (scope* tempSearchScope = s->topScope; tempSearchScope != NULL; tempSearchScope = tempSearchScope->parentScope)
    {
        temp = searchFor(&tempSearchScope->table , identifierName);
        if (temp != NULL)
        { /* returns a pointer to the first reference so we get the nearest copy from the nearest scope */
            return temp;
        }
    }
    /* Not found at all */
    return NULL;
}

singleEntryNode* functionLookup(scopeStack* s, const char* identifierName)
{
    if (s == NULL || identifierName == NULL)
    {
        return NULL;
    }

    scope* tempSearchScope = getGlobalScope(s);
    if (tempSearchScope == NULL)
    {
        return NULL;
    }

    return searchFor(&tempSearchScope->table , identifierName);
}

bool insertInCurrentScope(scopeStack* s, singleEntryNode* entry)
{
    if (s == NULL || s->topScope == NULL || entry == NULL)
    {
        return false;
    }

    scope* currentSearchScope = s->topScope;

    if (entry->varOrFunc == SYMBOL_FUNCTION && currentSearchScope->scopeDepth != 0)
    { /* functions must be global */
        char buffer[512] = "\0";
        strcat(buffer, "Function (");
        strcat(buffer, entry->identifierName);
        strcat(buffer, ") must be declared in the global scope.");
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);

        freeEntry(entry);
        return false;
    }

    if (searchFor(&currentSearchScope->table, entry->identifierName) != NULL)
    { /* Redeclaration check ONLY in current scope */
        char buffer[512] = "\0";
        strcat(buffer, "Identifier (");
        strcat(buffer, entry->identifierName);
        strcat(buffer, ") is already declared in this scope.");
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);

        freeEntry(entry);
        return false;
    }

    return insertSymbolTableEntry(&currentSearchScope->table, entry);
}

bool updateVariableValueScoped(scopeStack* s, const char* identifierName, type rhsType, value rhsVal)
{
    if (s == NULL || identifierName == NULL)
    {
        return false;
    }
    char buffer[512] = "\0";

    singleEntryNode* target = lookupAllScopes(s, identifierName);
    if (target == NULL)
    {
        strcat(buffer, "Identifier (");
        strcat(buffer, identifierName);
        strcat(buffer, ") is undeclared.");
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);
        return false;
    }

    if (target->varOrFunc == SYMBOL_FUNCTION)
    {
        strcat(buffer, "Identifier (");
        strcat(buffer, identifierName);
        strcat(buffer, ") is a function; cannot assign values to functions.");
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);
        return false;
    }

    if (target->isReadOnly)
    {
        strcat(buffer, "Identifier (");
        strcat(buffer, identifierName);
        strcat(buffer, ") is read-only; cannot assign to const identifiers.");
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);
        return false;
    }
    
    if (!isTypeCompatible(target->identifierType, rhsType))
    { /* Type check: LHS is the variable type stored in the symbol table */
        // Report error
        return false;
    }
    
    if (target->identifierType == STRING_TYPE)
    { /* free old string if it existed */
        if (target->isInitialised && target->currentValue.stringData != NULL)
        {
            free(target->currentValue.stringData);
            target->currentValue.stringData = NULL;
        }

        const char* src = (rhsVal.stringData != NULL) ? rhsVal.stringData : "";
        target->currentValue.stringData = strdup(src);

        if (target->currentValue.stringData == NULL)
        {
            perror("strdup failed while assigning string");
            return false;
        }
    }
    else
    { /* For numeric/bool/char etc. union copy is OK */
        target->currentValue = rhsVal;
    }

    target->isInitialised = true;
    return true;
}

// int main(void)
// {
//     printf("Program started\n");
//     printf("Index = %u\n", getBucketIndex("variable"));
//     return 0;
// }