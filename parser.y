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

%locations

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
%type <VOID_DATA> EXPRESSION_STATEMENT   /* CHANGED: keep as VOID_DATA */
%type <VOID_DATA> FUNCTION_CALL          /* CHANGED: keep as VOID_DATA */
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
    | ASSIGNMENT SEMI_COLON
    /* CHANGED: LOGICAL_EXPRESSION SEMI_COLON -> EXPRESSION_STATEMENT SEMI_COLON */
    | EXPRESSION_STATEMENT SEMI_COLON                 /* CHANGED */
    | RETURN_STATEMENT SEMI_COLON
    | BREAK SEMI_COLON
    | CONTINUE SEMI_COLON
    | SEMI_COLON
    | LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET
    | IF_STATEMENT
    | WHILE_STATEMENT
    | FOR_STATEMENT
    | REPEAT_STATEMENT
    | SWITCH_STATEMENT
    | FUNCTION_DEFINITION_IMPLEMENTATION

    // Error Handling:
    | DECLARATION error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    | ASSIGNMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    /* CHANGED: LOGICAL_EXPRESSION error -> EXPRESSION_STATEMENT error */
    | EXPRESSION_STATEMENT error {                     /* CHANGED */
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    | RETURN_STATEMENT error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    | BREAK error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    | CONTINUE error {
        reportError(SYNTAX_ERROR, "Expected ';'", @1.first_line);
        yyerrok;
    }
    | error SEMI_COLON {
        reportError(SYNTAX_ERROR, "Invalid statement", @2.first_line);
        yyerrok;
    }
    | LEFT_CURLY_BRACKET STATEMENTS error {
        reportError(SYNTAX_ERROR, "Expected '}'", @1.first_line);
        yyerrok;
    }
    | error STATEMENTS RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected '{'", @3.first_line);
        yyerrok;
    }
    | ELSE {
        reportError(SYNTAX_ERROR, "Unxpected 'else' Without Matching 'if'", @1.first_line);
        yyerrok;
    }
    | RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Unxpected '}' Without Matching '{'", @1.first_line);
        yyerrok;
    }
    | IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '(", @1.first_line);
        yyerrok;
    }
    ;

/* CHANGED: make expression-statement ONLY a function call.
   This avoids IDENTIFIER being a statement by itself, and removes the type-clash warnings. */
EXPRESSION_STATEMENT:                                 /* CHANGED */
      FUNCTION_CALL { $$ = NULL; }                    /* CHANGED */
    ;                                                 /* CHANGED */

FUNCTION_CALL:                                        /* CHANGED */
      IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET { $$ = NULL; }  /* CHANGED */
    ;                                                 /* CHANGED */

DECLARATION: 
    DATATYPE IDENTIFIERS
    | DATATYPE DECLARATORS
    | CONST DATATYPE DECLARATORS

    // Error Handling:
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype", @1.first_line);
        yyerrok;
    }
    | CONST error {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'const'", @1.first_line);
        yyerrok;
    }
    | CONST DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After 'const datatype'", @2.first_line);
        yyerrok;
    }
    | CONST DATATYPE SEMI_COLON {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After 'const datatype'", @3.first_line);
        yyerrok;
    }
    | DATATYPE CONST {
        reportError(SYNTAX_ERROR, "Unxpected Datatype Before 'const'", @1.first_line);
        yyerrok;
    }
    ;

IDENTIFIERS: 
    IDENTIFIER
    | IDENTIFIERS COMMA IDENTIFIER

    // Error Handling:
    | IDENTIFIERS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Identifier After ','", @2.first_line);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier/Declarator", @1.first_line);
        yyerrok;
    }
    ;

DECLARATORS: 
    DECLARATOR
    | DECLARATORS COMMA DECLARATOR

    // Error Handling:
    | DECLARATORS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After ','", @2.first_line);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier/Declarator", @1.first_line);
        yyerrok;
    }
    ;

