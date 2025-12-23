/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 282 "parser.y"

    #include "Utils.h"
    #include "SymbolTable.h"
    #include "Parameter.h"
    typedef struct Expression Expression;
    typedef struct DeclNode DeclNode;
    typedef struct ArgNode ArgNode;

#line 58 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    ELSE = 259,                    /* ELSE  */
    WHILE = 260,                   /* WHILE  */
    REPEAT = 261,                  /* REPEAT  */
    UNTIL = 262,                   /* UNTIL  */
    FOR = 263,                     /* FOR  */
    SWITCH = 264,                  /* SWITCH  */
    CASE = 265,                    /* CASE  */
    CONST = 266,                   /* CONST  */
    RETURN = 267,                  /* RETURN  */
    CONTINUE = 268,                /* CONTINUE  */
    BREAK = 269,                   /* BREAK  */
    DEFAULT = 270,                 /* DEFAULT  */
    FUNCTION = 271,                /* FUNCTION  */
    OR = 272,                      /* OR  */
    AND = 273,                     /* AND  */
    NOT = 274,                     /* NOT  */
    GT = 275,                      /* GT  */
    GE = 276,                      /* GE  */
    ST = 277,                      /* ST  */
    SE = 278,                      /* SE  */
    EQ = 279,                      /* EQ  */
    NEQ = 280,                     /* NEQ  */
    PLUS = 281,                    /* PLUS  */
    MINUS = 282,                   /* MINUS  */
    MULTIPLY = 283,                /* MULTIPLY  */
    DIVIDE = 284,                  /* DIVIDE  */
    MODULO = 285,                  /* MODULO  */
    POWER = 286,                   /* POWER  */
    INCREMENT = 287,               /* INCREMENT  */
    DECREMENT = 288,               /* DECREMENT  */
    EQUAL = 289,                   /* EQUAL  */
    SEMI_COLON = 290,              /* SEMI_COLON  */
    COMMA = 291,                   /* COMMA  */
    COLON = 292,                   /* COLON  */
    LEFT_ROUND_BRACKET = 293,      /* LEFT_ROUND_BRACKET  */
    RIGHT_ROUND_BRACKET = 294,     /* RIGHT_ROUND_BRACKET  */
    LEFT_CURLY_BRACKET = 295,      /* LEFT_CURLY_BRACKET  */
    RIGHT_CURLY_BRACKET = 296,     /* RIGHT_CURLY_BRACKET  */
    INTVALUE = 297,                /* INTVALUE  */
    BOOLVALUE = 298,               /* BOOLVALUE  */
    FLOATVALUE = 299,              /* FLOATVALUE  */
    CHARVALUE = 300,               /* CHARVALUE  */
    STRINGVALUE = 301,             /* STRINGVALUE  */
    IDENTIFIER = 302,              /* IDENTIFIER  */
    DATATYPE = 303,                /* DATATYPE  */
    UNKNOWN = 304,                 /* UNKNOWN  */
    LOWER_THAN_ELSE = 305,         /* LOWER_THAN_ELSE  */
    UMINUS = 306                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 293 "parser.y"

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

#line 149 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
