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

    extern FILE* yyin;
    extern int yylex();
    extern int yyparse();
    extern int previousValidLine;

    void yyerror(const char* String);
%}

// Data Types:
%union 
{
    int intData;
    float floatData;    
    char charData;
    char* stringData;
    int boolData;
    void* VOID_DATA;
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
%type <VOID_DATA> STATEMENTS
/* %type <VOID_DATA>
%type <VOID_DATA>
%type <VOID_DATA>
%type <VOID_DATA>
%type <VOID_DATA>
%type <VOID_DATA>
%type <intData>
%type <intData> */

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
    /* empty */ { $$ = NULL; }
    | STATEMENTS STATEMENT
    ;

STATEMENT: 
    DECLARATION SEMI_COLON
    | DECLARATION error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | ASSIGNMENT SEMI_COLON
    | ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | LOGICAL_EXPRESSION SEMI_COLON
    | LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | RETURN_STATEMENT SEMI_COLON
    | RETURN_STATEMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | BREAK SEMI_COLON
    | BREAK error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | CONTINUE SEMI_COLON
    | CONTINUE error {
        reportError(SYNTAX_ERROR, "Expected ';'", previousValidLine);
        yyerrok;
    }
    | SEMI_COLON
    | error SEMI_COLON {
        reportError(SYNTAX_ERROR, "Invalid statement", previousValidLine);
        yyerrok;
    }
    | LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET
    | LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}'", previousValidLine);
        yyerrok;
    }
    | error STATEMENTS RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected '{'", previousValidLine);
        yyerrok;
    }
    | IF_STATEMENT
    | ELSE {
        reportError(SYNTAX_ERROR, "Unxpected 'else' Without Matching 'if'", previousValidLine);
        yyerrok;
    }
    | WHILE_STATEMENT
    | FOR_STATEMENT
    | REPEAT_STATEMENT
    | SWITCH_STATEMENT
    | FUNCTION_DEFINITION_IMPLEMENTATION
    | RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Unxpected '}' Without Matching '{'", previousValidLine);
        yyerrok;
    }
    | IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '(", previousValidLine);
        yyerrok;
    }
    ;

DECLARATION: 
    DATATYPE IDENTIFIERS
    | DATATYPE DECLARATORS
    | CONST DATATYPE DECLARATORS
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype", previousValidLine);
        yyerrok;
    }
    | CONST error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'const'", previousValidLine);
        yyerrok;
    }
    | CONST DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After 'const datatype'", previousValidLine);
        yyerrok;
    }
    | CONST DATATYPE SEMI_COLON {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After 'const datatype'", previousValidLine);
        yyerrok;
    }
    ;

IDENTIFIERS: 
    IDENTIFIER
    | IDENTIFIERS COMMA IDENTIFIER
    | IDENTIFIERS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Identifier After ','", previousValidLine);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier", previousValidLine);
        yyerrok;
    }
    ;

DECLARATORS: 
    DECLARATOR
    | DECLARATORS COMMA DECLARATOR
    | DECLARATORS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Declarator After ','", previousValidLine);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Declarator", previousValidLine);
        yyerrok;
    }
    ;

DECLARATOR: 
    IDENTIFIER
    | IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", previousValidLine);
        yyerrok;
    }
    ;

ASSIGNMENT: 
    IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | IDENTIFIER INCREMENT
    | IDENTIFIER DECREMENT
    | INCREMENT IDENTIFIER
    | DECREMENT IDENTIFIER
    ;

IF_STATEMENT: 
    IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET %prec LOWER_THAN_ELSE
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET ELSE LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET
    ;

WHILE_STATEMENT: 
    WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET
    ;

REPEAT_STATEMENT: 
    REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET SEMI_COLON
    ;

FOR_STATEMENT: 
    FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET
    ;

ITERATOR: 
    IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | DATATYPE IDENTIFIER EQUAL LOGICAL_EXPRESSION
    ;

SWITCH_STATEMENT: 
    SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE RIGHT_CURLY_BRACKET
    ;

CASES: 
    /* empty */
    | CASES SINGLE_CASE
    ;

SINGLE_CASE: 
    CASE PRIMARY_CASE COLON STATEMENTS
    ;

DEFAULT_CASE: 
    /* empty */
    | DEFAULT COLON STATEMENTS
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
    ;

PARAMETER_LIST:
    /* empty */                   
    | PARAM_LIST_NONEMPTY          
    ;

PARAM_LIST_NONEMPTY :
    DATATYPE IDENTIFIER                              
    | PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER   
    ;

ARGUMENT_LIST: 
    /* empty */
    | ARGUMENTS
    ;

ARGUMENTS: 
    LOGICAL_EXPRESSION
    | ARGUMENTS COMMA LOGICAL_EXPRESSION
    ;

LOGICAL_EXPRESSION: 
    LOGICAL_EXPRESSION OR LOGICAL_TERM
    | LOGICAL_TERM
    ;

LOGICAL_TERM: 
    LOGICAL_TERM AND EQUALITY_EXPRESSION
    | EQUALITY_EXPRESSION
    ;

EQUALITY_EXPRESSION: 
    EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION
    | EQUALITY_EXPRESSION NEQ RELATIONAL_EXPRESSION
    | RELATIONAL_EXPRESSION
    ;

RELATIONAL_EXPRESSION: 
    RELATIONAL_EXPRESSION ST ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION GT ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION SE ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION GE ADDITIVE_EXPRESSION
    | ADDITIVE_EXPRESSION
    ;

ADDITIVE_EXPRESSION: 
    ADDITIVE_EXPRESSION PLUS MULTIPLICATIVE_EXPRESSION
    | ADDITIVE_EXPRESSION MINUS MULTIPLICATIVE_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION
    ;

MULTIPLICATIVE_EXPRESSION: 
    MULTIPLICATIVE_EXPRESSION MULTIPLY EXPONENT_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION DIVIDE EXPONENT_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION MODULO EXPONENT_EXPRESSION
    | EXPONENT_EXPRESSION
    ;

EXPONENT_EXPRESSION: 
    EXPONENT_EXPRESSION POWER UNARY_EXPRESSION
    | UNARY_EXPRESSION
    ;

UNARY_EXPRESSION: 
    MINUS UNARY_EXPRESSION %prec UMINUS
    | NOT UNARY_EXPRESSION
    | PRIMARY_EXPRESSION
    ;

PRIMARY_EXPRESSION: 
    INTVALUE
    | FLOATVALUE
    | CHARVALUE
    | BOOLVALUE
    | STRINGVALUE
    | IDENTIFIER PRIMARY_SUFFIX
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET
    ;

PRIMARY_SUFFIX:
    /* empty */
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
    } else {
        fprintf(stderr, "Test file argument is missing!\n");
        exit(1);
    }

    int parseResult = yyparse();
         
    printErrors();                    
    return (parseResult == 0 && getErrorCount() == 0) ? 0 : 1;
}