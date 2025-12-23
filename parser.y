// Definitions:
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <math.h>
    #include "Quadruple.h"
    #include "ErrorHandler.h"
    #include "WarningHandler.h"
    #include "Parameter.h"
    #include "Assembler.h"
    #include "SymbolTable.h"
    #include "Utils.h"

    extern FILE* yyin;
    extern int yylex();
    extern int yyparse();
    extern int previousValidLine;

    void yyerror(const char* String);

    char *break_label_stack[100];
    char *continue_label_stack[100];
    int loop_label_top = -1;

    type current_function_return_type = VOID_TYPE;

    char* get_break_label() {
        if (loop_label_top >= 0) return break_label_stack[loop_label_top];
        return NULL;
    }

    char* get_continue_label() {
        if (loop_label_top >= 0) return continue_label_stack[loop_label_top];
        return NULL;
    }

    void push_loop_labels(char *break_label, char *continue_label) {
        loop_label_top++;
        break_label_stack[loop_label_top] = break_label;
        continue_label_stack[loop_label_top] = continue_label;
    }

    void pop_loop_labels() {
        if (loop_label_top >= 0) loop_label_top--;
    }

    char *current_switch_var = NULL;
    char *current_switch_end_label = NULL;
    char *current_case_next_label = NULL;
    char *default_label = NULL;

    typedef struct DeclNode {
        char* name;
        struct Expression* initExpr;
        int line;
        struct DeclNode* next;
    } DeclNode;

    static DeclNode* makeDeclNode(const char* name, struct Expression* initExpr, int line) {
        DeclNode* n = (DeclNode*)malloc(sizeof(DeclNode));
        n->name = strdup(name);
        n->initExpr = initExpr;
        n->line = line;
        n->next = NULL;
        return n;
    }

    static DeclNode* appendDeclNode(DeclNode* list, DeclNode* tail) {
        if (!list) return tail;
        DeclNode* cur = list;
        while (cur->next) cur = cur->next;
        cur->next = tail;
        return list;
    }

    static void freeDeclList(DeclNode* list) {
        while (list) {
            DeclNode* nxt = list->next;
            free(list->name);
            free(list);
            list = nxt;
        }
    }

    typedef struct ArgNode {
        struct Expression* expr;
        struct ArgNode* next;
    } ArgNode;

    static ArgNode* makeArgNode(struct Expression* e) {
        ArgNode* a = (ArgNode*)malloc(sizeof(ArgNode));
        a->expr = e;
        a->next = NULL;
        return a;
    }

    static ArgNode* appendArgNode(ArgNode* list, ArgNode* tail) {
        if (!list) return tail;
        ArgNode* cur = list;
        while (cur->next) cur = cur->next;
        cur->next = tail;
        return list;
    }

    static void freeArgList(ArgNode* list) {
        while (list) {
            ArgNode* nxt = list->next;
            free(list);
            list = nxt;
        }
    }

    static type datatypeStringToType(const char* s) {
        if (!s) return VOID_TYPE;
        if (strcmp(s, "int") == 0) return INT_TYPE;
        if (strcmp(s, "float") == 0) return FLOAT_TYPE;
        if (strcmp(s, "bool") == 0) return BOOL_TYPE;
        if (strcmp(s, "char") == 0) return CHAR_TYPE;
        if (strcmp(s, "string") == 0) return STRING_TYPE;
        if (strcmp(s, "void") == 0) return VOID_TYPE;
        return VOID_TYPE;
    }

    static char* exprToOperand(struct Expression* e) {
        if (!e) return NULL;
        if (e->temp_var) return strdup(e->temp_var);
        char buf[256];
        switch (e->expressionType)
        {
            case INT_TYPE:
                snprintf(buf, sizeof(buf), "%d", e->expressionValue.INT_Data);
                return strdup(buf);

            case FLOAT_TYPE:
                snprintf(buf, sizeof(buf), "%f", e->expressionValue.FLOAT_Data);
                return strdup(buf);

            case BOOL_TYPE:
                snprintf(buf, sizeof(buf), "%d", e->expressionValue.BOOL_Data ? 1 : 0);
                return strdup(buf);

            case CHAR_TYPE:
                snprintf(buf, sizeof(buf), "%c", e->expressionValue.CHAR_Data);
                return strdup(buf);

            case STRING_TYPE:
                return e->expressionValue.STRING_Data ? strdup(e->expressionValue.STRING_Data) : strdup("");

            default:
                return NULL;
        }
    }

    static struct Expression* makeExpr(type t, value v, const char* temp) {
        struct Expression* e = (struct Expression*)malloc(sizeof(struct Expression));
        if (!e) return NULL;
        e->expressionType = t;
        e->expressionValue = v;
        e->temp_var = temp ? strdup(temp) : NULL;
        return e;
    }

    static struct Expression* makeTempExpr(type t, const char* tempName) {
        value v;
        memset(&v, 0, sizeof(v));
        return makeExpr(t, v, tempName);
    }

    static type arithmeticResultType(type a, type b) {
        if (a == FLOAT_TYPE || b == FLOAT_TYPE) return FLOAT_TYPE;
        return INT_TYPE;
    }

    static Parameter* makeParam(type t, const char* name) { return createParameter(name, t); }

    static Parameter* appendParam(Parameter* list, Parameter* tail) { return addParameter(list, tail); }

    scopeStack gScopeStack;

    bool isNumericType(type t) { return t == INT_TYPE || t == FLOAT_TYPE || t == CHAR_TYPE; }

    int boolToInt(bool b) { return b ? 1 : 0; }

    float boolToFloat(bool b) { return b ? 1.0f : 0.0f; }

    bool intToBool(int i) { return i != 0; }

    float intToFloat(int i) { return (float)i; }

    int floatToInt(float f) { return (int)f; }

    bool floatToBool(float f) { return f != 0.0f; }

    static bool isNumeric(type t) { return t == INT_TYPE || t == FLOAT_TYPE || t == BOOL_TYPE; }

    // static void requireNumeric(type t, int line, const char* msg) {
    //     if (!isNumeric(t)) reportError(SEMANTIC_ERROR, msg, line);
    // }

    // static type commonArithType(type a, type b) {
    //     if (a == FLOAT_TYPE || b == FLOAT_TYPE) return FLOAT_TYPE;
    //     return INT_TYPE;
    // }

    static Expression* castTo(Expression* e, type target) {
        if (!e) return NULL;
        if (e->expressionType == target) return e; 
        if (!isNumeric(e->expressionType) || !isNumeric(target)) {
            reportError(SEMANTIC_ERROR, "Invalid cast involving non-numeric type", previousValidLine);
            return e;
        }
        char* src = exprToOperand(e);
        char* t = createTemp();
        if (e->expressionType == INT_TYPE && target == FLOAT_TYPE) addQuadruple(OP_ITOF, src, NULL, t);
        else if (e->expressionType == BOOL_TYPE && target == FLOAT_TYPE) addQuadruple(OP_ITOF, src, NULL, t);
        else if (e->expressionType == FLOAT_TYPE && target == INT_TYPE) addQuadruple(OP_FTOI, src, NULL, t);
        else if (e->expressionType == INT_TYPE && target == BOOL_TYPE) addQuadruple(OP_ITOB, src, NULL, t);
        else if (e->expressionType == FLOAT_TYPE && target == BOOL_TYPE) addQuadruple(OP_FTOB, src, NULL, t);
        else if (e->expressionType == BOOL_TYPE && target == INT_TYPE) addQuadruple(OP_BTOI, src, NULL, t);
        else {
            reportError(SEMANTIC_ERROR, "Unsupported numeric cast", previousValidLine);
            free(src); 
            free(t);
            return e;
        }
        free(src);
        Expression* out = makeTempExpr(target, t);
        free(t);
        return out;
    }

    // static Expression* toBoolExpr(Expression* e, int line) {
    //     if (!e) return NULL;
    //     if (!isNumeric(e->expressionType)) {
    //         reportError(SEMANTIC_ERROR, "Logical operators require int/float/bool", line);
    //         return e;
    //     }
    //     return castTo(e, BOOL_TYPE);
    // }

    static Parameter* pending_function_params = NULL;
    static bool pending_params_should_insert = false;

    static bool inFunctionScope = false;
    static bool seen1ReturnStatement = false;

