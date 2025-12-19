#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

typedef enum {
    SYNTAX_ERROR,
    SEMANTIC_ERROR
} ErrorType;

void reportError(ErrorType Type, const char* Message, int Line);

void printErrors();

int getErrorCount();

#endif