#ifndef PARAMETER_H
#define PARAMETER_H

typedef struct Parameter {

    char* Name;
    char* Type;
    struct Parameter* Next;

} Parameter;

Parameter* createParameter(const char* Name, const char* Type);

Parameter* addParameter(Parameter* Head, Parameter* parameter);

void freeParameterList(Parameter* Head);

void printParameterList(const Parameter* Head);

char* parameterListToString(const Parameter* Head);

int compareParameters(Parameter* definedHead, Parameter* calledHead);


#endif