%}

%locations

%code requires
{
    #include "Utils.h"
    #include "SymbolTable.h"
    #include "Parameter.h"
    typedef struct Expression Expression;
    typedef struct DeclNode DeclNode;
    typedef struct ArgNode ArgNode;
}

// Data Types:
%union
{
    int intData;
    float floatData;
    char charData;
    char* stringData;
    void* voidData;
    DeclNode* declList;
    Expression* expr;
    Parameter* parameterList;  
    ArgNode* argList;          
    struct
    {
        char* code;
        char* true_label;
        char* false_label;
        char* next_label;
        char* start_label;
        char* end_label;
        char* cond_label;
        char* incr_label;
    } codeUtils;
}

// Tokens:
%token IF ELSE WHILE REPEAT UNTIL FOR SWITCH CASE CONST RETURN CONTINUE BREAK DEFAULT FUNCTION
%token OR AND NOT GT GE ST SE EQ NEQ
%token PLUS MINUS MULTIPLY DIVIDE MODULO POWER
%token INCREMENT DECREMENT
%token EQUAL SEMI_COLON COMMA COLON
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET
%token <intData> INTVALUE BOOLVALUE
%token <floatData> FLOATVALUE
%token <charData> CHARVALUE
%token <stringData> STRINGVALUE IDENTIFIER DATATYPE
%token UNKNOWN

// Types:
%type <voidData> STATEMENTS CASES SINGLE_CASE DEFAULT_CASE DECLARATION BLOCK DEFAULT_CASE_OPT
%type <expr> LOGICAL_EXPRESSION LOGICAL_TERM EQUALITY_EXPRESSION RELATIONAL_EXPRESSION ADDITIVE_EXPRESSION MULTIPLICATIVE_EXPRESSION EXPONENT_EXPRESSION UNARY_EXPRESSION PRIMARY_EXPRESSION PRIMARY_CASE FUNCTION_CALL
%type <codeUtils> IF_STATEMENT FOR_STATEMENT WHILE_STATEMENT REPEAT_STATEMENT SWITCH_STATEMENT IF_HEAD WHILE_HEAD REPEAT_HEAD FOR_HEADER
%type <parameterList> PARAMETER_LIST PARAM_LIST_NONEMPTY
%type <argList> ARGUMENT_LIST ARGUMENTS PRIMARY_SUFFIX
%type <declList> IDENTIFIERS

// Operator Precedence:
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left OR
%left AND
%left EQ NEQ
%left GT GE ST SE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right POWER
%right NOT
%right UMINUS

%%

PROGRAM:
    STATEMENTS
    ;

STATEMENTS:
    /* EMPTY */ { $$ = NULL; }
    | STATEMENTS STATEMENT
    ;

STATEMENT:
    DECLARATION SEMI_COLON
    | ASSIGNMENT SEMI_COLON
    | FUNCTION_CALL SEMI_COLON
    | RETURN_STATEMENT SEMI_COLON
    | BREAK SEMI_COLON { 
        char* b = get_break_label();
        if (!b) reportError(SEMANTIC_ERROR, "'break' Used Outside Loop/Switch", @1.first_line); // DONE
        else addQuadruple(OP_GOTO, NULL, NULL, b);
    }
    | CONTINUE SEMI_COLON {
        char* c = get_continue_label();
        if (!c) reportError(SEMANTIC_ERROR, "'continue' Used Outside Loop", @1.first_line); // DONE
        else addQuadruple(OP_GOTO, NULL, NULL, c);
    }
    | BLOCK
    | IF_STATEMENT
    | WHILE_STATEMENT
    | FOR_STATEMENT
    | REPEAT_STATEMENT
    | SWITCH_STATEMENT
    | FUNCTION_DEFINITION_IMPLEMENTATION

    // Error Handling:
    | DECLARATION error { 
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | FUNCTION_CALL error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | RETURN_STATEMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | BREAK error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | CONTINUE error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line); // DONE
        yyerrok;
    }
    | LEFT_CURLY_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '}' To Match '{'", @1.first_line); // DONE
        yyerrok;
    }
    ;

