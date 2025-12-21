#include "Utils.h"
#include <stdlib.h>

const char* typeToString(type t)
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

bool isTypeCompatible(type lhsType , type rhsType)
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
    return false;
}

char** split(const char* str, const char* delimiter, int* count) {
    char* str_copy = strdup(str);
    if (!str_copy) {
        return NULL;
    }

    char** result = malloc(sizeof(char*) * 10);
    if (!result) {
        free(str_copy);
        return NULL;
    }

    int index = 0;
    char* token = strtok(str_copy, delimiter);

    // Loop through the string and split it based on the delimiter
    while (token != NULL) {
        result[index] = strdup(token);
        if (!result[index]) {
            for (int i = 0; i < index; i++) {
                free(result[i]);
            }
            free(result);
            free(str_copy);
            return NULL;
        }
        index++;

        if (index % 10 == 0) {
            result = realloc(result, sizeof(char*) * (index + 10));
            if (!result) {
                for (int i = 0; i < index; i++) {
                    free(result[i]);
                }
                free(str_copy);
                return NULL;
            }
        }

        token = strtok(NULL, delimiter);
    }

    *count = index;
    free(str_copy);
    return result;
}

void free_split_result(char** result, int count) {
    for (int i = 0; i < count; i++) {
        free(result[i]);
    }
    free(result);
}