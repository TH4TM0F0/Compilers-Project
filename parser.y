// Definitions:
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <math.h>
    #include "Quadruple.h"
    #include "ErrorHandler.h"
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
    singleEntryNode* function_list_head = NULL;
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

    // ------------------------------
    // Declaration list node
    // ------------------------------
    typedef struct DeclNode {
        char* name;
        Expression* initExpr;
        int line;
        struct DeclNode* next;
    } DeclNode;

    static DeclNode* makeDeclNode(const char* name, Expression* initExpr, int line) {
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

    static char* exprToOperand(Expression* e) {
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

    static Expression* makeExpr(type t, value v, const char* temp) {
        Expression* e = (Expression*)malloc(sizeof(Expression));
        if (!e) return NULL;
        e->expressionType = t;
        e->expressionValue = v;
        e->temp_var = temp ? strdup(temp) : NULL;
        return e;
    }

    // ------------------------------
    // Global scope stack (based on your SymbolTable.c)
    // ------------------------------
    scopeStack gScopeStack;  // init in main() before yyparse()

%}

%locations

%code requires
{
    #include "Utils.h"
    #include "SymbolTable.h"
    #include "Parameter.h"

    typedef struct Expression Expression;
    typedef struct DeclNode DeclNode;
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
    struct 
    {
        char* code;     
        char* true_label;
        char* false_label;
        char* next_label;
        char* start_label;
        char* end_label;
        char* cond_label;
        char* body_label;
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

//Datatypes For Grammar Rules:
%type <voidData> STATEMENTS CASES SINGLE_CASE DEFAULT_CASE DECLARATION
%type <expr> LOGICAL_EXPRESSION LOGICAL_TERM EQUALITY_EXPRESSION RELATIONAL_EXPRESSION ADDITIVE_EXPRESSION MULTIPLICATIVE_EXPRESSION EXPONENT_EXPRESSION UNARY_EXPRESSION PRIMARY_EXPRESSION PRIMARY_CASE FUNCTION_CALL 
%type <codeUtils> IF_STATEMENT FOR_STATEMENT WHILE_STATEMENT REPEAT_STATEMENT SWITCH_STATEMENT 
%type <parameterList> PARAMETER_LIST PARAM_LIST_NONEMPTY ARGUMENT_LIST ARGUMENTS PRIMARY_SUFFIX /* Check PRIMARY_SUFFIX later */
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

// Grammar Rules:
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
        addQuadruple(OP_GOTO, NULL, NULL, get_break_label());
    }
    | CONTINUE SEMI_COLON {
        addQuadruple(OP_GOTO, NULL, NULL, get_continue_label());
    }
    | LEFT_CURLY_BRACKET { enterScope(&gScopeStack); } STATEMENTS RIGHT_CURLY_BRACKET { exitScope(&gScopeStack); }
    | IF_STATEMENT
    | WHILE_STATEMENT
    | FOR_STATEMENT
    | REPEAT_STATEMENT
    | SWITCH_STATEMENT
    | FUNCTION_DEFINITION_IMPLEMENTATION

    // Error Handling:
    | DECLARATION error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | FUNCTION_CALL error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | RETURN_STATEMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | BREAK error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | CONTINUE error {
        reportError(SYNTAX_ERROR, "Expected ';'", @2.first_line);
        yyerrok;
    }
    | LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}'", @3.first_line);
        yyerrok;
    }
    | EQUAL LOGICAL_EXPRESSION SEMI_COLON {
        reportError(SYNTAX_ERROR, "Expected Identifier Before '='", @1.first_line);
        yyerrok;
    }
    ;

