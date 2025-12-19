/* Definitions */
%{
    #include <stdio.h>
    #include "parser.tab.h"
    extern FILE *yyin; /*Exclusive input for lexer*/
%}

/*Regex constructs for easier formations*/
/****************************************/
/*Helpers*/
digit [0-9]
varNameStart [a-zA-Z_]
acceptableNamingChar [a-zA-Z0-9_]

/*Values that can be assigned to int data type*/
intData {digit}+

/*Values that can be assigned to float data type*/
floatData ({digit}+"."{digit}*)|({digit}*"."{digit}+)

/*Values that can be assigned to char data type*/
charData \'([^'\\]|\\[bnrt\\'"])\'

/*Char with no closing quotes stops matching till it gets a char with nextchar ; I want to add comma and right round bracket*/
/*print('a);*/
charDataMissingQuote \'([^'\\;]|\\[bnrt\\'"])

/*Values that can be assigned to string data type*/
stringData \"([^"\\]|\\.)*\"

/*String with no closing quotes stops matching till it gets a char with nextchar ; I want to add comma and right round bracket*/
/*print("Mazen);*/
stringDataMissingQuote \"([^"\\;]|\\.)*

/*Identifiers (funcNames or varNames)*/
identifier {varNameStart}{acceptableNamingChar}*

/* Regex Rules */
%%

"if"                                        { printf("Rule %-20s matched: '%s'\n", "IF", yytext); /*return IF;*/ }
"else"                                      { printf("Rule %-20s matched: '%s'\n", "ELSE", yytext); /*return ELSE;*/ }
"while"                                     { printf("Rule %-20s matched: '%s'\n", "WHILE", yytext); /*return WHILE;*/ }
"repeat"                                    { printf("Rule %-20s matched: '%s'\n", "REPEAT", yytext); /*return REPEAT;*/ }
"until"                                     { printf("Rule %-20s matched: '%s'\n", "UNTIL", yytext); /*return UNTIL;*/ }
"for"                                       { printf("Rule %-20s matched: '%s'\n", "FOR", yytext); /*return FOR;*/ }
"switch"                                    { printf("Rule %-20s matched: '%s'\n", "SWITCH", yytext); /*return SWITCH;*/ }
"case"                                      { printf("Rule %-20s matched: '%s'\n", "CASE", yytext); /*return CASE;*/ }
"const"                                     { printf("Rule %-20s matched: '%s'\n", "CONST", yytext); /*return CONST;*/ }
"return"                                    { printf("Rule %-20s matched: '%s'\n", "RETURN", yytext); /*return RETURN;*/ }
"continue"                                  { printf("Rule %-20s matched: '%s'\n", "CONTINUE", yytext); /*return CONTINUE;*/ }
"break"                                     { printf("Rule %-20s matched: '%s'\n", "BREAK", yytext); /*return BREAK;*/ }
"function"                                  { printf("Rule %-20s matched: '%s'\n", "FUNCTION", yytext); /*return FUNCTION;*/ }
"default"                                   { printf("Rule %-20s matched: '%s'\n", "DEFAULT", yytext); /*return DEFAULT;*/ }

"||"                                        { printf("Rule %-20s matched: '%s'\n", "OR", yytext); /*return OR;*/ }
"&&"                                        { printf("Rule %-20s matched: '%s'\n", "AND", yytext); /*return AND;*/ }
"!"                                         { printf("Rule %-20s matched: '%s'\n", "NOT", yytext); /*return NOT;*/ }
">"                                         { printf("Rule %-20s matched: '%s'\n", "GT", yytext); /*return GT;*/ }
">="                                        { printf("Rule %-20s matched: '%s'\n", "GE", yytext); /*return GE;*/ }
"<"                                         { printf("Rule %-20s matched: '%s'\n", "ST", yytext); /*return ST;*/ }
"<="                                        { printf("Rule %-20s matched: '%s'\n", "SE", yytext); /*return SE;*/ }
"=="                                        { printf("Rule %-20s matched: '%s'\n", "EQ", yytext); /*return EQ;*/ }
"!="                                        { printf("Rule %-20s matched: '%s'\n", "NEQ", yytext); /*return NEQ;*/ }

