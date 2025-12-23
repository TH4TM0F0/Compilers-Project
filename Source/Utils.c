#include "Utils.h"
#include <stdlib.h>
#include <stdio.h>

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

const char* valueToString(type t, value v)
{
    static char buffer[128];   // static lifetime
    buffer[0] = '\0';

    switch (t)
    {
        case INT_TYPE:
            snprintf(buffer, sizeof(buffer), "%d", v.INT_Data);
            break;

        case FLOAT_TYPE:
            snprintf(buffer, sizeof(buffer), "%f", v.FLOAT_Data);
            break;

        case CHAR_TYPE:
            snprintf(buffer, sizeof(buffer), "'%c'", v.CHAR_Data);
            break;

        case BOOL_TYPE:
            snprintf(buffer, sizeof(buffer), "%s", v.BOOL_Data ? "true" : "false");
            break;

        case STRING_TYPE:
            snprintf(
                buffer,
                sizeof(buffer),
                "\"%s\"",
                v.STRING_Data ? v.STRING_Data : ""
            );
            break;

        case VOID_TYPE:
            snprintf(buffer, sizeof(buffer), "<void>");
            break;

        default:
            snprintf(buffer, sizeof(buffer), "<unknown>");
            break;
    }

    return buffer;
}

static bool isNumericType(type t) 
{ 
        return t == INT_TYPE || t == FLOAT_TYPE || t == BOOL_TYPE;
}

bool isTypeCompatible(type lhsType, type rhsType)
{
    char buffer[512] = "\0";

    if (isNumericType(lhsType) && isNumericType(rhsType)) 
    {
        return true;
    }

    if (lhsType == CHAR_TYPE) 
    {
        if (rhsType == CHAR_TYPE) 
        {
            return true;
        }

        strcat(buffer, "Type mismatch: char cannot take ");
        strcat(buffer, typeToString(rhsType));
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);
        return false;
    }

    if (lhsType == STRING_TYPE) 
    {
        if (rhsType == STRING_TYPE) 
        {
            return true;
        }

        strcat(buffer, "Type mismatch: string cannot take ");
        strcat(buffer, typeToString(rhsType));
        reportError(SEMANTIC_ERROR, buffer, previousValidLine);
        return false;
    }

    if (lhsType == VOID_TYPE) 
    {
        reportError(SEMANTIC_ERROR, "Void data is not assignable", previousValidLine);
        return false;
    }

    // For anything else:
    strcat(buffer, "Type mismatch: cannot assign ");
    strcat(buffer, typeToString(rhsType));
    strcat(buffer, " to ");
    strcat(buffer, typeToString(lhsType));
    reportError(SEMANTIC_ERROR, buffer, previousValidLine);
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