DECLARATION:
    DATATYPE IDENTIFIERS {
        type declType = datatypeStringToType($1);
        bool isConst = false;
        DeclNode* cur = $2;
        while (cur) {
            // 1) redeclaration in SAME scope
            singleEntryNode* already = lookupCurrentScope(&gScopeStack, cur->name);
            if (already) {
                reportError(SEMANTIC_ERROR, "Redeclaration in same scope", cur->line);
                cur = cur->next;
                continue;
            }
            // 2) initializer checks
            bool hasInit = (cur->initExpr != NULL);
            if (hasInit) {
                if (!isTypeCompatible(declType, cur->initExpr->expressionType)) {
                    // isTypeCompatible reports the error (based on your SymbolTable.c implementation)
                    hasInit = false;
                }
            }
            // 3) create entry
            value initVal;
            memset(&initVal, 0, sizeof(initVal));
            if (hasInit) initVal = cur->initExpr->expressionValue;
            singleEntryNode* e = createNewEntry(declType, cur->name, SYMBOL_VARIABLE, initVal, hasInit, isConst, NULL);
            insertInCurrentScope(&gScopeStack, e);

            // 4) emit quadruple + update runtime value if init exists
            if (hasInit) {
                char* rhs = exprToOperand(cur->initExpr);
                addQuadruple(OP_ASSN, rhs, NULL, cur->name);
                free(rhs);
                // update scoped value using YOUR API
                updateVariableValueScoped(&gScopeStack, cur->name, declType, initVal);
            }
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
                reportError(SEMANTIC_ERROR, "Redeclaration in same scope", cur->line);
                cur = cur->next;
                continue;
            }
            // const MUST be initialized (your rule)
            if (!cur->initExpr) {
                reportError(SEMANTIC_ERROR, "const must be initialized", cur->line);
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

    // Error Handling (keep yours)
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After Datatype", @2.first_line);
        yyerrok;
        $$ = NULL;
    }
    | CONST error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'const'", @2.first_line);
        yyerrok;
        $$ = NULL;
    }
    | CONST DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After 'const datatype'", @3.first_line);
        yyerrok;
        $$ = NULL;
    }
    | DATATYPE CONST {
        reportError(SYNTAX_ERROR, "Unexpected Datatype Before 'const'", @1.first_line);
        yyerrok;
        $$ = NULL;
    }
    ;

IDENTIFIERS:
    IDENTIFIER { 
        $$ = makeDeclNode($1, NULL, @1.first_line); 
    }
    | IDENTIFIER EQUAL LOGICAL_EXPRESSION { 
        $$ = makeDeclNode($1, $3, @1.first_line); 
    }
    | IDENTIFIERS COMMA IDENTIFIER { 
        $$ = appendDeclNode($1, makeDeclNode($3, NULL, @3.first_line)); 
    }
    | IDENTIFIERS COMMA IDENTIFIER EQUAL LOGICAL_EXPRESSION { 
        $$ = appendDeclNode($1, makeDeclNode($3, $5, @3.first_line)); 
    }

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @3.first_line);
        yyerrok;
        $$ = makeDeclNode($1, NULL, @1.first_line);
    }
    | IDENTIFIERS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Identifier After ','", @3.first_line);
        yyerrok;
        $$ = $1;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier/Declarator", @2.first_line);
        yyerrok;
        $$ = NULL;
    }
    ;

ASSIGNMENT: 
    IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | IDENTIFIER INCREMENT
    | IDENTIFIER DECREMENT
    | INCREMENT IDENTIFIER
    | DECREMENT IDENTIFIER

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @3.first_line);
        yyerrok;
    }
    | INCREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '++'", @2.first_line);
        yyerrok;
    }
    | DECREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '--'", @2.first_line);
        yyerrok;
    }
    ;

FUNCTION_CALL:                                        
    IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET
    | IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST error {
    reportError(SYNTAX_ERROR, "Expected ')' In Function Call", @4.first_line);
    yyerrok;
    }
    | IDENTIFIER error ARGUMENT_LIST RIGHT_ROUND_BRACKET {
    reportError(SYNTAX_ERROR, "Expected '(' In Function Call", @2.first_line);
    yyerrok;
    }
    ;                                                 

IF_STATEMENT: 
    IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET %prec LOWER_THAN_ELSE
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET ELSE LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

    // Error Handling:
    | IF LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'if'", @3.first_line);
        yyerrok;
    }
    | IF error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'if'", @2.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @4.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'if (condition)'", @5.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close 'if' Block", @7.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET ELSE error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'else'", @8.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET ELSE LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close 'else' Block", @11.first_line);
        yyerrok;
    }
    ;