BLOCK:
    LEFT_CURLY_BRACKET {
        enterScope(&gScopeStack);
        if (pending_params_should_insert && pending_function_params) {
            Parameter* p = pending_function_params;
            while (p) {
                if (p->Name) {
                    singleEntryNode* a = lookupCurrentScope(&gScopeStack, p->Name);
                    if (a) reportError(SEMANTIC_ERROR, "Duplicate parameter name", @1.first_line);
                    else {
                        value pv; memset(&pv, 0, sizeof(pv));
                        singleEntryNode* pe = createNewEntry(p->Type, p->Name, SYMBOL_VARIABLE, pv, false, false, NULL);
                        insertInCurrentScope(&gScopeStack, pe);
                    }
                }
                p = p->Next;
            }
        }
        pending_function_params = NULL;
        pending_params_should_insert = false;
    }
    STATEMENTS RIGHT_CURLY_BRACKET { exitScope(&gScopeStack); $$ = NULL; }
    ;

DECLARATION:
    DATATYPE IDENTIFIERS {
        type declType = datatypeStringToType($1);
        bool isConst = false;
        DeclNode* cur = $2;
        while (cur) {
            singleEntryNode* already = lookupCurrentScope(&gScopeStack, cur->name);
            if (already) {
                reportError(SEMANTIC_ERROR, "Redeclaration In The Same Scope", cur->line); // DONE
                cur = cur->next;
                continue;
            }
            bool hasInit = (cur->initExpr != NULL);
            if (hasInit)
                if (!isTypeCompatible(declType, cur->initExpr->expressionType))
                    hasInit = false;
            value initVal;
            memset(&initVal, 0, sizeof(initVal));
            ////////////////       1111111111111111111111111111111111
            if (hasInit) {
                // char* rhs = exprToOperand(cur->initExpr);
                // addQuadruple(OP_ASSN, rhs, NULL, cur->name);
                // free(rhs);
                // updateVariableValueScoped(&gScopeStack, cur->name, declType, initVal);

                /* Should be correct, to be tested */
                Expression* init = cur->initExpr;
                if (init && isNumeric(declType)) {
                    init = castTo(init, declType);
                }

                char* rhs = exprToOperand(init);
                addQuadruple(OP_ASSN, rhs, NULL, cur->name);
                free(rhs);
            }
            if (hasInit) initVal = cur->initExpr->expressionValue;
            singleEntryNode* e = createNewEntry(declType, cur->name, SYMBOL_VARIABLE, initVal, hasInit, isConst, NULL);
            insertInCurrentScope(&gScopeStack, e);
            cur = cur->next;
        }
        freeDeclList($2);
        $$ = NULL;
    }
    | CONST DATATYPE IDENTIFIERS {
        type declType = datatypeStringToType($2);
        bool isConst = true;
        DeclNode* cur = $3;
        while (cur) {
            singleEntryNode* already = lookupCurrentScope(&gScopeStack, cur->name);
            if (already) {
                reportError(SEMANTIC_ERROR, "Redeclaration In The Same Scope", cur->line); // DONE
                cur = cur->next;
                continue;
            }
            if (!cur->initExpr) {
                reportError(SEMANTIC_ERROR, "const Identifier Must Be Initialized", cur->line); // DONE
                cur = cur->next;
                continue;
            }
            if (!isTypeCompatible(declType, cur->initExpr->expressionType)) {
                cur = cur->next;
                continue;
            }
            value initVal = cur->initExpr->expressionValue;
            singleEntryNode* e = createNewEntry(declType, cur->name, SYMBOL_VARIABLE, initVal, true, isConst, NULL);
            insertInCurrentScope(&gScopeStack, e);
            char* rhs = exprToOperand(cur->initExpr);
            addQuadruple(OP_ASSN, rhs, NULL, cur->name);
            free(rhs);
            updateVariableValueScoped(&gScopeStack, cur->name, declType, initVal);
            cur = cur->next;
        }
        freeDeclList($3);
        $$ = NULL;
    }

    // Error Handling:
    | DATATYPE error { 
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After Datatype", @1.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    | CONST error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'const'", @1.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    | CONST DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After 'const datatype'", @2.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    | DATATYPE CONST {
        reportError(SYNTAX_ERROR, "Unexpected Datatype Before 'const'", @1.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    ;

IDENTIFIERS:
    IDENTIFIER { $$ = makeDeclNode($1, NULL, @1.first_line); }
    | IDENTIFIER EQUAL LOGICAL_EXPRESSION { $$ = makeDeclNode($1, $3, @1.first_line); }
    | IDENTIFIERS COMMA IDENTIFIER { $$ = appendDeclNode($1, makeDeclNode($3, NULL, @3.first_line)); }
    | IDENTIFIERS COMMA IDENTIFIER EQUAL LOGICAL_EXPRESSION { $$ = appendDeclNode($1, makeDeclNode($3, $5, @3.first_line)); }

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line); // DONE
        yyerrok;
        $$ = makeDeclNode($1, NULL, @1.first_line);
    }
    | IDENTIFIERS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Identifier After ','", @2.first_line); // DONE
        yyerrok;
        $$ = $1;
    }
    | IDENTIFIERS COMMA IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @4.first_line); // DONE
        yyerrok;
        $$ = $1;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier/Declarator", @1.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    ;

