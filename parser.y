/* Definitions */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    int yylex(void);
    void yyerror(char *);
    extern FILE *yyin;
%}

%union 
{
    int intData;
    float floatData;    
    char charData;
    char* stringData;
    int boolData;
    void* VOID_DATA;
}

/* Tokens sent from lexer */

%token IF ELSE WHILE REPEAT UNTIL FOR SWITCH CASE CONST RETURN CONTINUE BREAK FUNCTION DEFAULT
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


/* Types for Grammar Rules */
%type <VOID_DATA> statements statement
%type <VOID_DATA> DECLARATION ASSIGNMENT IF_CONDITION IF_ONLY IF_ELSE IF_HEADER IF_BODY
%type <VOID_DATA> REPEAT_UNTIL_LOOP REPEAT_BODY UNTIL_STATEMENT WHILE_LOOP WHILE_HEADER WHILE_BODY
%type <VOID_DATA> SWITCH_CASE SWITCH_HEADER SWITCH_BODY CASE_LIST CASE_ELEMENT
%type <VOID_DATA> FUNCTION_DEFINITION_IMPLEMENTATION PARAMETER_LIST PARAM_LIST_NONEMPTY
%type <VOID_DATA> FUNCTION_CALL ARGUMENT_LIST ARGUMENT_LIST_NONEMPTY
%type <VOID_DATA> FOR_LOOP FOR_HEADER FOR_BODY ITERATOR_DECLARATION SCOPE_OF_STATEMENTS
%type <intData> EXPRESSION LOGICAL_EXPRESSION LOGICAL_EXPRESSION_TAIL LOGICAL_EXPRESSION_FACTOR LOGICAL_EXPRESSION_FACTOR_TAIL
%type <intData> LOGICAL_EXPRESSION_TERM ARITHMETIC_EXPRESSION_TERM ARITHMETIC_EXPRESSION_FACTOR ARITHMETIC_EXPRESSION_UNARY PRIMARY_EXPRESSION

%%
/* Entry Rule */
program : 
statements  /* A program contains some statements */
;


/* Statements contains multiple statement */
statements : 
/* A statement can be empty */                                 { $$ = NULL; }
| statements statement   
;


/* A statement can exist is a block or not ex: cannot declare a func inside if body */
statement :       /* Types of statements that could exist in our language (till now might add others later) */
DECLARATION SEMI_COLON                                 
| ASSIGNMENT SEMI_COLON      
| IF_CONDITION                                          
| WHILE_LOOP                                          
| REPEAT_UNTIL_LOOP                                                
| FOR_LOOP          
| CONTINUE SEMI_COLON                     
| BREAK SEMI_COLON                                           
| SWITCH_CASE       
| FUNCTION_DEFINITION_IMPLEMENTATION                                             
| FUNCTION_CALL SEMI_COLON                            
| RETURN SEMI_COLON                                                  
| SCOPE_OF_STATEMENTS    
| EXPRESSION SEMI_COLON                                
;


/* uninit dec , init dec , const dec + init */
DECLARATION : 
DATATYPE IDENTIFIER                            { $$ = NULL; }
| DATATYPE IDENTIFIER EQUAL EXPRESSION         { $$ = NULL; }
| CONST DATATYPE IDENTIFIER COLON EXPRESSION   { $$ = NULL; }
;


ASSIGNMENT :
IDENTIFIER EQUAL EXPRESSION           /* function calls are added in EXPRESSION --> (int x = add(3,5); ) , also func calls are in statement --> (add(3,5); left in the main should not give an error or for void functions) */         
| IDENTIFIER INCREMENT                /* post-inc */
| IDENTIFIER DECREMENT                /* post-dec */
| INCREMENT IDENTIFIER                /* pre-inc */
| DECREMENT IDENTIFIER                /* pre-dec */
;


/* IF , IF with ELSE */
IF_CONDITION :
IF_ONLY
| IF_ELSE
;


IF_ONLY :
IF_HEADER IF_BODY
| IF_HEADER SEMI_COLON
;


IF_ELSE :
IF_HEADER IF_BODY ELSE IF_BODY
| IF_HEADER IF_BODY ELSE SEMI_COLON   
;


IF_HEADER :
IF LEFT_ROUND_BRACKET EXPRESSION RIGHT_ROUND_BRACKET
;


