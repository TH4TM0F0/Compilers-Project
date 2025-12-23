#include "WarningHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WARNINGS 1000

typedef struct 
{
    WarningType Type;
    char Message[256];
} Warning;

static Warning Warnings[MAX_WARNINGS];

static int warningsCount = 0;

void reportWarning(WarningType Type, const char* Message) 
{
    if (warningsCount >= MAX_WARNINGS) return;
    Warnings[warningsCount].Type = Type;
    strncpy(Warnings[warningsCount].Message, Message, 255);
    warningsCount++;
}

void printWarnings() 
{
    for (int i = 0; i < warningsCount; i++) 
    {
        const char* Type = (Warnings[i].Type == UNUSED_VARIABLE) ? "Unused Variable" : "Unused Function";
        printf("[%s Warning] %s\n", Type, Warnings[i].Message);
    }
}

int getWarningCount() 
{ 
    return warningsCount; 
}