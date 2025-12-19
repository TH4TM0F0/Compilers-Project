#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Parameter.h"

Parameter* createParameter(const char* Name, const char* Type) {
    if (!Name || !Type) {
        fprintf(stderr, "Error: Invalid Parameters Missing Name Or Type!\n");
        return NULL;
    }
    Parameter* parameter = (Parameter*)malloc(sizeof(Parameter));
    if (!parameter) return NULL;
    parameter->Name = strdup(Name);
    parameter->Type = strdup(Type);
    parameter->Next = NULL;
    return parameter;
}

Parameter* addParameter(Parameter* Head, Parameter* parameter) {
    if (!parameter) return Head;
    if (!Head) return parameter;
    Parameter* Current = Head;
    while (Current->Next) {
        Current = Current->Next;
    }
    Current->Next = parameter;
    return Head;
}

void freeParameterList(Parameter* Head) {
    while (Head) {
        Parameter* Temp = Head;
        Head = Head->Next;
        free(Temp->Name);
        free(Temp->Type);
        free(Temp);
    }
}

void printParameterList(const Parameter* Head) {
    int i = 1;
    while (Head) {
        printf("Parameter %d: Name = %s, Type = %s\n", i, Head->Name, Head->Type);
        Head = Head->Next;
        i++;
    }
}

char* parameterListToString(const Parameter* Head) {
    static char String[1024];
    String[0] = '\0';
    while (Head) {
        strcat(String, Head->Type);
        strcat(String, " ");
        strcat(String, Head->Name);
        if (Head->Next) strcat(String, ", ");
        Head = Head->Next;
    }
    if (strlen(String) == 0) strcpy(String, "No Parameters");
    return String;
}

int compareParameters(Parameter* definedHead, Parameter* calledHead) {
    while (definedHead && calledHead) {
        if (strcmp(definedHead->Type, calledHead->Type) != 0) return 0;
        definedHead = definedHead->Next;
        calledHead = calledHead->Next;
    }
    return (definedHead == NULL && calledHead == NULL);
}