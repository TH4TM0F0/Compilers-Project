#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ErrorHandler.h"

#define MAX_ERRORS 1000

typedef struct {
    ErrorType Type;
    char Message[256];
    int Line;
} Error;

static Error Errors[MAX_ERRORS];

static int errorsCount = 0;

void reportError(ErrorType Type, const char* Message, int Line) {
    if (errorsCount >= MAX_ERRORS) return;
    Errors[errorsCount].Type = Type;
    strncpy(Errors[errorsCount].Message, Message, 255);
    Errors[errorsCount].Line = Line;
    errorsCount++;
}

void printErrors() {
    for (int i = 0; i < errorsCount; i++) {
        const char* Type = (Errors[i].Type == SYNTAX_ERROR) ? "Syntax" : "Semantic";
        printf("[%s Error] Line %d: %s\n", Type, Errors[i].Line, Errors[i].Message);
    }
}

int getErrorCount() { 
    return errorsCount; 
}