/* Could be a body (true scope) or exactly one statement */
IF_BODY :
SCOPE_OF_STATEMENTS
| statement              /* This is not completely true as what if the statement was a func declaration or loop keywords (continue , break) it would be completely wrong , thus it will be handled semantically in PH2 isA*/
;


REPEAT_UNTIL_LOOP :
REPEAT REPEAT_BODY UNTIL_STATEMENT SEMI_COLON
;


REPEAT_BODY :
SCOPE_OF_STATEMENTS  
| statement
;


UNTIL_STATEMENT :
UNTIL LEFT_ROUND_BRACKET EXPRESSION RIGHT_ROUND_BRACKET
;


WHILE_LOOP :
WHILE_HEADER WHILE_BODY
| WHILE_HEADER SEMI_COLON
;

WHILE_HEADER :
WHILE LEFT_ROUND_BRACKET EXPRESSION RIGHT_ROUND_BRACKET 
;


WHILE_BODY :
SCOPE_OF_STATEMENTS
| statement
;


FOR_LOOP :
FOR_HEADER FOR_BODY
| FOR_HEADER SEMI_COLON
;


FOR_HEADER :    /* Fix EXPRESSION not statement , easier semantic checks for PH2 isA */
FOR LEFT_ROUND_BRACKET ITERATOR_DECLARATION SEMI_COLON EXPRESSION SEMI_COLON EXPRESSION RIGHT_ROUND_BRACKET
;


FOR_BODY :
SCOPE_OF_STATEMENTS                   /* not all statements are allowed inside the for body scope */
| statement
;


ITERATOR_DECLARATION :
DATATYPE IDENTIFIER EQUAL EXPRESSION 
| IDENTIFIER EQUAL EXPRESSION 
| IDENTIFIER 
;


SWITCH_CASE :
SWITCH_HEADER SWITCH_BODY
;


SWITCH_HEADER :
SWITCH LEFT_ROUND_BRACKET EXPRESSION RIGHT_ROUND_BRACKET
;


SWITCH_BODY :
LEFT_CURLY_BRACKET CASE_LIST RIGHT_CURLY_BRACKET
;


CASE_LIST :
/* empty */                   { $$ = NULL; }
| CASE_LIST CASE_ELEMENT      { $$ = NULL; }
| CASE_LIST DEFAULT_ELEMENT   { $$ = NULL; }
;     


CASE_ELEMENT :
CASE VALUE COLON SCOPE_OF_STATEMENTS
;


DEFAULT_ELEMENT :
DEFAULT COLON SCOPE_OF_STATEMENTS
;


/* these are the only supported types in a switch statement (decision was taken based on C syntax and some research on implementation algos for strings and floats) */
VALUE : 
INTVALUE
| CHARVALUE
| BOOLVALUE
;


FUNCTION_DEFINITION_IMPLEMENTATION :
FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET SCOPE_OF_STATEMENTS
;


PARAMETER_LIST :
/* empty */                     { $$ = NULL; }
| PARAM_LIST_NONEMPTY           { $$ = $1; }
;


PARAM_LIST_NONEMPTY :
DATATYPE IDENTIFIER                               { /* first param , should be added to some sort of DS that can store params of a func */ }
| PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER   { /* additional param */ }
;


FUNCTION_CALL :
IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET
;


ARGUMENT_LIST :
/* empty */                     { $$ = NULL; }
| ARGUMENT_LIST_NONEMPTY
;

ARGUMENT_LIST_NONEMPTY :
EXPRESSION                               { /* first arg */ }
| ARGUMENT_LIST_NONEMPTY COMMA EXPRESSION   { /* additional arg */ }
;



SCOPE_OF_STATEMENTS :
LEFT_CURLY_BRACKET statements RIGHT_CURLY_BRACKET
;


EXPRESSION :
LOGICAL_EXPRESSION
;


LOGICAL_EXPRESSION :
LOGICAL_EXPRESSION OR LOGICAL_EXPRESSION_TAIL       { $$ = $1 || $3; }
| LOGICAL_EXPRESSION_TAIL                           { $$ = $1; }
;

