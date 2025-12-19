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

const char* operatorToString (Quad_Op Operator) {
    switch (Operator) {
        // Arithmetic Operator(s):
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%%";
        case OP_EXP: return "**";
        case OP_UMIN: return "UMINUS"; // <---
        case OP_INC: return "++";
        case OP_DEC: return "--";
        // Assignment Operator(s):
        case OP_ASSN: return "=";
        // Relational Operator(s):
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LTE: return "<=";
        case OP_GTE: return ">=";
        case OP_EQ: return "==";
        case OP_NEQ: return "!=";
        // Logical Operator(s):
        case OP_AND: return "&&";
        case OP_OR: return "||";
        case OP_NOT: return "!";
        // Control Flow Operator(s):
        case OP_GOTO: return "GO_TO";
        case OP_IFGOTO: return "IF_GO_TO";
        case OP_IFFALSE: return "IF_FALSE";
        case OP_LABEL: return "LABEL"; // <---
        // Fuction Calls & Stack Handling Operator(s):
        case OP_CALL: return "CALL";
        case OP_PARM: return "PARM";
        case OP_RETURN: return "RETURN";
        // Type Conversion Operator(s):
        case OP_ITOF: return "INT_TO_FLOAT";
        case OP_FTOI: return "FLOAT_TO_INT";
        case OP_CTOI: return "CHAR_TO_INT";
        case OP_ITOB: return "INT_TO_BOOL";
        // Anomaly Operator:
        default: return "UNKNOWN";
    }
}

void addQuadruple (Quad_Op Operator, const char* Argument1, const char* Argument2, const char* Result) {
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