ASSIGNMENT:
    IDENTIFIER EQUAL LOGICAL_EXPRESSION {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", @1.first_line); // DONE
        else
        {
            if (e->isReadOnly) reportError(SEMANTIC_ERROR, "Cannot Assign To const", @1.first_line); // DONE
            else if (!isTypeCompatible(e->identifierType, $3->expressionType)) { }  // isTypeCompatible already reports
            else {
                char* rhs = exprToOperand($3);
                addQuadruple(OP_ASSN, rhs, NULL, $1);
                free(rhs);
                updateVariableValueScoped(&gScopeStack, $1, e->identifierType, $3->expressionValue);
            }
        }
    }
    | IDENTIFIER INCREMENT {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", @1.first_line); // DONE
        else addQuadruple(OP_INC, NULL, NULL, $1);
    }
    | IDENTIFIER DECREMENT {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", @1.first_line); // DONE
        else addQuadruple(OP_DEC, NULL, NULL, $1);
    }
    | INCREMENT IDENTIFIER {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $2);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", @2.first_line); // DONE
        else addQuadruple(OP_INC, NULL, NULL, $2);
    }
    | DECREMENT IDENTIFIER {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $2);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", @2.first_line); // DONE
        else addQuadruple(OP_DEC, NULL, NULL, $2);
    }

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line); // DONE
        yyerrok;
    }
    | INCREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '++'", @1.first_line); // DONE
        yyerrok;
    }
    | DECREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '--'", @1.first_line); // DONE
        yyerrok;
    }
    ;

FUNCTION_CALL:
    IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET {
        singleEntryNode* fn = functionLookup(&gScopeStack, $1);
        if (!fn || fn->varOrFunc != SYMBOL_FUNCTION) {
            reportError(SEMANTIC_ERROR, "Call To Undeclared Function", @1.first_line); // DONE
            $$ = makeTempExpr(INT_TYPE, $1);
        }
        else {
            ArgNode* a = $3;
            while (a) {
                char* op = exprToOperand(a->expr);
                addQuadruple(OP_PARM, op, NULL, NULL);
                free(op);
                a = a->next;
            }
            if (fn->identifierType != VOID_TYPE) {
                char* t = createTemp();
                addQuadruple(OP_CALL, $1, NULL, t);
                $$ = makeTempExpr(fn->identifierType, t);
                free(t);
            }
            else {
                addQuadruple(OP_CALL, $1, NULL, NULL);
                value v;
                memset(&v, 0, sizeof(v));
                $$ = makeExpr(VOID_TYPE, v, NULL);
            }
        }
        freeArgList($3);
    }

    // Error Handling:
    | IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST error {
        reportError(SYNTAX_ERROR, "Expected ')' In Function Call", @3.first_line); // DONE
        yyerrok;
    }
    | IDENTIFIER error ARGUMENT_LIST RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected '(' In Function Call", @2.first_line); // DONE
        yyerrok;
    }
    ;

IF_STATEMENT:
    IF_HEAD BLOCK %prec LOWER_THAN_ELSE {
        addQuadruple(OP_LABEL, NULL, NULL, $1.false_label);
        free($1.false_label);
        free($1.end_label);
        $$.code = NULL;
    }
    | IF_HEAD BLOCK {
        addQuadruple(OP_GOTO, NULL, NULL, $1.end_label);
        addQuadruple(OP_LABEL, NULL, NULL, $1.false_label);
    }
    ELSE BLOCK {
        addQuadruple(OP_LABEL, NULL, NULL, $1.end_label);
        free($1.false_label);
        free($1.end_label);
        $$.code = NULL;
    }

    // Error Handling:
    | IF_HEAD error {
        reportError(SYNTAX_ERROR, "Expected If Block '{ }' After 'if (...)'", @1.first_line); // DONE
        yyerrok;
    }
    ;

IF_HEAD:
    IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET {
        $$.false_label = createLabel();
        $$.end_label = createLabel();
        char* cond = exprToOperand($3);
        addQuadruple(OP_IFFALSE, cond, NULL, $$.false_label);
        free(cond);
    }

    // Error Handling:
    | IF error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'if'", @1.first_line); // DONE
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", @2.first_line); // DONE
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @3.first_line); // DONE
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'if'", @3.first_line); // DONE
        yyerrok;
    }
    ;

WHILE_STATEMENT:
    WHILE_HEAD BLOCK {
        pop_loop_labels();
        addQuadruple(OP_GOTO, NULL, NULL, $1.start_label);
        addQuadruple(OP_LABEL, NULL, NULL, $1.end_label);
        free($1.start_label);
        free($1.end_label);
        $$.code = NULL;
    }

    // Error Handling:
    | WHILE_HEAD error {
        reportError(SYNTAX_ERROR, "Expected While Block '{ }' After 'while'", @1.first_line); // DONE
        yyerrok;
    }
    ;

WHILE_HEAD:
    WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET {
        $$.start_label = createLabel();
        $$.end_label   = createLabel();
        addQuadruple(OP_LABEL, NULL, NULL, $$.start_label);
        char* cond = exprToOperand($3);
        addQuadruple(OP_IFFALSE, cond, NULL, $$.end_label);
        free(cond);
        push_loop_labels($$.end_label, $$.start_label);
    }

    // Error Handling:
    | WHILE error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'while'", @1.first_line); // DONE
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", @2.first_line); // DONE
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @3.first_line); // DONE
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'while'", @3.first_line); // DONE
        yyerrok;
    }
    ;

