#include "SymbolTable.h"

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
        symTable->buckets[index]->nextEntry = newEntry;
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

bool updateVariableValue(symbolTable* symTable, const char* identifier, value newValue)
{
    if (symTable == NULL || identifier == NULL || newValue.voidData == NULL)
    {
        return false;
    }
    
    if (searchFor(symTable , identifier) == NULL)
    {
        char buffer[512] = "\0";
        strcat(buffer, "Identifier (");
        strcat(buffer, identifier);
        strcat(buffer, ") is undelcared.");
        reportError(SEMANTIC_ERROR , buffer , previousValidLine);
        return false;
    }
    // go to the outer scope and search again

}

void symbolTableFree(symbolTable* symTable)
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


// int main(void)
// {
//     printf("Program started\n");
//     printf("Index = %u\n", getBucketIndex("variable"));
//     return 0;
// }
