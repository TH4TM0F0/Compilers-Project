/* Definitions */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <math.h>
    #include "parser.tab.h"

    // extern int yylineno;
    int lastValidLine = 1;
    int previousValidLine = 1;
%}

%option yylineno noyywrap

/* Regex constructs for easier formations */
/***************************************/

/* Helpers */
digit [0-9]
varNameStart [a-zA-Z_]
acceptableNamingChar [a-zA-Z0-9_]

/* Values that can be assigned to int data type */
intData {digit}+

/* Values that can be assigned to float data type */
floatData ({digit}+"."{digit}*)|({digit}*"."{digit}+)

/* Values that can be assigned to char data type */
charData \'([^'\\]|\\[bnrt\\'"])\'

/* Char with no closing quotes stops matching till it gets a char with nextchar ; I want to add comma and right round bracket */
/* print('a); */
charDataMissingQuote \'([^'\\;]|\\[bnrt\\'"])

/* Values that can be assigned to string data type */
stringData \"([^"\\]|\\.)*\"

/* String with no closing quotes stops matching till it gets a char with nextchar ; I want to add comma and right round bracket */
/* print("Mazen); */
stringDataMissingQuote \"([^"\\;]|\\.)*

/* Identifiers (funcNames or varNames) */
identifier {varNameStart}{acceptableNamingChar}*

/* Regex Rules */
%%