REPEAT_STATEMENT:
    REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET SEMI_COLON {
        pop_loop_labels();
        char* cond = exprToOperand($5);
        addQuadruple(OP_IFFALSE, cond, NULL, $1.start_label);
        free(cond);
        addQuadruple(OP_LABEL, NULL, NULL, $1.end_label);
        free($1.start_label);
        free($1.end_label);
        $$.code = NULL;
    }

    // Error Handling:
    | REPEAT_HEAD error {
        reportError(SYNTAX_ERROR, "Expected Repeat Block '{ }' After 'repeat'", @1.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK error {
        reportError(SYNTAX_ERROR, "Expected 'until (...) ;' After Repeat Block", @2.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK UNTIL error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'until'", @3.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", @4.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error  {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @5.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'until'", @4.first_line); // DONE
        yyerrok;
    }
    | REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error  {
        reportError(SYNTAX_ERROR, "Expected ';' After 'until (...)'", @5.first_line); // DONE
        yyerrok;
    }
    ;

REPEAT_HEAD:
    REPEAT {
        $$.start_label = createLabel();
        $$.end_label   = createLabel();
        addQuadruple(OP_LABEL, NULL, NULL, $$.start_label);
        push_loop_labels($$.end_label, $$.start_label);
    }
    ;

FOR_STATEMENT:
    FOR_HEADER BLOCK {
        pop_loop_labels();
        addQuadruple(OP_GOTO, NULL, NULL, $1.incr_label);
        addQuadruple(OP_LABEL, NULL, NULL, $1.end_label);
        free($1.cond_label);
        free($1.incr_label);
        free($1.end_label);
        $$.code = NULL;
    }

    // Error Handling:
    | FOR_HEADER error {
        reportError(SYNTAX_ERROR, "Expected For Block '{ }' After 'for'", @1.first_line); // DONE
        yyerrok;
    }
    ;

FOR_HEADER:
    FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET {
        $$.cond_label = createLabel();
        $$.incr_label = createLabel();
        $$.end_label  = createLabel();
        addQuadruple(OP_GOTO, NULL, NULL, $$.cond_label);
        addQuadruple(OP_LABEL, NULL, NULL, $$.incr_label);
        addQuadruple(OP_LABEL, NULL, NULL, $$.cond_label);
        char* cond = exprToOperand($5);
        addQuadruple(OP_IFFALSE, cond, NULL, $$.end_label);
        free(cond);
        push_loop_labels($$.end_label, $$.incr_label);
    }

    // Error Handling:
    | FOR error {
        reportError(SYNTAX_ERROR, "Expected '(' After for", @1.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected Iterator After '('", @2.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR error {
        reportError(SYNTAX_ERROR, "Expected ';' After Iterator", @3.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON error {
        reportError(SYNTAX_ERROR, "Expected Condition After ';'", @4.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ';' After Condition", @5.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON error {
        reportError(SYNTAX_ERROR, "Expected Loop Step After ';'", @6.first_line); // DONE
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ')' After Loop Step", @7.first_line); // DONE
        yyerrok;
    }
    ;

ITERATOR:
    IDENTIFIER EQUAL LOGICAL_EXPRESSION {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) reportError(SEMANTIC_ERROR, "Identifier is undeclared (for-init)", @1.first_line);
        else if (e->isReadOnly) reportError(SEMANTIC_ERROR, "Cannot assign to const (for-init)", @1.first_line);
        else if (!isTypeCompatible(e->identifierType, $3->expressionType)) { } // isTypeCompatible reports
        else {
            char* rhs = exprToOperand($3);
            addQuadruple(OP_ASSN, rhs, NULL, $1);
            free(rhs);
            updateVariableValueScoped(&gScopeStack, $1, e->identifierType, $3->expressionValue);
        }
    }
    | DATATYPE IDENTIFIER EQUAL LOGICAL_EXPRESSION {
        type declType = datatypeStringToType($1);
        singleEntryNode* already = lookupCurrentScope(&gScopeStack, $2);
        if (already) reportError(SEMANTIC_ERROR, "Redeclaration in same scope (for-init)", @2.first_line);
        else {
            if (!isTypeCompatible(declType, $4->expressionType)) { } // isTypeCompatible reports
            else {
                value initVal = $4->expressionValue;
                singleEntryNode* e = createNewEntry(declType, $2, SYMBOL_VARIABLE, initVal, true, false, NULL);
                insertInCurrentScope(&gScopeStack, e);
                char* rhs = exprToOperand($4);
                addQuadruple(OP_ASSN, rhs, NULL, $2);
                free(rhs);
                updateVariableValueScoped(&gScopeStack, $2, declType, initVal);
            }
        }
    }

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line); // DONE
        yyerrok;
    }
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype", @1.first_line); // DONE
        yyerrok;
    }
    | DATATYPE IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '=' After Identifier", @2.first_line); // DONE
        yyerrok;
    }
    | DATATYPE IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line); // DONE
        yyerrok;
    }
    ;

SWITCH_STATEMENT:
    SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET {
        char* t  = createTemp();
        char* sw = exprToOperand($3);
        addQuadruple(OP_ASSN, sw, NULL, t);
        free(sw);
        current_switch_var = strdup(t);
        free(t);
        current_switch_end_label = createLabel();
        default_label = createLabel();
        push_loop_labels(current_switch_end_label, NULL);
    }
    LEFT_CURLY_BRACKET { enterScope(&gScopeStack); } CASES DEFAULT_CASE_OPT RIGHT_CURLY_BRACKET { exitScope(&gScopeStack); } {
        addQuadruple(OP_LABEL, NULL, NULL, default_label);
        addQuadruple(OP_LABEL, NULL, NULL, current_switch_end_label);
        pop_loop_labels();
        free(current_switch_var);
        current_switch_var = NULL;
        free(current_switch_end_label);
        current_switch_end_label = NULL;
        free(default_label);
        default_label = NULL;
        $$.code = NULL;
    }

    // Error Handling:
    | SWITCH error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'switch'", @1.first_line);
        yyerrok;
    }
    ;

DEFAULT_CASE_OPT:
    /* EMPTY */ { $$ = NULL; }
    | DEFAULT_CASE
    ;

CASES:
    /* EMPTY */ { $$ = NULL; }
    | CASES SINGLE_CASE
    ;

