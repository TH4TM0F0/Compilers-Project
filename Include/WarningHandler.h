#ifndef WARNING_HANDLER_H
#define WARNING_HANDLER_H

typedef enum
{
    UNUSED_VARIABLE,
    UNUSED_FUNCTION
} WarningType;

void reportWarning(WarningType Type, const char* Message);

void printWarnings();

int getWarningCount();

#endif