LOGICAL_EXPRESSION_TAIL :
LOGICAL_EXPRESSION_TAIL AND LOGICAL_EXPRESSION_FACTOR    { $$ = $1 && $3; }
| LOGICAL_EXPRESSION_FACTOR                              { $$ = $1; }
;


LOGICAL_EXPRESSION_FACTOR :
LOGICAL_EXPRESSION_FACTOR EQ LOGICAL_EXPRESSION_FACTOR_TAIL      { $$ = ($1 == $3); }
| LOGICAL_EXPRESSION_FACTOR NEQ LOGICAL_EXPRESSION_FACTOR_TAIL   { $$ = ($1 != $3); }
| LOGICAL_EXPRESSION_FACTOR_TAIL                                 { $$ = $1; }
;


LOGICAL_EXPRESSION_FACTOR_TAIL :
LOGICAL_EXPRESSION_FACTOR_TAIL GT LOGICAL_EXPRESSION_TERM      { $$ = ($1 > $3);  }
| LOGICAL_EXPRESSION_FACTOR_TAIL GE LOGICAL_EXPRESSION_TERM    { $$ = ($1 >= $3); }
| LOGICAL_EXPRESSION_FACTOR_TAIL ST LOGICAL_EXPRESSION_TERM    { $$ = ($1 < $3); }
| LOGICAL_EXPRESSION_FACTOR_TAIL SE LOGICAL_EXPRESSION_TERM    { $$ = ($1 <= $3); }
;


LOGICAL_EXPRESSION_TERM :
LOGICAL_EXPRESSION_TERM PLUS ARITHMETIC_EXPRESSION_TERM        { $$ = $1 + $3; }
| LOGICAL_EXPRESSION_TERM MINUS ARITHMETIC_EXPRESSION_TERM     { $$ = $1 - $3; }
| ARITHMETIC_EXPRESSION_TERM                                 { $$ = $1; }
;


ARITHMETIC_EXPRESSION_TERM :
ARITHMETIC_EXPRESSION_TERM MULTIPLY ARITHMETIC_EXPRESSION_FACTOR      { $$ = $1 * $3; }
| ARITHMETIC_EXPRESSION_TERM DIVIDE ARITHMETIC_EXPRESSION_FACTOR      { if($3 == 0) { yyerror("Can not divide by zero!"); } else { $$ = $1 / $3; } }
| ARITHMETIC_EXPRESSION_TERM MODULO ARITHMETIC_EXPRESSION_FACTOR      { $$ = $1 % $3; }
| ARITHMETIC_EXPRESSION_FACTOR                                        { $$ = $1; }
;


ARITHMETIC_EXPRESSION_FACTOR :
ARITHMETIC_EXPRESSION_UNARY POWER ARITHMETIC_EXPRESSION_FACTOR      { $$ = pow($1 , $3); }
| ARITHMETIC_EXPRESSION_UNARY                                       { $$ = $1; }
;


ARITHMETIC_EXPRESSION_UNARY :
MINUS ARITHMETIC_EXPRESSION_UNARY           { $$ = -$2; }
| NOT ARITHMETIC_EXPRESSION_UNARY           { $$ = !$2; } 
| PRIMARY_EXPRESSION                        { $$ = $1; }
;


PRIMARY_EXPRESSION :
LEFT_ROUND_BRACKET EXPRESSION RIGHT_ROUND_BRACKET    { /* recursion */ $$ = $2; }
| INTVALUE                                           { $$ = $1; }
| FLOATVALUE                                         { $$ = $1; }
| CHARVALUE                                          { $$ = $1; }
| STRINGVALUE                                        { $$ = $1; }
| BOOLVALUE                                          { $$ = $1; }
| IDENTIFIER                                         { /* an expression can have variables or constants */ $$ = $1; }
| FUNCTION_CALL                                      { /* a func can return a value (in semantics check func does not return void) */ $$ = $1; }
;


%%

void yyerror(char *s) 
{
    fprintf(stderr, "%s\n", s);
}

int main(int argc , char** argv) 
{
    if(argc > 1)
    {
        yyin = fopen(argv[1] , "r");
        if(!yyin)
        {
            fprintf(stderr, "Could not open file: %s\n", argv[1]);
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "Test file argument is missing!\n");
        exit(1);
    }

    if(yyparse() == 0)
    {
        return 0;
    }
    return 1;
}