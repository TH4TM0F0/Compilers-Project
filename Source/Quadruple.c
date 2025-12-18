#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Quadruple.h"

Quadruple Quadruples[MAX_QUADS];

int quadruplesCount = 0;

int nextTemp = 1, nextLabel = 1;

char* createTemp () {
    char* temp = malloc(10);
    snprintf(temp, 10, "t%d", nextTemp++);
    return temp;
}

char* createLabel () {
    char* label = malloc(10);
    snprintf(label, 10, "L%d", nextLabel++);
    return label;
}

const char* operatorToString (Op Operator) {
    switch (Operator) {
        // Arithmetic Operator(s):
        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "*";
        case DIV: return "/";
        case MOD: return "%";
        case EXP: return "**";
        case UMIN: return "UMINUS"; // <---
        case INC: return "++";
        case DEC: return "--";
        // Assignment Operator(s):
        case ASSN: return "=";
        // Relational Operator(s):
        case LT: return "<";
        case GT: return ">";
        case LTE: return "<=";
        case GTE: return ">=";
        case EQ: return "==";
        case NEQ: return "!=";
        // Logical Operator(s):
        case AND: return "&&";
        case OR: return "||";
        case NOT: return "!";
        // Control Flow Operator(s):
        case GOTO: return "GO_TO";
        case IFGOTO: return "IF_GO_TO";
        case IFFALSE: return "IF_FALSE";
        case LABEL: return "LABEL"; // <---
        // Fuction Calls & Stack Handling Operator(s):
        case CALL: return "CALL";
        case PARM: return "PARM";
        case RETURN: return "RETURN";
        // Type Conversion Operator(s):
        case ITOF: return "INT_TO_FLOAT";
        case FTOI: return "FLOAT_TO_INT";
        case CTOI: return "CHAR_TO_INT";
        case ITOB: return "INT_TO_BOOL";
        // Anomaly Operator:
        default: return "UNKNOWN";
    }
}

void addQuadruple (Op Operator, const char* Argument1, const char* Argument2, const char* Result) {
    if (quadruplesCount >= MAX_QUADS) {
        fprintf(stderr, "Error: Exceeded Maximum Quadruples!\n");
        exit(1);
    }
    Quadruples[quadruplesCount].Operator = Operator;
    Quadruples[quadruplesCount].Argument1 = Argument1 ? strdup(Argument1): NULL;
    Quadruples[quadruplesCount].Argument2 = Argument2 ? strdup(Argument2): NULL;
    Quadruples[quadruplesCount].Result = Result ? strdup(Result): NULL;
    quadruplesCount++;
}

void printQuadruples () {
    printf("\n=== Generated Quadruples ===\n");
    for (int i = 0; i < quadruplesCount; i++) {
        printf("[%d] (%s, %s, %s, %s)\n", 
            i,
            operatorToString(Quadruples[i].Operator),
            Quadruples[i].Argument1 ? Quadruples[i].Argument1 : "NONE",
            Quadruples[i].Argument2 ? Quadruples[i].Argument2 : "NONE",
            Quadruples[i].Result ? Quadruples[i].Result : "NONE"
        );
    }
}

void freeQuadruples () {
    for (int i = 0; i < quadruplesCount; i++) {
        free(Quadruples[i].Argument1);
        free(Quadruples[i].Argument2);
        free(Quadruples[i].Result);
    }
    quadruplesCount = 0;
}