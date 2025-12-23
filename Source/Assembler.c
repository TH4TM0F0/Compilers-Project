#include <stdio.h>
#include <string.h>
#include "Quadruple.h"

static const char* removeNone (const char* string) {
    return (string && strcmp(string, "NONE") != 0) ? string : "";
}

void quadruplesToAssembly (const char *filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Failed Opening %s!\n", filename);
        return;
    }

    char lastJump[128] = "";

    for (int i = 0; i < quadruplesCount; i++) {

        Quadruple currentQuadruple = Quadruples[i];
        const char* Argument1 = currentQuadruple.Argument1 ? currentQuadruple.Argument1 : "NONE";
        const char* Argument2 = currentQuadruple.Argument2 ? currentQuadruple.Argument2 : "NONE";
        const char* Result = currentQuadruple.Result ? currentQuadruple.Result : "NONE";

        switch (currentQuadruple.Operator) {
            // Arithmetic Operator(s):
            case OP_ADD: 
                fprintf(file, "ADD %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_SUB:
                fprintf(file, "SUB %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_MUL:
                fprintf(file, "MUL %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_DIV:
                fprintf(file, "DIV %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_MOD:
                fprintf(file, "MOD %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_EXP:
                fprintf(file, "EXP %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_UMIN:
                fprintf(file, "NEG %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            case OP_INC:
                fprintf(file, "INC %s\n", removeNone(Result));
                break;

            case OP_DEC:
                fprintf(file, "DEC %s\n", removeNone(Result));
                break;

            // Assignment Operator(s):
            case OP_ASSN:
                fprintf(file, "MOV %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            // Relational Operator(s):
            case OP_LT:
                fprintf(file, "LT %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_GT:
                fprintf(file, "GT %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_LTE:
                fprintf(file, "LTE %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_GTE:
                fprintf(file, "GTE %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_EQ:
                fprintf(file, "EQ %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_NEQ:
                fprintf(file, "NEQ %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            // Logical Operator(s):
            case OP_AND:
                fprintf(file, "AND %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_OR:
                fprintf(file, "OR %s, %s, %s\n", removeNone(Result), removeNone(Argument1), removeNone(Argument2));
                break;

            case OP_NOT:
                fprintf(file, "NOT %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            // Control Flow Operator(s):
            case OP_GOTO:
                if ((Result) && strcmp((Result), "NONE") != 0 && strlen((Result)) > 0) {
                    char Jump[128];
                    snprintf(Jump, sizeof(Jump), "JMP %s", Result);
                    if (strcmp(Jump, lastJump) != 0) {
                        fprintf(file, "%s\n", Jump);
                        strcpy(lastJump, Jump);
                    }
                }
                else 
                    fprintf(file, ";\n");
                break;

            case OP_IFGOTO:
                if ((Result) && strcmp((Result), "NONE") != 0 && strlen((Result)) > 0)
                    fprintf(file,"JNZ %s, %s\n", removeNone(Argument1), Result);
                else 
                    fprintf(file, ";\n");
                break;

            case OP_IFFALSE:
                if ((Result) && strcmp((Result), "NONE") != 0 && strlen((Result)) > 0)
                    fprintf(file, "JZ %s, %s\n", removeNone(Argument1), Result);
                else
                    fprintf(file, ";\n");
                break;

            case OP_LABEL:
                if ((Result) && strcmp((Result), "NONE") != 0 && strlen((Result)) > 0)
                    fprintf(file, "\n%s:\n", Result);
                else
                    fprintf(file, ";\n");
                break;

            // Fuction Calls & Stack Handling Operator(s):
            case OP_CALL:
                fprintf(file, "CALL %s\n", removeNone(Argument1));
                if ((Result) && strcmp((Result), "NONE") != 0 && strlen((Result)) > 0)
                    fprintf(file, "MOV %s, EAX\n", Result);
                break;

            case OP_PARM:
                if ((Argument1) && strcmp((Argument1), "NONE") != 0 && strlen((Argument1)) > 0)
                    fprintf(file, "PUSH %s\n", Argument1);
                else
                    fprintf(file, ";\n");
                break;

            case OP_RETURN:
                if ((Argument1) && strcmp((Argument1), "NONE") != 0 && strlen((Argument1)) > 0)
                    fprintf(file, "MOV EAX, %s\n", Argument1);
                fprintf(file, "RET\n");
                break;

            // Type Conversion Operator(s):
            case OP_ITOF:
                fprintf(file, "ITOF %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            case OP_ITOB:
                fprintf(file, "ITOB %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            case OP_FTOI:
                fprintf(file, "FTOI %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            case OP_FTOB:
                fprintf(file, "FTOB %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            case OP_BTOI:
                fprintf(file, "BTOI %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;
                
            case OP_BTOF:
                fprintf(file, "BTOF %s, %s\n", removeNone(Result), removeNone(Argument1));
                break;

            // case OP_CTOI:
            //     fprintf(file, "CTOI %s, %s\n", removeNone(Result), removeNone(Argument1));
            //     break;
            
            // Anomaly Operator:
            default:
                fprintf(file, ";\n");
                break;
        }
    }
    fclose(file);
}