DECLARATOR: 
    IDENTIFIER
    | IDENTIFIER EQUAL LOGICAL_EXPRESSION

    // Error Handling:
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line);
        yyerrok;
    }
    ;

ASSIGNMENT: 
    IDENTIFIER EQUAL LOGICAL_EXPRESSION
    | IDENTIFIER INCREMENT
    | IDENTIFIER DECREMENT
    | INCREMENT IDENTIFIER
    | DECREMENT IDENTIFIER

    // Error Handling:
    | EQUAL {
        reportError(SYNTAX_ERROR, "Expected Identifier Before '='", @1.first_line);
        yyerrok;
    }
    | error EQUAL LOGICAL_EXPRESSION {
        reportError(SYNTAX_ERROR, "Expected Identifier Before '='", @2.first_line);
        yyerrok;
    }
    | IDENTIFIER EQUAL error {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", @2.first_line);
        yyerrok;
    }
    | INCREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '++'", @1.first_line);
        yyerrok;
    }
    | DECREMENT error {
        reportError(SYNTAX_ERROR, "Expected Identifier After '--'", @1.first_line);
        yyerrok;
    }
    ;

IF_STATEMENT: 
    IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET %prec LOWER_THAN_ELSE
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET ELSE LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

    // Error Handling:
    | IF LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'if'", @1.first_line);
        yyerrok;
    }
    | IF error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'if'", @1.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @4.first_line);
        yyerrok;
    }
    | IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'if (condition)'", @1.first_line);
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
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'while'", @1.first_line);
        yyerrok;
    }
    | WHILE error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'while'", @1.first_line);
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", @4.first_line);
        yyerrok;
    }
    | WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Expected '{' After 'while (condition)'", @1.first_line);
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
        reportError(SYNTAX_ERROR, "Expected '{' After 'repeat'", @1.first_line);
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
        reportError(SYNTAX_ERROR, "Expected '(' After 'until'", @5.first_line);
        yyerrok;
    }
    | REPEAT LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET UNTIL LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET SEMI_COLON {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'until'", @6.first_line);
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
        reportError(SYNTAX_ERROR, "Expected '(' After 'for'", @1.first_line);
        yyerrok;
    }
    | FOR LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Invalid 'for' Header", @1.first_line);
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
        reportError(SYNTAX_ERROR, "Expected Expression After '=' In For-Initializer", @2.first_line);
        yyerrok;
    }
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype In For-Initializer", @1.first_line);
        yyerrok;
    }
    | DATATYPE IDENTIFIER error {
        reportError(SYNTAX_ERROR, "Expected '=' After Identifier In For-Initializer", @2.first_line);
        yyerrok;
    }
    ;

SWITCH_STATEMENT: 
    SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE RIGHT_CURLY_BRACKET

    // Error Handling:
    | SWITCH error {
        reportError(SYNTAX_ERROR, "Expected '(' After 'switch'", @1.first_line);
        yyerrok;
    }
    | SWITCH LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE RIGHT_CURLY_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )' After 'switch'", @1.first_line);
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
    | SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET CASES DEFAULT_CASE error {
        reportError(SYNTAX_ERROR, "Expected '}' To Close Switch Body", @8.first_line);
        yyerrok;
    }
    ;

CASES: 
    /* empty */
    | CASES SINGLE_CASE
    ;

SINGLE_CASE: 
    CASE PRIMARY_CASE COLON STATEMENTS

    // Error Handling:
    | CASE error {
        reportError(SYNTAX_ERROR, "Expected Case Value After 'case'", @1.first_line);
        yyerrok;
    }
    | CASE PRIMARY_CASE error {
        reportError(SYNTAX_ERROR, "Expected ':' After Case Value", @3.first_line);
        yyerrok;
    }
    ;

