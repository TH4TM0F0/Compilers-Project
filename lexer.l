/* Definitions */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <math.h>
    #include "parser.tab.h"
    #include "ErrorHandler.h"

    extern YYLTYPE yylloc;
    
    int lastValidLine = 1;
    int previousValidLine = 1;

    static int count_newlines(const char* s) {
        int c = 0;
        for (; *s; ++s) if (*s == '\n') c++;
        return c;
    }

    #define SET_LOC_TEXT(textptr) do {                 \
        int nl = count_newlines((textptr));            \
        yylloc.first_line = yylineno;                  \
        yylloc.last_line  = yylineno + nl;             \
    } while(0)

    #define RETURN_TOKEN(tok) do {                     \
        SET_LOC_TEXT(yytext);                          \
        previousValidLine = lastValidLine;             \
        lastValidLine = yylloc.first_line;             \
        return (tok);                                  \
    } while(0)

%}

%option yylineno noyywrap noinput nounput


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

"if" { 
    printf("Rule %-20s matched: '%s'\n", "IF", yytext); 
    RETURN_TOKEN(IF); 
}

"else" { 
    printf("Rule %-20s matched: '%s'\n", "ELSE", yytext); 
    RETURN_TOKEN(ELSE); 
}

"while" { 
    printf("Rule %-20s matched: '%s'\n", "WHILE", yytext); 
    RETURN_TOKEN(WHILE); 
}

"repeat" { 
    printf("Rule %-20s matched: '%s'\n", "REPEAT", yytext); 
    RETURN_TOKEN(REPEAT); 
}

"until" { 
    printf("Rule %-20s matched: '%s'\n", "UNTIL", yytext); 
    RETURN_TOKEN(UNTIL); 
}

"for" { 
    printf("Rule %-20s matched: '%s'\n", "FOR", yytext); 
    RETURN_TOKEN(FOR); 
}

"switch" { 
    printf("Rule %-20s matched: '%s'\n", "SWITCH", yytext); 
    RETURN_TOKEN(SWITCH); 
}

"case" { 
    printf("Rule %-20s matched: '%s'\n", "CASE", yytext); 
    RETURN_TOKEN(CASE); 
}

"const" { 
    printf("Rule %-20s matched: '%s'\n", "CONST", yytext); 
    RETURN_TOKEN(CONST); 
}

"return" { 
    printf("Rule %-20s matched: '%s'\n", "RETURN", yytext); 
    RETURN_TOKEN(RETURN); 
}

"continue" { 
    printf("Rule %-20s matched: '%s'\n", "CONTINUE", yytext); 
    RETURN_TOKEN(CONTINUE); 
}

"break" { 
    printf("Rule %-20s matched: '%s'\n", "BREAK", yytext); 
    RETURN_TOKEN(BREAK); 
}

"function" { 
    printf("Rule %-20s matched: '%s'\n", "FUNCTION", yytext); 
    RETURN_TOKEN(FUNCTION); 
}

"default" { 
    printf("Rule %-20s matched: '%s'\n", "DEFAULT", yytext); 
    RETURN_TOKEN(DEFAULT); 
}

"||" { 
    printf("Rule %-20s matched: '%s'\n", "OR", yytext); 
    RETURN_TOKEN(OR); 
}

"&&" { 
    printf("Rule %-20s matched: '%s'\n", "AND", yytext); 
    RETURN_TOKEN(AND); 
}

"!=" { 
    printf("Rule %-20s matched: '%s'\n", "NEQ", yytext); 
    RETURN_TOKEN(NEQ); 
}

"!" { 
    printf("Rule %-20s matched: '%s'\n", "NOT", yytext); 
    RETURN_TOKEN(NOT); 
}

">=" { 
    printf("Rule %-20s matched: '%s'\n", "GE", yytext); 
    RETURN_TOKEN(GE); 
}

"<=" { 
    printf("Rule %-20s matched: '%s'\n", "SE", yytext); 
    RETURN_TOKEN(SE); 
}

">" { 
    printf("Rule %-20s matched: '%s'\n", "GT", yytext); 
    RETURN_TOKEN(GT); 
}

"<" { 
    printf("Rule %-20s matched: '%s'\n", "ST", yytext); 
    RETURN_TOKEN(ST); 
}

"==" { 
    printf("Rule %-20s matched: '%s'\n", "EQ", yytext); 
    RETURN_TOKEN(EQ); 
}

"++" { 
    printf("Rule %-20s matched: '%s'\n", "INCREMENT", yytext); 
    RETURN_TOKEN(INCREMENT); 
}

"--" { 
    printf("Rule %-20s matched: '%s'\n", "DECREMENT", yytext); 
    RETURN_TOKEN(DECREMENT); 
}

"**" { 
    printf("Rule %-20s matched: '%s'\n", "POWER", yytext); 
    RETURN_TOKEN(POWER); 
}

"+" { 
    printf("Rule %-20s matched: '%s'\n", "PLUS", yytext); 
    RETURN_TOKEN(PLUS); 
}

"-" { 
    printf("Rule %-20s matched: '%s'\n", "MINUS", yytext); 
    RETURN_TOKEN(MINUS); 
}

"*" { 
    printf("Rule %-20s matched: '%s'\n", "MULTIPLY", yytext); 
    RETURN_TOKEN(MULTIPLY); 
}

"/" { 
    printf("Rule %-20s matched: '%s'\n", "DIVIDE", yytext); 
    RETURN_TOKEN(DIVIDE); 
}

"%" { 
    printf("Rule %-20s matched: '%s'\n", "MODULO", yytext); 
    RETURN_TOKEN(MODULO); 
}

