#include "Utils.h"

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
}