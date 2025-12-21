#ifndef PARAMETER_H
#define PARAMETER_H

#include "Utils.h"
#include "ErrorHandler.h"

typedef struct Parameter 
{

    char* Name;
    type Type;
    struct Parameter* Next;

} Parameter;

Parameter* createParameter(const char* Name, const type Type);

Parameter* addParameter(Parameter* Head, Parameter* parameter);

void freeParameterList(Parameter* Head);

void printParameterList(const Parameter* Head);

char* parameterListToString(const Parameter* Head);

int compareParameters(Parameter* definedHead, Parameter* calledHead);


#endif