"=" { 
    printf("Rule %-20s matched: '%s'\n", "EQUAL", yytext); 
    RETURN_TOKEN(EQUAL); 
}

";" { 
    printf("Rule %-20s matched: '%s'\n", "SEMI_COLON", yytext); 
    RETURN_TOKEN(SEMI_COLON); 
}

"," { 
    printf("Rule %-20s matched: '%s'\n", "COMMA", yytext); 
    RETURN_TOKEN(COMMA); 
}

":" { 
    printf("Rule %-20s matched: '%s'\n", "COLON", yytext); 
    RETURN_TOKEN(COLON); 
}

"(" { 
    printf("Rule %-20s matched: '%s'\n", "LEFT_ROUND_BRACKET", yytext); 
    RETURN_TOKEN(LEFT_ROUND_BRACKET); 
}

")" { 
    printf("Rule %-20s matched: '%s'\n", "RIGHT_ROUND_BRACKET", yytext); 
    RETURN_TOKEN(RIGHT_ROUND_BRACKET); 
}

"{" { 
    printf("Rule %-20s matched: '%s'\n", "LEFT_CURLY_BRACKET", yytext); 
    RETURN_TOKEN(LEFT_CURLY_BRACKET); 
}

"}" { 
    printf("Rule %-20s matched: '%s'\n", "RIGHT_CURLY_BRACKET", yytext); 
    RETURN_TOKEN(RIGHT_CURLY_BRACKET); 
}

"int"|"float"|"char"|"string"|"bool"|"void" { 
    printf("Rule %-20s matched: '%s'\n", "DATATYPE", yytext); 
    yylval.stringData = strdup(yytext); 
    RETURN_TOKEN(DATATYPE);  
}

"true" { 
    printf("Rule %-20s matched: '%s'\n", "BOOLVALUE", yytext); 
    yylval.intData = 1; 
    RETURN_TOKEN(BOOLVALUE); 
}

"false" { 
    printf("Rule %-20s matched: '%s'\n", "BOOLVALUE", yytext); 
    yylval.intData = 0; RETURN_TOKEN(BOOLVALUE); 
}

{intData} { 
    printf("Rule %-20s matched: '%s'\n", "INTVALUE", yytext); 
    yylval.intData = atoi(yytext); 
    RETURN_TOKEN(INTVALUE); 
}

{floatData} { 
    printf("Rule %-20s matched: '%s'\n", "FLOATVALUE", yytext); 
    yylval.floatData = (float)atof(yytext); 
    RETURN_TOKEN(FLOATVALUE); 
}

{charData} {
    SET_LOC_TEXT(yytext);
    char out = '\0';
    if (yytext[1] != '\\') {
        out = yytext[1];
    } 
    else {
        char esc = yytext[2];
        switch (esc) {
            case 'n':  out = '\n'; break;
            case 't':  out = '\t'; break;
            case 'r':  out = '\r'; break;
            case 'b':  out = '\b'; break;
            case '\\': out = '\\'; break;
            case '\'': out = '\''; break;
            case '"':  out = '\"'; break;
            case '0':  out = '\0'; break;
            default:
                reportError(SYNTAX_ERROR, "Unknown escape sequence in character literal", yylloc.first_line);
                return UNKNOWN;
        }
    }
    yylval.charData = out;
    previousValidLine = lastValidLine;
    lastValidLine = yylloc.first_line;
    return CHARVALUE;
}

{stringData} { 
    SET_LOC_TEXT(yytext);
    printf("Rule %-20s matched: '%s'\n", "STRINGVALUE", yytext); 
    int n = (int)strlen(yytext); 
    char* s = (char*)malloc(n - 1); 
    memcpy(s, yytext + 1, n - 2); 
    s[n - 2] = '\0'; 
    yylval.stringData = s;
    previousValidLine = lastValidLine;
    lastValidLine = yylloc.first_line; 
    return STRINGVALUE; 
}

{identifier} { 
    printf("Rule %-20s matched: '%s'\n", "IDENTIFIER", yytext); 
    yylval.stringData = strdup(yytext); 
    RETURN_TOKEN(IDENTIFIER); 
}

[ \t\r]+ { 
    SET_LOC_TEXT(yytext);
}

\n+ { 
    SET_LOC_TEXT(yytext);
}

"//".* { 
    printf("Single line comment\n"); 
}

"/*"([^*]|\*+[^*/])*\*+"/" { 
    printf("Multi line comment\n"); 
}

{charDataMissingQuote} {
    SET_LOC_TEXT(yytext); 
    printf("Missing quotes: %s\n" , yytext); 
    reportError(SYNTAX_ERROR, "Unterminated Character Literal", yylloc.first_line);
    previousValidLine = lastValidLine;
    lastValidLine = yylloc.first_line;
    return UNKNOWN;
}

{stringDataMissingQuote} {
    SET_LOC_TEXT(yytext); 
    printf("Missing quotes: %s\n" , yytext); 
    reportError(SYNTAX_ERROR, "Unterminated String Literal", yylloc.first_line);
    previousValidLine = lastValidLine;
    lastValidLine = yylloc.first_line;
    return UNKNOWN;
}

. { 
    SET_LOC_TEXT(yytext);
    printf("Rule %-20s matched: '%s'\n", "UNKNOWN", yytext); 
    reportError(SYNTAX_ERROR, "Unrecognized Character", yylloc.first_line); 
    previousValidLine = lastValidLine;
    lastValidLine = yylloc.first_line;
    return UNKNOWN; 
}

%%