WHILE_STATEMENT: 
    WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

    // Error Handling:
    | WHILE LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'while'", @3.first_line);
        yyerrok;
    }
    | WHILE error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'while'", @2.first_line);
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @4.first_line);
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'while (condition)'", @5.first_line);
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close 'while' Block", @7.first_line);
        yyerrok;
    }
    ;

REPEAT_STATEMENT: 
    REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET SEMI_COLON

    // Error Handling:
    | REPEAT error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'repeat'", @2.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close 'repeat' Block", @4.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected 'until' After 'repeat' Block", @5.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'until'", @6.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET SEMI_COLON {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'until'", @7.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @8.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected ';' After 'until (condition)'", @9.first_line);
        yyerrok;
    }
    ;

FOR_STATEMENT: 
    FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

    // Error Handling:
    | FOR error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'for'", @2.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Invalid 'for' Header", @3.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR error {
        reportError(SYNTAX_ERROR, "Expected ';' After For-Initializer", @4.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ';' After For-Condition", @6.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ')' After For-Step", @8.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'for (...)'", @9.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close 'for' Block", @11.first_line);
        yyerrok;
    }
    ;

ITERATOR: 
    IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | DATATYPE IDENTIFIER EQUAL LOGICAL_EXPRESSION

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '=' In For-Initializer", @3.first_line);
        yyerrok;
    }
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype In For-Initializer", @2.first_line);
        yyerrok;
    }
    | DATATYPE IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '=' After Identifier In For-Initializer", @3.first_line);
        yyerrok;
    }
    ;

SWITCH_STATEMENT:                                                               
    SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES RIGHT_CURLY_BRACKET
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE RIGHT_CURLY_BRACKET

    // Error Handling:
    | SWITCH error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'switch'", @2.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )' After 'switch'", @3.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )' After 'switch'", @3.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Switch Expression", @4.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' To Start Switch Body", @5.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close Switch Body", @7.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close Switch Body", @8.first_line);
        yyerrok;
    }
    ;

CASES: 
    /* EMPTY */ { $$ = NULL; }
    | CASES SINGLE_CASE
    ;

SINGLE_CASE: 
    CASE PRIMARY_CASE COLON STATEMENTS

    // Error Handling:
    | CASE error {
        reportError(SYNTAX_ERROR, "Expected Case Value After 'case'", @2.first_line);
        yyerrok;
    }
    | CASE PRIMARY_CASE error {
        reportError(SYNTAX_ERROR, "Expected ':' After Case Value", @3.first_line);
        yyerrok;
    }
    ;

DEFAULT_CASE:                                                                  
    DEFAULT COLON STATEMENTS

    // Error Handling:
    | DEFAULT error {
        reportError(SYNTAX_ERROR, "Expected ':' After 'default'", @2.first_line);
        yyerrok;
    }
    ;

PRIMARY_CASE: 
    INTVALUE
    | CHARVALUE
    | BOOLVALUE
    | IDENTIFIER
    | FLOATVALUE
    | STRINGVALUE
    ;

RETURN_STATEMENT: 
    RETURN
    | RETURN LOGICAL_EXPRESSION
    ;

FUNCTION_DEFINITION_IMPLEMENTATION :
    FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

    // Error Handling:
    | FUNCTION error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'function'", @2.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Function Datatype", @3.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '(' After Function Name", @4.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST error {
        reportError(SYNTAX_ERROR, "Expected ')' After Parameters", @6.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' To Start Function Body", @7.first_line);
        yyerrok;
    }
    | FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close Function Body", @9.first_line);
        yyerrok;
    }
    ;

PARAMETER_LIST: 
    /* EMPTY */ { $$ = NULL; }        
    | PARAM_LIST_NONEMPTY          
    ;

PARAM_LIST_NONEMPTY :
    DATATYPE IDENTIFIER                              
    | PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER 

    // Error Handling:
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @2.first_line);
        yyerrok;
    }
    | PARAM_LIST_NONEMPTY COMMA error {
        reportError(SYNTAX_ERROR, "Expected Datatype After ',' In Parameter List", @3.first_line);
        yyerrok;
    }
    | PARAM_LIST_NONEMPTY COMMA DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @4.first_line);
        yyerrok;
    }  
    ;