DEFAULT_CASE: 
    /* empty */
    | DEFAULT COLON STATEMENTS

    // Error Handling:
    | DEFAULT error {
        reportError(SYNTAX_ERROR, "Expected ':' After 'default'", @1.first_line);
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

    // Error Handling:
    | RETURN error {
        reportError(SYNTAX_ERROR, "Expected Expression After 'return'", @1.first_line);
        yyerrok;
    }
    ;

FUNCTION_DEFINITION_IMPLEMENTATION :
    FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET LEFT_CURLY_BRACKET STATEMENTS RIGHT_CURLY_BRACKET

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
    | FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Invalid Parameter List", @4.first_line);
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
    /* empty */                   
    | PARAM_LIST_NONEMPTY          
    ;

PARAM_LIST_NONEMPTY :
    DATATYPE IDENTIFIER                              
    | PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER 

    // Error Handling:
    | DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @1.first_line);
        yyerrok;
    }
    | PARAM_LIST_NONEMPTY COMMA error {
        reportError(SYNTAX_ERROR, "Expected Datatype After ',' In Parameter List", @2.first_line);
        yyerrok;
    }
    | PARAM_LIST_NONEMPTY COMMA DATATYPE error {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", @3.first_line);
        yyerrok;
    }  
    ;

ARGUMENT_LIST: 
    /* empty */
    | ARGUMENTS
    ;

ARGUMENTS: 
    LOGICAL_EXPRESSION
    | ARGUMENTS COMMA LOGICAL_EXPRESSION

    // Error Handling:
    | ARGUMENTS COMMA error {
        reportError(SYNTAX_ERROR, "Expected Expression After ',' In Arguments", @2.first_line);
        yyerrok;
    }
    | COMMA error {
        reportError(SYNTAX_ERROR, "Unexpected ',' Before First Argument", @1.first_line);
        yyerrok;
    }
    ;