SINGLE_CASE:
    CASE PRIMARY_CASE {
        current_case_next_label = createLabel();
        if (!current_switch_var) reportError(SEMANTIC_ERROR, "case used outside switch", @1.first_line);
        else {
            char* caseOp = exprToOperand($2);
            char* tcmp   = createTemp();
            addQuadruple(OP_EQ, current_switch_var, caseOp, tcmp);
            addQuadruple(OP_IFFALSE, tcmp, NULL, current_case_next_label);
            free(caseOp);
            free(tcmp);
        }
    }
    COLON STATEMENTS {
        if (current_switch_end_label) addQuadruple(OP_GOTO, NULL, NULL, current_switch_end_label);
        addQuadruple(OP_LABEL, NULL, NULL, current_case_next_label);
        free(current_case_next_label);
        current_case_next_label = NULL;
        $$ = NULL;
    }

    // Error Handling:
    | CASE error {
        reportError(SYNTAX_ERROR, "Expected Case Value After 'case'", @1.first_line);
        yyerrok;
    }
    | CASE PRIMARY_CASE error {
        reportError(SYNTAX_ERROR, "Expected ':' After Case Value", @2.first_line);
        yyerrok;
    }
    ;

DEFAULT_CASE:
    DEFAULT COLON { if (default_label) addQuadruple(OP_LABEL, NULL, NULL, default_label); }
    STATEMENTS {
        if (current_switch_end_label) addQuadruple(OP_GOTO, NULL, NULL, current_switch_end_label);
        $$ = NULL;
    }

    // Error Handling:
    | DEFAULT error {
        reportError(SYNTAX_ERROR, "Expected ':' After 'default'", @1.first_line);
        yyerrok;
    }
    ;

PRIMARY_CASE:
    INTVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.INT_Data = $1;
        $$ = makeExpr(INT_TYPE, v, NULL);
    }
    | CHARVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.CHAR_Data = $1;
        $$ = makeExpr(CHAR_TYPE, v, NULL);
    }
    | BOOLVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.BOOL_Data = $1;
        $$ = makeExpr(BOOL_TYPE, v, NULL);
    }
    | IDENTIFIER {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) {
            reportError(SEMANTIC_ERROR, "Use of undeclared identifier in case", @1.first_line);
            value v;
            memset(&v, 0, sizeof(v));
            $$ = makeExpr(INT_TYPE, v, $1);
        }
        else $$ = makeExpr(e->identifierType, e->currentValue, $1);
    }
    | FLOATVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.FLOAT_Data = $1;
        $$ = makeExpr(FLOAT_TYPE, v, NULL);
    }
    | STRINGVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.STRING_Data = strdup($1);
        $$ = makeExpr(STRING_TYPE, v, NULL);
    }
    ;

RETURN_STATEMENT:
    RETURN {
        if (inFunctionScope == false) reportError(SEMANTIC_ERROR, "Return used outside function", @1.first_line);
        if (current_function_return_type != VOID_TYPE) reportError(SEMANTIC_ERROR, "Missing return value", @1.first_line);
        else seen1ReturnStatement = true;
        addQuadruple(OP_RETURN, NULL, NULL, NULL);
    }
    | RETURN LOGICAL_EXPRESSION {
        if (inFunctionScope == false) reportError(SEMANTIC_ERROR, "Return used outside function", @1.first_line);
        if (current_function_return_type == VOID_TYPE && gScopeStack.topScope->scopeDepth != 0) reportError(SEMANTIC_ERROR, "Returning a value from void function", @1.first_line);
        else if ($2 && !isTypeCompatible(current_function_return_type, $2->expressionType)) { } // isTypeCompatible already reports
        else {
            seen1ReturnStatement = true;
            char* rv = exprToOperand($2);
            addQuadruple(OP_RETURN, rv, NULL, NULL);
            free(rv);
        }
    }
    ;

FUNCTION_DEFINITION_IMPLEMENTATION:
    FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET {
        seen1ReturnStatement = false;
        inFunctionScope = true;
        type retType = datatypeStringToType($2);
        singleEntryNode* already = lookupCurrentScope(&gScopeStack, $3);
        if (already) reportError(SEMANTIC_ERROR, "Redeclaration of function in same scope", @3.first_line);
        else {
            value v;
            memset(&v, 0, sizeof(v));
            singleEntryNode* fn = createNewEntry(retType, $3, SYMBOL_FUNCTION, v, true, false, $5);
            insertInCurrentScope(&gScopeStack, fn);
        }
        current_function_return_type = retType;
        // enterScope(&gScopeStack); /* Send the params to the function scope, comment this*/
        /* Params should be inserted to the function scope inside the block */
        // Parameter* p = $5;
        // while (p) {
        //     if (p->Name) {
        //         singleEntryNode* a = lookupCurrentScope(&gScopeStack, p->Name);
        //         if (a) reportError(SEMANTIC_ERROR, "Duplicate parameter name", @3.first_line);
        //         else {
        //             value pv; 
        //             memset(&pv, 0, sizeof(pv));
        //             singleEntryNode* pe = createNewEntry(p->Type, p->Name, SYMBOL_VARIABLE, pv, false, false, NULL);
        //             insertInCurrentScope(&gScopeStack, pe);
        //         }
        //     }
        //     p = p->Next;
        // }
        /* Params handling */
        pending_function_params = $5;
        pending_params_should_insert = true;

    }
    BLOCK {
        // exitScope(&gScopeStack); /* commented to match the commented enter scope above */
        if (inFunctionScope) {
            if (current_function_return_type != VOID_TYPE && !seen1ReturnStatement) reportError(SEMANTIC_ERROR, "Missing return statement in non-void function", @2.first_line);
            inFunctionScope = false;
        }
        current_function_return_type = VOID_TYPE;
    }

    // Error Handling:
    | FUNCTION error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'function'", @1.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Function Datatype", @2.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '(' After Function Name", @3.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST error {
        reportError(SYNTAX_ERROR, "Expected ')' After Parameters", @5.first_line);
        yyerrok;
    }
    ;

PARAMETER_LIST:
    /* EMPTY */ { $$ = NULL; }
    | PARAM_LIST_NONEMPTY { $$ = $1; }
    ;