ARGUMENT_LIST: 
    /* EMPTY */ { $$ = NULL; }
    | ARGUMENTS
    ;

ARGUMENTS: 
    LOGICAL_EXPRESSION
    | ARGUMENTS COMMA LOGICAL_EXPRESSION

    // Error Handling:
    | ARGUMENTS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Expression After ',' In Arguments", @3.first_line);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unexpected ',' Before First Argument", @2.first_line);
        yyerrok;
    }
    ;

LOGICAL_EXPRESSION: 
    LOGICAL_EXPRESSION OR LOGICAL_TERM { $$ = $1; }
    | LOGICAL_TERM { $$ = $1; }

    // Error Handling:
    | LOGICAL_EXPRESSION OR error {
        reportError(SYNTAX_ERROR, "Expected Expression After '||'", @3.first_line);
        yyerrok;
    }
    | OR error {
        reportError(SYNTAX_ERROR, "Unexpected '||' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

LOGICAL_TERM: 
    LOGICAL_TERM AND EQUALITY_EXPRESSION { $$ = $1; }
    | EQUALITY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | LOGICAL_TERM AND error {
        reportError(SYNTAX_ERROR, "Expected Expression After '&&'", @3.first_line);
        yyerrok;
    }
    | AND error {
        reportError(SYNTAX_ERROR, "Unexpected '&&' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

EQUALITY_EXPRESSION: 
    EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION { $$ = $1; }
    | EQUALITY_EXPRESSION NEQ RELATIONAL_EXPRESSION { $$ = $1; }
    | RELATIONAL_EXPRESSION { $$ = $1; }

    // Error Handling:
    | EQUALITY_EXPRESSION EQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '=='", @3.first_line);
        yyerrok;
    }
    | EQUALITY_EXPRESSION NEQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!='", @3.first_line);
        yyerrok;
    }
    | EQ error {
        reportError(SYNTAX_ERROR, "Unexpected '==' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | NEQ error {
        reportError(SYNTAX_ERROR, "Unexpected '!=' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

RELATIONAL_EXPRESSION: 
    RELATIONAL_EXPRESSION ST ADDITIVE_EXPRESSION { $$ = $1; }
    | RELATIONAL_EXPRESSION GT ADDITIVE_EXPRESSION { $$ = $1; }
    | RELATIONAL_EXPRESSION SE ADDITIVE_EXPRESSION { $$ = $1; }
    | RELATIONAL_EXPRESSION GE ADDITIVE_EXPRESSION { $$ = $1; }
    | ADDITIVE_EXPRESSION { $$ = $1; }

    // Error Handling:
    | RELATIONAL_EXPRESSION ST error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<'", @3.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>'", @3.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION SE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<='", @3.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>='", @3.first_line);
        yyerrok;
    }
    | ST error {
        reportError(SYNTAX_ERROR, "Unexpected '<' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | GT error {
        reportError(SYNTAX_ERROR, "Unexpected '>' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | SE error {
        reportError(SYNTAX_ERROR, "Unexpected '<=' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | GE error {
        reportError(SYNTAX_ERROR, "Unexpected '>=' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

ADDITIVE_EXPRESSION: 
    ADDITIVE_EXPRESSION PLUS MULTIPLICATIVE_EXPRESSION { $$ = $1; }
    | ADDITIVE_EXPRESSION MINUS MULTIPLICATIVE_EXPRESSION { $$ = $1; }
    | MULTIPLICATIVE_EXPRESSION { $$ = $1; }

    // Error Handling:
    | ADDITIVE_EXPRESSION PLUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '+'", @3.first_line);
        yyerrok;
    }
    | ADDITIVE_EXPRESSION MINUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '-'", @3.first_line);
        yyerrok;
    }
    | PLUS error {
        reportError(SYNTAX_ERROR, "Unexpected '+' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

MULTIPLICATIVE_EXPRESSION: 
    MULTIPLICATIVE_EXPRESSION MULTIPLY EXPONENT_EXPRESSION { $$ = $1; }
    | MULTIPLICATIVE_EXPRESSION DIVIDE EXPONENT_EXPRESSION { $$ = $1; }
    | MULTIPLICATIVE_EXPRESSION MODULO EXPONENT_EXPRESSION { $$ = $1; }
    | EXPONENT_EXPRESSION { $$ = $1; }

    // Error Handling:
    | MULTIPLICATIVE_EXPRESSION MULTIPLY error {
        reportError(SYNTAX_ERROR, "Expected Expression After '*'", @3.first_line);
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION DIVIDE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '/'", @3.first_line);
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION MODULO error {
        reportError(SYNTAX_ERROR, "Expected Expression After '%'", @3.first_line);
        yyerrok;
    }
    | MULTIPLY error {
        reportError(SYNTAX_ERROR, "Unexpected '*' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | DIVIDE error {
        reportError(SYNTAX_ERROR, "Unexpected '/' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    | MODULO error {
        reportError(SYNTAX_ERROR, "Unexpected '%' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

EXPONENT_EXPRESSION: 
    EXPONENT_EXPRESSION POWER UNARY_EXPRESSION { $$ = $1; }
    | UNARY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | EXPONENT_EXPRESSION POWER error {
        reportError(SYNTAX_ERROR, "Expected Expression After '**'", @3.first_line);
        yyerrok;
    }
    | POWER error {
        reportError(SYNTAX_ERROR, "Unexpected '**' At Start Of Expression", @2.first_line);
        yyerrok;
    }
    ;

UNARY_EXPRESSION: 
    MINUS UNARY_EXPRESSION %prec UMINUS { $$ = $2; }
    | NOT UNARY_EXPRESSION { $$ = $2; }
    | PRIMARY_EXPRESSION { $$ = $1; }

    // Error Handling:
    | NOT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!'", @2.first_line);
        yyerrok;
    }
    | MINUS error %prec UMINUS {
        reportError(SYNTAX_ERROR, "Expected Expression After Unary '-'", @2.first_line);
        yyerrok;
    }
    ;

PRIMARY_EXPRESSION:
    INTVALUE {
        value v; memset(&v, 0, sizeof(v));
        v.INT_Data = $1;
        $$ = makeExpr(INT_TYPE, v, NULL);
    }
    | FLOATVALUE {
        value v; memset(&v, 0, sizeof(v));
        v.FLOAT_Data = $1;
        $$ = makeExpr(FLOAT_TYPE, v, NULL);
    }
    | CHARVALUE {
        value v; memset(&v, 0, sizeof(v));
        v.CHAR_Data = $1;
        $$ = makeExpr(CHAR_TYPE, v, NULL);
    }
    | BOOLVALUE {
        value v; memset(&v, 0, sizeof(v));
        v.BOOL_Data = $1;
        $$ = makeExpr(BOOL_TYPE, v, NULL);
    }
    | STRINGVALUE {
        value v; memset(&v, 0, sizeof(v));
        v.STRING_Data = strdup($1);
        $$ = makeExpr(STRING_TYPE, v, NULL);
    }
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET {
        $$ = $2;
    }
    | IDENTIFIER PRIMARY_SUFFIX {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, $1);
        if (!e) {
            reportError(SEMANTIC_ERROR, "Use of undeclared identifier", @1.first_line);
            value v; memset(&v, 0, sizeof(v));
            $$ = makeExpr(INT_TYPE, v, $1);
        } else {
            $$ = makeExpr(e->identifierType, e->currentValue, $1);
        }
    }

    // Error handling (keep yours)
    | LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )'", @2.first_line);
        yyerrok;
        $$ = NULL;
    }
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')'", @3.first_line);
        yyerrok;
        $$ = $2;
    }
    ;

PRIMARY_SUFFIX: 
    /*EMPTY*/ { $$ = NULL; }
    | LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET
    ;

%%

void yyerror(const char* String) { }

int main(int argc , char** argv) {
    if(argc > 1) {
        yyin = fopen(argv[1] , "r");
        if(!yyin) {
            fprintf(stderr, "Could not open file: %s\n", argv[1]);
            exit(1);
        }
    } 
    else yyin = stdin; // GUI RELATED
    scopeStackInit(&gScopeStack);
    enterScope(&gScopeStack);
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
    return (parseResult == 0 && getErrorCount() == 0) ? 0 : 1;
}