LOGICAL_EXPRESSION: 
    LOGICAL_EXPRESSION OR LOGICAL_TERM
    | LOGICAL_TERM

    // Error Handling:
    | LOGICAL_EXPRESSION OR error {
        reportError(SYNTAX_ERROR, "Expected Expression After '||'", @2.first_line);
        yyerrok;
    }
    | OR error {
        reportError(SYNTAX_ERROR, "Unexpected '||' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

LOGICAL_TERM: 
    LOGICAL_TERM AND EQUALITY_EXPRESSION
    | EQUALITY_EXPRESSION

    // Error Handling:
    | LOGICAL_TERM AND error {
        reportError(SYNTAX_ERROR, "Expected Expression After '&&'", @2.first_line);
        yyerrok;
    }
    | AND error {
        reportError(SYNTAX_ERROR, "Unexpected '&&' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

EQUALITY_EXPRESSION: 
    EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION
    | EQUALITY_EXPRESSION NEQ RELATIONAL_EXPRESSION
    | RELATIONAL_EXPRESSION

    // Error Handling:
    | EQUALITY_EXPRESSION EQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '=='", @2.first_line);
        yyerrok;
    }
    | EQUALITY_EXPRESSION NEQ error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!='", @2.first_line);
        yyerrok;
    }
    | EQ error {
        reportError(SYNTAX_ERROR, "Unexpected '==' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | NEQ error {
        reportError(SYNTAX_ERROR, "Unexpected '!=' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

RELATIONAL_EXPRESSION: 
    RELATIONAL_EXPRESSION ST ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION GT ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION SE ADDITIVE_EXPRESSION
    | RELATIONAL_EXPRESSION GE ADDITIVE_EXPRESSION
    | ADDITIVE_EXPRESSION

    // Error Handling:
    | RELATIONAL_EXPRESSION ST error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<'", @2.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>'", @2.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION SE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '<='", @2.first_line);
        yyerrok;
    }
    | RELATIONAL_EXPRESSION GE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '>='", @2.first_line);
        yyerrok;
    }
    | ST error {
        reportError(SYNTAX_ERROR, "Unexpected '<' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | GT error {
        reportError(SYNTAX_ERROR, "Unexpected '>' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | SE error {
        reportError(SYNTAX_ERROR, "Unexpected '<=' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | GE error {
        reportError(SYNTAX_ERROR, "Unexpected '>=' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

ADDITIVE_EXPRESSION: 
    ADDITIVE_EXPRESSION PLUS MULTIPLICATIVE_EXPRESSION
    | ADDITIVE_EXPRESSION MINUS MULTIPLICATIVE_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION

    // Error Handling:
    | ADDITIVE_EXPRESSION PLUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '+'", @2.first_line);
        yyerrok;
    }
    | ADDITIVE_EXPRESSION MINUS error {
        reportError(SYNTAX_ERROR, "Expected Expression After '-'", @2.first_line);
        yyerrok;
    }
    | PLUS error {
        reportError(SYNTAX_ERROR, "Unexpected '+' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

MULTIPLICATIVE_EXPRESSION: 
    MULTIPLICATIVE_EXPRESSION MULTIPLY EXPONENT_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION DIVIDE EXPONENT_EXPRESSION
    | MULTIPLICATIVE_EXPRESSION MODULO EXPONENT_EXPRESSION
    | EXPONENT_EXPRESSION

    // Error Handling:
    | MULTIPLICATIVE_EXPRESSION MULTIPLY error {
        reportError(SYNTAX_ERROR, "Expected Expression After '*'", @2.first_line);
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION DIVIDE error {
        reportError(SYNTAX_ERROR, "Expected Expression After '/'", @2.first_line);
        yyerrok;
    }
    | MULTIPLICATIVE_EXPRESSION MODULO error {
        reportError(SYNTAX_ERROR, "Expected Expression After '%'", @2.first_line);
        yyerrok;
    }
    | MULTIPLY error {
        reportError(SYNTAX_ERROR, "Unexpected '*' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | DIVIDE error {
        reportError(SYNTAX_ERROR, "Unexpected '/' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    | MODULO error {
        reportError(SYNTAX_ERROR, "Unexpected '%' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

EXPONENT_EXPRESSION: 
    EXPONENT_EXPRESSION POWER UNARY_EXPRESSION
    | UNARY_EXPRESSION

    // Error Handling:
    | EXPONENT_EXPRESSION POWER error {
        reportError(SYNTAX_ERROR, "Expected Expression After '**'", @2.first_line);
        yyerrok;
    }
    | POWER error {
        reportError(SYNTAX_ERROR, "Unexpected '**' At Start Of Expression", @1.first_line);
        yyerrok;
    }
    ;

UNARY_EXPRESSION: 
    MINUS UNARY_EXPRESSION %prec UMINUS
    | NOT UNARY_EXPRESSION
    | PRIMARY_EXPRESSION

    // Error Handling:
    | NOT error {
        reportError(SYNTAX_ERROR, "Expected Expression After '!'", @1.first_line);
        yyerrok;
    }
    | MINUS error %prec UMINUS {
        reportError(SYNTAX_ERROR, "Expected Expression After Unary '-'", @1.first_line);
        yyerrok;
    }
    ;

PRIMARY_EXPRESSION: 
    INTVALUE
    | FLOATVALUE
    | CHARVALUE
    | BOOLVALUE
    | STRINGVALUE
    | IDENTIFIER PRIMARY_SUFFIX
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET

    // Error Handling:
    | LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )'", @1.first_line);
        yyerrok;
    }
    | LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error {
        reportError(SYNTAX_ERROR, "Expected ')'", @3.first_line);
        yyerrok;
    }
    ;

PRIMARY_SUFFIX:
    /* empty */
    | LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET

    // Error Handling:
    | LEFT_ROUND_BRACKET ARGUMENT_LIST error {
        reportError(SYNTAX_ERROR, "Expected ')' After Arguments", @3.first_line);
        yyerrok;
    }
    | LEFT_ROUND_BRACKET error {
        reportError(SYNTAX_ERROR, "Invalid Argument List", @1.first_line);
        yyerrok;
    }
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

    int parseResult = yyparse();
    int errorCount = getErrorCount();
    if (errorCount){
        printf("\n%d Errors:\n", errorCount);
        printErrors();
    }               
    return (parseResult == 0 && getErrorCount() == 0) ? 0 : 1;
}