"+"                                         { printf("Rule %-20s matched: '%s'\n", "PLUS", yytext); /*return PLUS;*/ }
"-"                                         { printf("Rule %-20s matched: '%s'\n", "MINUS", yytext); /*return MINUS;*/ }
"*"                                         { printf("Rule %-20s matched: '%s'\n", "MULTIPLY", yytext); /*return MULTIPLY;*/ }
"/"                                         { printf("Rule %-20s matched: '%s'\n", "DIVIDE", yytext); /*return DIVIDE;*/ }
"%"                                         { printf("Rule %-20s matched: '%s'\n", "MODULO", yytext); /*return MODULO;*/ }
"**"                                        { printf("Rule %-20s matched: '%s'\n", "POWER", yytext); /*return POWER;*/ }
"++"                                        { printf("Rule %-20s matched: '%s'\n", "INCREMENT", yytext); /*return INCREMENT;*/ }
"--"                                        { printf("Rule %-20s matched: '%s'\n", "DECREMENT", yytext); /*return DECREMENT;*/ }
"="                                         { printf("Rule %-20s matched: '%s'\n", "EQUAL", yytext); /*return EQUAL;*/ }

";"                                         { printf("Rule %-20s matched: '%s'\n", "SEMI_COLON", yytext); /*return SEMI_COLON;*/ }
","                                         { printf("Rule %-20s matched: '%s'\n", "COMMA", yytext); /*return COMMA;*/ }
":"                                         { printf("Rule %-20s matched: '%s'\n", "COLON", yytext); /*return COLON;*/ }
"("                                         { printf("Rule %-20s matched: '%s'\n", "LEFT_ROUND_BRACKET", yytext); /*return LEFT_ROUND_BRACKET;*/ }
")"                                         { printf("Rule %-20s matched: '%s'\n", "RIGHT_ROUND_BRACKET", yytext); /*return RIGHT_ROUND_BRACKET;*/ }
"{"                                         { printf("Rule %-20s matched: '%s'\n", "LEFT_CURLY_BRACKET", yytext); /*return LEFT_CURLY_BRACKET;*/ }
"}"                                         { printf("Rule %-20s matched: '%s'\n", "RIGHT_CURLY_BRACKET", yytext); /*return RIGHT_CURLY_BRACKET;*/ }

"int"|"float"|"char"|"string"|"bool"|"void" { printf("Rule %-20s matched: '%s'\n", "DATATYPE", yytext); /* return DATATYPE; */ }

"true"|"false"                              { printf("Rule %-20s matched: '%s'\n", "BOOLVALUE", yytext); /*return BOOLVALUE;*/ }
{intData}                                   { printf("Rule %-20s matched: '%s'\n", "INTVALUE", yytext); /*return INTVALUE;*/ }
{floatData}                                 { printf("Rule %-20s matched: '%s'\n", "FLOATVALUE", yytext); /*return FLOATVALUE;*/ }
{charData}                                  { printf("Rule %-20s matched: '%s'\n", "CHARVALUE", yytext); /*return CHARVALUE;*/ }
{stringData}                                { printf("Rule %-20s matched: '%s'\n", "STRINGVALUE", yytext); /*return STRINGVALUE;*/ }

{identifier}                                { printf("Rule %-20s matched: '%s'\n", "IDENTIFIER", yytext); /*return IDENTIFIER;*/ }

[ \t\n]+                                    { /* ignore whitespace tabs newlines */ }
"//".*                                      { printf("Single line comment\n");/* ignore single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/"                  { printf("Multi line comment\n");/* ignore multi-line comments */ }

{charDataMissingQuote}                      { /*Did not find closing quote for charData*/ printf("Missing quotes: %s\n" , yytext); }
{stringDataMissingQuote}                    { /*Did not find closing quote for stringData*/ printf("Missing quotes: %s\n" , yytext); }

.                                           { printf("Rule %-20s matched: '%s'\n", "UNKNOWN", yytext); /* I guess return UNKOWN for future error correction */ /*return UNKOWN;*/ }

%%

/* Subroutines */
int yywrap(void) 
{
    return 1;
}

// int main(int argc, char *argv[]) 
// {
//     if(argc > 1) 
//     {
//         yyin = fopen(argv[1], "r");
//         if(!yyin) 
//         {
//             perror("Cannot open file");
//             return 1;
//         }
//     }
//     yylex();
//     if (yyin) fclose(yyin);
//     return 0;
// }