PARAM_LIST_NONEMPTY:
    DATATYPE IDENTIFIER {
        $$ = makeParam(datatypeStringToType($1), $2);
    }
    | PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER {
        $$ = appendParam($1, makeParam(datatypeStringToType($3), $4));
    }

    // Error Handling:
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @1.first_line);
        yyerrok;
        $$ = NULL;
    }
    | PARAM_LIST_NONEMPTY COMMA error {
        reportError(SYNTAX_ERROR, "Expected Datatype After ',' In Parameter List", @2.first_line);
        yyerrok;
        $$ = $1;
    }
    | PARAM_LIST_NONEMPTY COMMA DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @3.first_line);
        yyerrok;
        $$ = $1;
    }
    ;

ARGUMENT_LIST:
    /* EMPTY */ { $$ = NULL; }
    | ARGUMENTS { $$ = $1; }
    ;

ARGUMENTS:
    LOGICAL_EXPRESSION { $$ = makeArgNode($1); }
    | ARGUMENTS COMMA LOGICAL_EXPRESSION { $$ = appendArgNode($1, makeArgNode($3)); }

    // Error Handling:
    | ARGUMENTS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Expression After ',' In Arguments", @2.first_line); // DONE
        yyerrok;
        $$ = $1;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unexpected ',' Before First Argument", @1.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    ;