"if"                                        { printf("Rule %-20s matched: '%s'\n", "IF", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return IF; }
"else"                                      { printf("Rule %-20s matched: '%s'\n", "ELSE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return ELSE; }
"while"                                     { printf("Rule %-20s matched: '%s'\n", "WHILE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return WHILE; }
"repeat"                                    { printf("Rule %-20s matched: '%s'\n", "REPEAT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return REPEAT; }
"until"                                     { printf("Rule %-20s matched: '%s'\n", "UNTIL", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return UNTIL; }
"for"                                       { printf("Rule %-20s matched: '%s'\n", "FOR", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return FOR; }
"switch"                                    { printf("Rule %-20s matched: '%s'\n", "SWITCH", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return SWITCH; }
"case"                                      { printf("Rule %-20s matched: '%s'\n", "CASE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return CASE; }
"const"                                     { printf("Rule %-20s matched: '%s'\n", "CONST", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return CONST; }
"return"                                    { printf("Rule %-20s matched: '%s'\n", "RETURN", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return RETURN; }
"continue"                                  { printf("Rule %-20s matched: '%s'\n", "CONTINUE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return CONTINUE; }
"break"                                     { printf("Rule %-20s matched: '%s'\n", "BREAK", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return BREAK; }
"function"                                  { printf("Rule %-20s matched: '%s'\n", "FUNCTION", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return FUNCTION; }
"default"                                   { printf("Rule %-20s matched: '%s'\n", "DEFAULT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return DEFAULT; }

"||"                                        { printf("Rule %-20s matched: '%s'\n", "OR", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return OR; }
"&&"                                        { printf("Rule %-20s matched: '%s'\n", "AND", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return AND; }
"!"                                         { printf("Rule %-20s matched: '%s'\n", "NOT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return NOT; }
">"                                         { printf("Rule %-20s matched: '%s'\n", "GT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return GT; }
">="                                        { printf("Rule %-20s matched: '%s'\n", "GE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return GE; }
"<"                                         { printf("Rule %-20s matched: '%s'\n", "ST", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return ST; }
"<="                                        { printf("Rule %-20s matched: '%s'\n", "SE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return SE; }
"=="                                        { printf("Rule %-20s matched: '%s'\n", "EQ", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return EQ; }
"!="                                        { printf("Rule %-20s matched: '%s'\n", "NEQ", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return NEQ; }

"+"                                         { printf("Rule %-20s matched: '%s'\n", "PLUS", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return PLUS; }
"-"                                         { printf("Rule %-20s matched: '%s'\n", "MINUS", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return MINUS; }
"**"                                        { printf("Rule %-20s matched: '%s'\n", "POWER", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return POWER; }
"*"                                         { printf("Rule %-20s matched: '%s'\n", "MULTIPLY", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return MULTIPLY; }
"/"                                         { printf("Rule %-20s matched: '%s'\n", "DIVIDE", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return DIVIDE; }
"%"                                         { printf("Rule %-20s matched: '%s'\n", "MODULO", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return MODULO; }
"++"                                        { printf("Rule %-20s matched: '%s'\n", "INCREMENT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return INCREMENT; }
"--"                                        { printf("Rule %-20s matched: '%s'\n", "DECREMENT", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return DECREMENT; }
"="                                         { printf("Rule %-20s matched: '%s'\n", "EQUAL", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return EQUAL; }

";"                                         { printf("Rule %-20s matched: '%s'\n", "SEMI_COLON", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return SEMI_COLON; }
","                                         { printf("Rule %-20s matched: '%s'\n", "COMMA", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return COMMA; }
":"                                         { printf("Rule %-20s matched: '%s'\n", "COLON", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return COLON; }
"("                                         { printf("Rule %-20s matched: '%s'\n", "LEFT_ROUND_BRACKET", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return LEFT_ROUND_BRACKET; }
")"                                         { printf("Rule %-20s matched: '%s'\n", "RIGHT_ROUND_BRACKET", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return RIGHT_ROUND_BRACKET; }
"{"                                         { printf("Rule %-20s matched: '%s'\n", "LEFT_CURLY_BRACKET", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return LEFT_CURLY_BRACKET; }
"}"                                         { printf("Rule %-20s matched: '%s'\n", "RIGHT_CURLY_BRACKET", yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return RIGHT_CURLY_BRACKET; }

"int"|"float"|"char"|"string"|"bool"|"void" { printf("Rule %-20s matched: '%s'\n", "DATATYPE", yytext); yylval.stringData = strdup(yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return DATATYPE;  }

"true"                                      { printf("Rule %-20s matched: '%s'\n", "BOOLVALUE", yytext); yylval.boolData = 1; previousValidLine = lastValidLine; lastValidLine = yylineno; return BOOLVALUE; }
"false"                                     { printf("Rule %-20s matched: '%s'\n", "BOOLVALUE", yytext); yylval.boolData = 0; previousValidLine = lastValidLine; lastValidLine = yylineno; return BOOLVALUE; }
{intData}                                   { printf("Rule %-20s matched: '%s'\n", "INTVALUE", yytext); yylval.intData = atoi(yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return INTVALUE; }
{floatData}                                 { printf("Rule %-20s matched: '%s'\n", "FLOATVALUE", yytext); yylval.floatData = (float)atof(yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return FLOATVALUE; }
{charData}                                  { printf("Rule %-20s matched: '%s'\n", "CHARVALUE", yytext); yylval.charData = yytext[1]; previousValidLine = lastValidLine; lastValidLine = yylineno; return CHARVALUE; }
{stringData}                                { printf("Rule %-20s matched: '%s'\n", "STRINGVALUE", yytext); yylval.stringData = strdup(yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return STRINGVALUE; }

{identifier}                                { printf("Rule %-20s matched: '%s'\n", "IDENTIFIER", yytext); yylval.stringData = strdup(yytext); previousValidLine = lastValidLine; lastValidLine = yylineno; return IDENTIFIER; }

[ \t\r]+                                    { }
\n                                          { }
"//".*                                      { printf("Single line comment\n"); }
"/*"([^*]|\*+[^*/])*\*+"/"                  { printf("Multi line comment\n"); }

{charDataMissingQuote}                      { printf("Missing quotes: %s\n" , yytext); }
{stringDataMissingQuote}                    { printf("Missing quotes: %s\n" , yytext); }

.                                           { printf("Rule %-20s matched: '%s'\n", "UNKNOWN", yytext); return UNKNOWN; }

%%