LOGICAL_EXPRESSION:
    LOGICAL_EXPRESSION OR LOGICAL_TERM {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_OR, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | LOGICAL_TERM { $$ = $1; }

    // Error Handling:
    | LOGICAL_EXPRESSION OR error {
        reportError(SYNTAX_ERROR, "Expected Expression After '||'", @2.first_line); // DONE
        yyerrok;
    }
    | OR error {
        reportError(SYNTAX_ERROR, "Unexpected '||' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

LOGICAL_TERM:
    LOGICAL_TERM AND EQUALITY_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_AND, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | EQUALITY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | LOGICAL_TERM AND error {
        reportError(SYNTAX_ERROR, "Expected Expression After '&&'", @2.first_line); // DONE
        yyerrok;
    }
    | AND error {
        reportError(SYNTAX_ERROR, "Unexpected '&&' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

EQUALITY_EXPRESSION:
    EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_EQ, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | EQUALITY_EXPRESSION NEQ RELATIONAL_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_NEQ, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | RELATIONAL_EXPRESSION { $$ = $1; }

    // Error Handling:
    | EQUALITY_EXPRESSION EQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '=='", @2.first_line); // DONE
        yyerrok;
    }
    | EQUALITY_EXPRESSION NEQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!='", @2.first_line); // DONE
        yyerrok;
    }
    | EQ error {
        reportError(SYNTAX_ERROR, "Unexpected '==' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | NEQ error {
        reportError(SYNTAX_ERROR, "Unexpected '!=' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

RELATIONAL_EXPRESSION:
    RELATIONAL_EXPRESSION ST ADDITIVE_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_LT, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | RELATIONAL_EXPRESSION GT ADDITIVE_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_GT, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | RELATIONAL_EXPRESSION SE ADDITIVE_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_LTE, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | RELATIONAL_EXPRESSION GE ADDITIVE_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_GTE, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | ADDITIVE_EXPRESSION { $$ = $1; }

    // Error Handling:
    | RELATIONAL_EXPRESSION ST error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<'", @2.first_line); // DONE
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>'", @2.first_line); // DONE
        yyerrok;
    }
    | RELATIONAL_EXPRESSION SE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<='", @2.first_line); // DONE
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>='", @2.first_line); // DONE
        yyerrok;
    }
    | ST error {
        reportError(SYNTAX_ERROR, "Unexpected '<' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | GT error {
        reportError(SYNTAX_ERROR, "Unexpected '>' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | SE error {
        reportError(SYNTAX_ERROR, "Unexpected '<=' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | GE error {
        reportError(SYNTAX_ERROR, "Unexpected '>=' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

ADDITIVE_EXPRESSION:
    ADDITIVE_EXPRESSION PLUS MULTIPLICATIVE_EXPRESSION {
        type rt = arithmeticResultType($1->expressionType, $3->expressionType);
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_ADD, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(rt, t);
        free(t);
    }
    | ADDITIVE_EXPRESSION MINUS MULTIPLICATIVE_EXPRESSION {
        type rt = arithmeticResultType($1->expressionType, $3->expressionType);
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_SUB, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(rt, t);
        free(t);
    }
    | MULTIPLICATIVE_EXPRESSION { $$ = $1; }

    // Error Handling:
    | ADDITIVE_EXPRESSION PLUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '+'", @2.first_line); // DONE
        yyerrok;
    }
    | ADDITIVE_EXPRESSION MINUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '-'", @2.first_line); // DONE
        yyerrok;
    }
    | PLUS error {
        reportError(SYNTAX_ERROR, "Unexpected '+' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

MULTIPLICATIVE_EXPRESSION:
    MULTIPLICATIVE_EXPRESSION MULTIPLY EXPONENT_EXPRESSION {
        type rt = arithmeticResultType($1->expressionType, $3->expressionType);
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_MUL, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(rt, t);
        free(t);
    }
    | MULTIPLICATIVE_EXPRESSION DIVIDE EXPONENT_EXPRESSION {
        type rt = FLOAT_TYPE;
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_DIV, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(rt, t);
        free(t);
    }
    | MULTIPLICATIVE_EXPRESSION MODULO EXPONENT_EXPRESSION {
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_MOD, a1, a2, t);
        free(a1);
        free(a2);
        $$ = makeTempExpr(INT_TYPE, t);
        free(t);
    }
    | EXPONENT_EXPRESSION { $$ = $1; }

    // Error Handling:
    | MULTIPLICATIVE_EXPRESSION MULTIPLY error {
        reportError(SYNTAX_ERROR, "Expected Expression After '*'", @2.first_line); // DONE
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION DIVIDE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '/'", @2.first_line); // DONE
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION MODULO error {
        reportError(SYNTAX_ERROR, "Expected Expression After '%'", @2.first_line); // DONE
        yyerrok;
    }
    | MULTIPLY error {
        reportError(SYNTAX_ERROR, "Unexpected '*' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | DIVIDE error {
        reportError(SYNTAX_ERROR, "Unexpected '/' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    | MODULO error {
        reportError(SYNTAX_ERROR, "Unexpected '%' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

EXPONENT_EXPRESSION:
    EXPONENT_EXPRESSION POWER UNARY_EXPRESSION {
        type resultType = (($1 && $1->expressionType == FLOAT_TYPE) || ($3 && $3->expressionType == FLOAT_TYPE)) ? FLOAT_TYPE : INT_TYPE;
        char* a1 = exprToOperand($1);
        char* a2 = exprToOperand($3);
        char* t  = createTemp();
        addQuadruple(OP_EXP, a1, a2, t);
        value v;
        memset(&v, 0, sizeof(v));
        $$ = makeExpr(resultType, v, t);
        free(a1);
        free(a2);
        free(t);
    }
    | UNARY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | EXPONENT_EXPRESSION POWER error {
        reportError(SYNTAX_ERROR, "Expected Expression After '**'", @2.first_line); // DONE
        yyerrok;
    }
    | POWER error {
        reportError(SYNTAX_ERROR, "Unexpected '**' At Start Of Expression", @1.first_line); // DONE
        yyerrok;
    }
    ;

UNARY_EXPRESSION:
    MINUS UNARY_EXPRESSION %prec UMINUS {
        char* a1 = exprToOperand($2);
        char* t  = createTemp();
        addQuadruple(OP_UMIN, a1, NULL, t);
        free(a1);
        $$ = makeTempExpr($2->expressionType, t);
        free(t);
    }
    | NOT UNARY_EXPRESSION {
        char* a1 = exprToOperand($2);
        char* t  = createTemp();
        addQuadruple(OP_NOT, a1, NULL, t);
        free(a1);
        $$ = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
    | PRIMARY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | NOT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!'", @1.first_line); // DONE
        yyerrok;
    }
    | MINUS error %prec UMINUS {
        reportError(SYNTAX_ERROR, "Expected Expression After Unary '-'", @1.first_line); // DONE
        yyerrok;
    }
    ;

PRIMARY_EXPRESSION:
    INTVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.INT_Data = $1;
        $$ = makeExpr(INT_TYPE, v, NULL);
    }
    | FLOATVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.FLOAT_Data = $1;
        $$ = makeExpr(FLOAT_TYPE, v, NULL);
    }
    | CHARVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.CHAR_Data = $1;
        $$ = makeExpr(CHAR_TYPE, v, NULL);
    }
    | BOOLVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.BOOL_Data = $1;
        $$ = makeExpr(BOOL_TYPE, v, NULL);
    }
    | STRINGVALUE {
        value v;
        memset(&v, 0, sizeof(v));
        v.STRING_Data = strdup($1);
        $$ = makeExpr(STRING_TYPE, v, NULL);
    }
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET {
        $$ = $2;
    }
    | IDENTIFIER PRIMARY_SUFFIX {
        if ($2 != NULL) {
            singleEntryNode* fn = functionLookup(&gScopeStack, $1);
            if (!fn || fn->varOrFunc != SYMBOL_FUNCTION) {
                reportError(SEMANTIC_ERROR, "Call to undeclared function", @1.first_line);
                $$ = makeTempExpr(INT_TYPE, $1);
            }
            else {
                ArgNode* a = $2;
                while (a) {
                    char* op = exprToOperand(a->expr);
                    addQuadruple(OP_PARM, op, NULL, NULL);
                    free(op);
                    a = a->next;
                }
                if (fn->identifierType != VOID_TYPE) {
                    char* t = createTemp();
                    addQuadruple(OP_CALL, $1, NULL, t);
                    $$ = makeTempExpr(fn->identifierType, t);
                    free(t);
                }
                else {
                    addQuadruple(OP_CALL, $1, NULL, NULL);
                    value v;
                    memset(&v, 0, sizeof(v));
                    $$ = makeExpr(VOID_TYPE, v, NULL);
                }
            }
            freeArgList($2);
        }
        else {
            singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
            if (!e) {
                reportError(SEMANTIC_ERROR, "Use of undeclared identifier", @1.first_line);
                value v;
                memset(&v, 0, sizeof(v));
                $$ = makeExpr(INT_TYPE, v, $1);
            }
            else $$ = makeExpr(e->identifierType, e->currentValue, $1);
        }
    }

    // Error Handling:
    | LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )'", @2.first_line); // DONE
        yyerrok;
        $$ = NULL;
    }
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')'", @2.first_line); // DONE
        yyerrok;
        $$ = $2;
    }
    ;

PRIMARY_SUFFIX:
    /*EMPTY*/ { $$ = NULL; }
    | LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET { $$ = $2; }
    ;

%%

void yyerror(const char* String) { (void)String; }

int main(int argc , char** argv) {
    if(argc > 1) {
        yyin = fopen(argv[1] , "r");
        if(!yyin) {
            fprintf(stderr, "Could not open file: %s\n", argv[1]);
            exit(1);
        }
    }
    else yyin = stdin;

    scopeStackInit(&gScopeStack);
    initScopeLogFile("exe/scopes_logs.txt"); /* For the Global Symbol Table + Children Symbol Tables */

    enterScope(&gScopeStack); // global scope
    int parseResult = yyparse();
    printQuadruplesToFile("exe/quadruples.txt");
    dumpScopeStackToFile(&gScopeStack, "exe/symbol_table.txt");
    quadruplesToAssembly("exe/output.asm");
    exitScope(&gScopeStack);

    int errorCount = getErrorCount();
    if (errorCount){
        printf("\n%d Errors:\n", errorCount);
        printErrors();
    }
    int warningCount = getWarningCount();
    if (warningCount) {
        printf("\n%d Warnings:\n", warningCount);
        printWarnings();
    }

    return (parseResult == 0 && getErrorCount() == 0) ? 0 : 1;
}