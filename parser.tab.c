/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "parser.y"

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
                
        value outv;
        memset(&outv, 0, sizeof(outv));

        if (e->temp_var == NULL) /* literal or known constant */
        {
            if (target == INT_TYPE)
            {
                if (e->expressionType == FLOAT_TYPE) outv.INT_Data = (int)e->expressionValue.FLOAT_Data;
                else if (e->expressionType == BOOL_TYPE) outv.INT_Data = e->expressionValue.BOOL_Data ? 1 : 0;
                else outv.INT_Data = e->expressionValue.INT_Data;
            }
            else if (target == FLOAT_TYPE)
            {
                if (e->expressionType == INT_TYPE) outv.FLOAT_Data = (float)e->expressionValue.INT_Data;
                else if (e->expressionType == BOOL_TYPE) outv.FLOAT_Data = e->expressionValue.BOOL_Data ? 1.0f : 0.0f;
                else outv.FLOAT_Data = e->expressionValue.FLOAT_Data;
            }
            else if (target == BOOL_TYPE)
            {
                if (e->expressionType == INT_TYPE) outv.BOOL_Data = (e->expressionValue.INT_Data != 0);
                else if (e->expressionType == FLOAT_TYPE) outv.BOOL_Data = (e->expressionValue.FLOAT_Data != 0.0f);
                else outv.BOOL_Data = e->expressionValue.BOOL_Data;
            }
        }

                Expression* out = makeExpr(target, outv, t);
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

    static bool comingFromForLoopHeader = false;


#line 348 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IF = 3,                         /* IF  */
  YYSYMBOL_ELSE = 4,                       /* ELSE  */
  YYSYMBOL_WHILE = 5,                      /* WHILE  */
  YYSYMBOL_REPEAT = 6,                     /* REPEAT  */
  YYSYMBOL_UNTIL = 7,                      /* UNTIL  */
  YYSYMBOL_FOR = 8,                        /* FOR  */
  YYSYMBOL_SWITCH = 9,                     /* SWITCH  */
  YYSYMBOL_CASE = 10,                      /* CASE  */
  YYSYMBOL_CONST = 11,                     /* CONST  */
  YYSYMBOL_RETURN = 12,                    /* RETURN  */
  YYSYMBOL_CONTINUE = 13,                  /* CONTINUE  */
  YYSYMBOL_BREAK = 14,                     /* BREAK  */
  YYSYMBOL_DEFAULT = 15,                   /* DEFAULT  */
  YYSYMBOL_FUNCTION = 16,                  /* FUNCTION  */
  YYSYMBOL_OR = 17,                        /* OR  */
  YYSYMBOL_AND = 18,                       /* AND  */
  YYSYMBOL_NOT = 19,                       /* NOT  */
  YYSYMBOL_GT = 20,                        /* GT  */
  YYSYMBOL_GE = 21,                        /* GE  */
  YYSYMBOL_ST = 22,                        /* ST  */
  YYSYMBOL_SE = 23,                        /* SE  */
  YYSYMBOL_EQ = 24,                        /* EQ  */
  YYSYMBOL_NEQ = 25,                       /* NEQ  */
  YYSYMBOL_PLUS = 26,                      /* PLUS  */
  YYSYMBOL_MINUS = 27,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 28,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 29,                    /* DIVIDE  */
  YYSYMBOL_MODULO = 30,                    /* MODULO  */
  YYSYMBOL_POWER = 31,                     /* POWER  */
  YYSYMBOL_INCREMENT = 32,                 /* INCREMENT  */
  YYSYMBOL_DECREMENT = 33,                 /* DECREMENT  */
  YYSYMBOL_EQUAL = 34,                     /* EQUAL  */
  YYSYMBOL_SEMI_COLON = 35,                /* SEMI_COLON  */
  YYSYMBOL_COMMA = 36,                     /* COMMA  */
  YYSYMBOL_COLON = 37,                     /* COLON  */
  YYSYMBOL_LEFT_ROUND_BRACKET = 38,        /* LEFT_ROUND_BRACKET  */
  YYSYMBOL_RIGHT_ROUND_BRACKET = 39,       /* RIGHT_ROUND_BRACKET  */
  YYSYMBOL_LEFT_CURLY_BRACKET = 40,        /* LEFT_CURLY_BRACKET  */
  YYSYMBOL_RIGHT_CURLY_BRACKET = 41,       /* RIGHT_CURLY_BRACKET  */
  YYSYMBOL_INTVALUE = 42,                  /* INTVALUE  */
  YYSYMBOL_BOOLVALUE = 43,                 /* BOOLVALUE  */
  YYSYMBOL_FLOATVALUE = 44,                /* FLOATVALUE  */
  YYSYMBOL_CHARVALUE = 45,                 /* CHARVALUE  */
  YYSYMBOL_STRINGVALUE = 46,               /* STRINGVALUE  */
  YYSYMBOL_IDENTIFIER = 47,                /* IDENTIFIER  */
  YYSYMBOL_DATATYPE = 48,                  /* DATATYPE  */
  YYSYMBOL_UNKNOWN = 49,                   /* UNKNOWN  */
  YYSYMBOL_LOWER_THAN_ELSE = 50,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_UMINUS = 51,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 52,                  /* $accept  */
  YYSYMBOL_PROGRAM = 53,                   /* PROGRAM  */
  YYSYMBOL_STATEMENTS = 54,                /* STATEMENTS  */
  YYSYMBOL_STATEMENT = 55,                 /* STATEMENT  */
  YYSYMBOL_BLOCK = 56,                     /* BLOCK  */
  YYSYMBOL_57_1 = 57,                      /* $@1  */
  YYSYMBOL_DECLARATION = 58,               /* DECLARATION  */
  YYSYMBOL_IDENTIFIERS = 59,               /* IDENTIFIERS  */
  YYSYMBOL_ASSIGNMENT = 60,                /* ASSIGNMENT  */
  YYSYMBOL_FUNCTION_CALL = 61,             /* FUNCTION_CALL  */
  YYSYMBOL_IF_STATEMENT = 62,              /* IF_STATEMENT  */
  YYSYMBOL_63_2 = 63,                      /* $@2  */
  YYSYMBOL_IF_HEAD = 64,                   /* IF_HEAD  */
  YYSYMBOL_WHILE_STATEMENT = 65,           /* WHILE_STATEMENT  */
  YYSYMBOL_WHILE_HEAD = 66,                /* WHILE_HEAD  */
  YYSYMBOL_REPEAT_STATEMENT = 67,          /* REPEAT_STATEMENT  */
  YYSYMBOL_REPEAT_HEAD = 68,               /* REPEAT_HEAD  */
  YYSYMBOL_FOR_STATEMENT = 69,             /* FOR_STATEMENT  */
  YYSYMBOL_FOR_HEADER = 70,                /* FOR_HEADER  */
  YYSYMBOL_ITERATOR = 71,                  /* ITERATOR  */
  YYSYMBOL_SWITCH_STATEMENT = 72,          /* SWITCH_STATEMENT  */
  YYSYMBOL_73_3 = 73,                      /* $@3  */
  YYSYMBOL_74_4 = 74,                      /* $@4  */
  YYSYMBOL_75_5 = 75,                      /* $@5  */
  YYSYMBOL_DEFAULT_CASE_OPT = 76,          /* DEFAULT_CASE_OPT  */
  YYSYMBOL_CASES = 77,                     /* CASES  */
  YYSYMBOL_SINGLE_CASE = 78,               /* SINGLE_CASE  */
  YYSYMBOL_79_6 = 79,                      /* $@6  */
  YYSYMBOL_DEFAULT_CASE = 80,              /* DEFAULT_CASE  */
  YYSYMBOL_81_7 = 81,                      /* $@7  */
  YYSYMBOL_PRIMARY_CASE = 82,              /* PRIMARY_CASE  */
  YYSYMBOL_RETURN_STATEMENT = 83,          /* RETURN_STATEMENT  */
  YYSYMBOL_FUNCTION_DEFINITION_IMPLEMENTATION = 84, /* FUNCTION_DEFINITION_IMPLEMENTATION  */
  YYSYMBOL_85_8 = 85,                      /* $@8  */
  YYSYMBOL_PARAMETER_LIST = 86,            /* PARAMETER_LIST  */
  YYSYMBOL_PARAM_LIST_NONEMPTY = 87,       /* PARAM_LIST_NONEMPTY  */
  YYSYMBOL_ARGUMENT_LIST = 88,             /* ARGUMENT_LIST  */
  YYSYMBOL_ARGUMENTS = 89,                 /* ARGUMENTS  */
  YYSYMBOL_LOGICAL_EXPRESSION = 90,        /* LOGICAL_EXPRESSION  */
  YYSYMBOL_LOGICAL_TERM = 91,              /* LOGICAL_TERM  */
  YYSYMBOL_EQUALITY_EXPRESSION = 92,       /* EQUALITY_EXPRESSION  */
  YYSYMBOL_RELATIONAL_EXPRESSION = 93,     /* RELATIONAL_EXPRESSION  */
  YYSYMBOL_ADDITIVE_EXPRESSION = 94,       /* ADDITIVE_EXPRESSION  */
  YYSYMBOL_MULTIPLICATIVE_EXPRESSION = 95, /* MULTIPLICATIVE_EXPRESSION  */
  YYSYMBOL_EXPONENT_EXPRESSION = 96,       /* EXPONENT_EXPRESSION  */
  YYSYMBOL_UNARY_EXPRESSION = 97,          /* UNARY_EXPRESSION  */
  YYSYMBOL_PRIMARY_EXPRESSION = 98,        /* PRIMARY_EXPRESSION  */
  YYSYMBOL_PRIMARY_SUFFIX = 99             /* PRIMARY_SUFFIX  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   980

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  48
/* YYNRULES -- Number of rules.  */
#define YYNRULES  199
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  306

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   353,   353,   357,   358,   362,   363,   364,   365,   366,
     371,   376,   377,   378,   379,   380,   381,   382,   385,   389,
     393,   397,   401,   405,   409,   416,   416,   443,   504,   538,
     543,   548,   553,   561,   562,   563,   564,   567,   572,   577,
     582,   590,   623,   640,   657,   674,   699,   703,   707,   714,
     787,   791,   798,   804,   804,   816,   823,   832,   836,   840,
     844,   851,   861,   868,   879,   883,   887,   891,   898,   910,
     914,   918,   922,   926,   930,   934,   941,   950,   961,   968,
     982,   986,   990,   994,   998,  1002,  1006,  1013,  1027,  1048,
    1052,  1056,  1060,  1067,  1078,  1078,  1067,  1092,  1099,  1100,
    1104,  1105,  1109,  1109,  1130,  1134,  1141,  1141,  1148,  1155,
    1161,  1167,  1173,  1183,  1189,  1198,  1204,  1218,  1218,  1262,
    1266,  1270,  1274,  1281,  1282,  1286,  1289,  1294,  1299,  1304,
    1312,  1313,  1317,  1318,  1321,  1326,  1334,  1344,  1347,  1351,
    1358,  1368,  1371,  1375,  1382,  1392,  1402,  1405,  1409,  1413,
    1417,  1424,  1434,  1444,  1454,  1464,  1467,  1471,  1475,  1479,
    1483,  1487,  1491,  1495,  1502,  1513,  1524,  1527,  1531,  1535,
    1542,  1553,  1564,  1574,  1577,  1581,  1585,  1589,  1593,  1597,
    1604,  1617,  1620,  1624,  1631,  1639,  1647,  1650,  1654,  1661,
    1667,  1673,  1679,  1685,  1691,  1694,  1742,  1747,  1755,  1756
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IF", "ELSE", "WHILE",
  "REPEAT", "UNTIL", "FOR", "SWITCH", "CASE", "CONST", "RETURN",
  "CONTINUE", "BREAK", "DEFAULT", "FUNCTION", "OR", "AND", "NOT", "GT",
  "GE", "ST", "SE", "EQ", "NEQ", "PLUS", "MINUS", "MULTIPLY", "DIVIDE",
  "MODULO", "POWER", "INCREMENT", "DECREMENT", "EQUAL", "SEMI_COLON",
  "COMMA", "COLON", "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "INTVALUE", "BOOLVALUE",
  "FLOATVALUE", "CHARVALUE", "STRINGVALUE", "IDENTIFIER", "DATATYPE",
  "UNKNOWN", "LOWER_THAN_ELSE", "UMINUS", "$accept", "PROGRAM",
  "STATEMENTS", "STATEMENT", "BLOCK", "$@1", "DECLARATION", "IDENTIFIERS",
  "ASSIGNMENT", "FUNCTION_CALL", "IF_STATEMENT", "$@2", "IF_HEAD",
  "WHILE_STATEMENT", "WHILE_HEAD", "REPEAT_STATEMENT", "REPEAT_HEAD",
  "FOR_STATEMENT", "FOR_HEADER", "ITERATOR", "SWITCH_STATEMENT", "$@3",
  "$@4", "$@5", "DEFAULT_CASE_OPT", "CASES", "SINGLE_CASE", "$@6",
  "DEFAULT_CASE", "$@7", "PRIMARY_CASE", "RETURN_STATEMENT",
  "FUNCTION_DEFINITION_IMPLEMENTATION", "$@8", "PARAMETER_LIST",
  "PARAM_LIST_NONEMPTY", "ARGUMENT_LIST", "ARGUMENTS",
  "LOGICAL_EXPRESSION", "LOGICAL_TERM", "EQUALITY_EXPRESSION",
  "RELATIONAL_EXPRESSION", "ADDITIVE_EXPRESSION",
  "MULTIPLICATIVE_EXPRESSION", "EXPONENT_EXPRESSION", "UNARY_EXPRESSION",
  "PRIMARY_EXPRESSION", "PRIMARY_SUFFIX", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-86)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-103)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -86,    38,   870,   -86,    41,   178,   -86,   182,   200,     8,
     933,   266,   294,    13,    24,    31,     7,   232,    23,   -86,
     -86,   295,   297,   298,   -86,    25,   -86,    40,   -86,    65,
     -86,    99,   -86,   325,   -86,   -86,    66,   -86,   100,   -86,
      16,   -86,   933,   -86,   105,    29,    45,    30,   114,   149,
     153,   161,   166,   170,   175,   113,   214,   217,   296,   327,
     167,   -86,   -86,   -86,   -86,   -86,   352,   317,   374,   278,
     202,   332,   207,   363,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,    34,   -86,   -86,   -86,   -86,   -86,   -86,   902,   -86,
     -86,   231,   902,   -86,   -86,   422,   391,   390,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   423,   -86,   -86,   -86,
      36,   -86,   -86,   -86,   -86,   411,   160,   412,   163,   -86,
     418,    51,   326,    19,   -86,   390,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   414,   164,   902,   -86,   479,   510,   541,
     572,   603,   625,   656,   678,   709,   730,   761,   782,   792,
     813,   -86,   230,   858,   453,   419,   420,   317,   -86,   317,
     134,   -86,   262,    52,   477,   -86,   233,   -86,   -86,   -86,
     -86,   -86,   -86,   293,   -86,   238,   -86,   324,   -86,   -86,
     -86,   -86,   443,   -86,   374,   -86,   278,   -86,   202,   -86,
     202,   -86,   332,   -86,   332,   -86,   332,   -86,   332,   -86,
     207,   -86,   207,   -86,   363,   -86,   363,   -86,   363,   -86,
     -86,   -86,   409,   -86,   -86,   -86,   355,   -86,   -86,   -86,
     317,   -86,   449,   444,   -86,   386,   -86,   317,   -86,   417,
     -86,    27,   445,   -86,   102,   136,   451,   -86,   317,   448,
     -86,   450,   165,   -86,   317,   -86,   101,   -86,   -86,   -86,
     -86,   -86,    21,   -86,   317,   -86,   -86,   356,   -86,   195,
     168,   -86,   444,   -86,   106,   -86,   -86,   -86,   -86,    35,
     -86,   -86,   -86,   199,   135,   447,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   169,   -86,   -86,   -86,   -86,
     459,   -86,   -86,   -86,   870,   870
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,    76,     0,     0,     0,
     115,     0,     0,     0,     0,     0,     0,     0,     0,     4,
      11,     0,     0,     0,    12,     0,    13,     0,    15,     0,
      14,     0,    16,     0,    17,    57,     0,    64,     0,    80,
       0,    97,     0,    30,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   189,   192,   190,   191,   193,   198,   116,   137,   141,
     146,   155,   166,   173,   181,   186,    23,    10,    22,     9,
     119,     0,    47,    44,    48,    45,    24,     3,   130,    42,
      43,     0,   130,    29,    32,     0,    33,    27,    18,     5,
      19,     6,    20,     7,    55,    25,    52,    62,    61,    69,
       0,    78,    77,    21,     8,    58,     0,    65,     0,    81,
       0,     0,     0,     0,    31,    28,   139,   143,   187,   185,
     161,   163,   160,   162,   149,   150,   169,   188,   184,   177,
     178,   179,   183,     0,     0,   130,   195,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   120,     0,     0,     0,     0,   131,   132,    46,    41,
       0,    40,     0,     0,     0,    70,     0,    60,    59,    56,
      67,    66,    63,     0,    90,     0,    82,     0,    93,   196,
     197,   194,     0,   138,   136,   142,   140,   147,   144,   148,
     145,   157,   152,   159,   154,   156,   151,   158,   153,   167,
     164,   168,   165,   174,   170,   175,   171,   176,   172,   182,
     180,   121,   123,    26,   135,    51,     0,    50,    49,    37,
      34,    38,    35,     0,    71,     0,    89,    87,    91,     0,
      83,     0,     0,   199,     0,     0,   124,   134,   133,     0,
      54,    72,     0,    92,    88,    84,     0,    94,   127,   125,
     122,   117,     0,    39,    36,    74,    73,     0,    85,     0,
       0,   100,     0,   128,     0,    75,    68,    86,    79,    98,
     118,   129,   126,     0,     0,     0,   101,    99,   104,   109,
     111,   113,   110,   114,   112,     0,   108,   106,    95,   105,
       0,     3,    96,     3,   107,   103
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -86,   -86,   -82,   -86,   -25,   -86,   -86,   468,   257,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -85,   -86,    -9,   367,
     368,   239,   268,   240,   206,   -44,   -86,   -86
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    19,    20,    87,    21,    97,    22,    23,
      24,   174,    25,    26,    27,    28,    29,    30,    31,   122,
      32,   242,   271,   302,   285,   279,   286,   300,   287,   301,
     295,    33,    34,   272,   245,   246,   165,   166,   167,    68,
      69,    70,    71,    72,    73,    74,    75,   146
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     106,    67,   108,   129,   110,   163,   112,   170,    86,    43,
     -25,   138,   -25,   -25,    80,   -25,   -25,   119,   -25,   -25,
     -25,   -25,   273,   -25,    93,    82,   104,   116,   255,   118,
     126,   128,    84,   123,    94,   161,   147,   175,     3,   -25,
     -25,   107,    35,   176,   147,   283,   127,   -25,   -25,    47,
     284,   144,   184,   231,   -25,   -25,    44,    55,   188,    95,
     192,    81,   256,   120,   121,   105,   109,   115,    60,   274,
      96,    83,    61,    62,    63,    64,    65,    66,    85,    36,
     105,   162,   169,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   185,   232,
     111,   117,   268,   258,    60,   105,   124,   281,    61,    62,
      63,    64,    65,    66,   137,   130,   220,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    47,    14,    15,   227,   296,   260,    60,   105,
      55,    95,    61,    62,    63,    64,    65,    66,   269,   259,
     131,    60,    96,   282,   132,    61,    62,    63,    64,    65,
      66,   178,   133,   230,   181,   190,   266,   134,   143,   277,
     299,   135,   297,   228,   237,   261,   136,   147,   241,    37,
     147,   147,   147,    39,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,   179,
     288,    41,   182,   191,   267,    60,  -102,   278,   250,    61,
      62,    63,    64,    65,    66,   139,    38,   248,   140,   304,
      40,   305,   151,   152,   153,   154,   252,    89,    90,    91,
     254,   221,   168,    88,   234,   157,   158,   159,    42,   238,
     264,   289,   290,   291,   292,   293,   294,   280,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,   229,    89,    90,    91,    76,   222,    60,
      92,   235,   239,    61,    62,    63,    64,    65,    66,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,   236,    78,    98,   141,   100,   102,
      60,    77,   149,   150,    61,    62,    63,    64,    65,    66,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,   240,   113,   186,   142,    79,
      99,    60,   101,   103,   147,    61,    62,    63,    64,    65,
      66,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,   247,   275,   155,   156,
     114,   187,    60,   214,   216,   218,    61,    62,    63,    64,
      65,    66,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,   251,   198,   200,
     145,   276,   148,    60,   160,   210,   212,    61,    62,    63,
      64,    65,    66,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   253,   202,
     204,   206,   208,   171,    60,   172,   173,   -53,    61,    62,
      63,    64,    65,    66,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,   263,
     177,   180,   183,   189,   224,    60,   226,   244,   225,    61,
      62,    63,    64,    65,    66,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
     193,   233,   243,   249,   105,   257,    60,   262,   298,   265,
      61,    62,    63,    64,    65,    66,   303,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,   195,   125,   270,   194,     0,   196,    60,     0,     0,
       0,    61,    62,    63,    64,    65,    66,     0,     0,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,   197,     0,     0,     0,     0,     0,    60,     0,
       0,     0,    61,    62,    63,    64,    65,    66,     0,     0,
      47,    48,    49,    50,    51,     0,     0,    54,    55,    56,
      57,    58,    59,   199,     0,     0,     0,     0,     0,    60,
       0,     0,     0,    61,    62,    63,    64,    65,    66,     0,
       0,    47,    48,    49,    50,    51,     0,     0,    54,    55,
      56,    57,    58,    59,   201,     0,     0,     0,     0,     0,
      60,     0,     0,     0,    61,    62,    63,    64,    65,    66,
       0,     0,    47,     0,     0,     0,   203,     0,     0,    54,
      55,    56,    57,    58,    59,     0,     0,     0,     0,     0,
       0,    60,     0,     0,    47,    61,    62,    63,    64,    65,
      66,    54,    55,    56,    57,    58,    59,   205,     0,     0,
       0,     0,     0,    60,     0,     0,     0,    61,    62,    63,
      64,    65,    66,     0,     0,    47,     0,     0,     0,   207,
       0,     0,    54,    55,    56,    57,    58,    59,     0,     0,
       0,     0,     0,     0,    60,     0,     0,    47,    61,    62,
      63,    64,    65,    66,    54,    55,    56,    57,    58,    59,
     209,     0,     0,     0,     0,     0,    60,     0,     0,     0,
      61,    62,    63,    64,    65,    66,     0,     0,    47,     0,
       0,   211,     0,     0,     0,     0,    55,    56,    57,    58,
      59,     0,     0,     0,     0,     0,     0,    60,     0,    47,
       0,    61,    62,    63,    64,    65,    66,    55,    56,    57,
      58,    59,   213,     0,     0,     0,     0,     0,    60,     0,
       0,     0,    61,    62,    63,    64,    65,    66,     0,     0,
      47,     0,     0,   215,     0,     0,     0,     0,    55,     0,
       0,     0,    59,   217,     0,     0,     0,     0,     0,    60,
       0,    47,     0,    61,    62,    63,    64,    65,    66,    55,
       0,    47,     0,    59,   219,     0,     0,     0,     0,    55,
      60,     0,     0,    59,    61,    62,    63,    64,    65,    66,
      60,     0,    47,     0,    61,    62,    63,    64,    65,    66,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    60,     0,     0,     0,    61,    62,    63,    64,    65,
      66,     4,     0,     5,     6,     0,     7,     8,     0,     9,
      10,    11,    12,     4,    13,     5,     6,     0,     7,     8,
       0,     9,    10,    11,    12,     0,    13,     0,     0,     0,
      14,    15,     0,     0,     0,     0,     0,     0,    16,   223,
       0,     0,    14,    15,     0,    17,    18,     0,     0,     0,
      16,     0,     0,     0,     0,     0,     0,    17,    18,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,     0,     0,     0,     0,   164,     0,
      60,     0,     0,     0,    61,    62,    63,    64,    65,    66,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     0,     0,     0,     0,     0,
       0,    60,     0,     0,     0,    61,    62,    63,    64,    65,
      66
};

static const yytype_int16 yycheck[] =
{
      25,    10,    27,    47,    29,    87,    31,    92,     1,     1,
       3,    55,     5,     6,     1,     8,     9,     1,    11,    12,
      13,    14,     1,    16,     1,     1,     1,    36,     1,    38,
       1,     1,     1,    42,    11,     1,    17,     1,     0,    32,
      33,     1,     1,     7,    17,    10,     1,    40,    41,    19,
      15,    60,     1,     1,    47,    48,    48,    27,    39,    36,
     145,    48,    35,    47,    48,    40,     1,     1,    38,    48,
      47,    47,    42,    43,    44,    45,    46,    47,    47,    38,
      40,    47,    91,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    47,    47,
       1,     1,     1,     1,    38,    40,     1,     1,    42,    43,
      44,    45,    46,    47,     1,     1,   160,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    19,    32,    33,     1,     1,     1,    38,    40,
      27,    36,    42,    43,    44,    45,    46,    47,    47,    47,
       1,    38,    47,    47,     1,    42,    43,    44,    45,    46,
      47,     1,     1,   172,     1,     1,     1,     1,     1,     1,
       1,     1,    37,    39,   183,    39,     1,    17,   187,     1,
      17,    17,    17,     1,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    39,
       1,     1,    39,    39,    39,    38,    37,    39,   233,    42,
      43,    44,    45,    46,    47,     1,    38,   226,     1,   301,
      38,   303,    20,    21,    22,    23,   235,    32,    33,    34,
     239,     1,     1,     1,     1,    28,    29,    30,    38,     1,
     249,    42,    43,    44,    45,    46,    47,   272,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,     1,    32,    33,    34,     1,    38,    38,
      38,    38,    34,    42,    43,    44,    45,    46,    47,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,     1,     1,     1,     1,     1,     1,
      38,    35,    24,    25,    42,    43,    44,    45,    46,    47,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,     1,     1,     1,     1,    35,
      35,    38,    35,    35,    17,    42,    43,    44,    45,    46,
      47,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,     1,     1,    26,    27,
      35,    35,    38,   157,   158,   159,    42,    43,    44,    45,
      46,    47,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,     1,   149,   150,
      38,    35,    18,    38,    31,   155,   156,    42,    43,    44,
      45,    46,    47,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,     1,   151,
     152,   153,   154,     1,    38,    34,    36,     4,    42,    43,
      44,    45,    46,    47,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,     1,
      39,    39,    34,    39,     1,    38,    36,    48,    39,    42,
      43,    44,    45,    46,    47,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
       1,     4,    39,    34,    40,    40,    38,    36,    41,    39,
      42,    43,    44,    45,    46,    47,    37,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,     1,    44,   256,   147,    -1,   148,    38,    -1,    -1,
      -1,    42,    43,    44,    45,    46,    47,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,     1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    47,    -1,    -1,
      19,    20,    21,    22,    23,    -1,    -1,    26,    27,    28,
      29,    30,    31,     1,    -1,    -1,    -1,    -1,    -1,    38,
      -1,    -1,    -1,    42,    43,    44,    45,    46,    47,    -1,
      -1,    19,    20,    21,    22,    23,    -1,    -1,    26,    27,
      28,    29,    30,    31,     1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    42,    43,    44,    45,    46,    47,
      -1,    -1,    19,    -1,    -1,    -1,     1,    -1,    -1,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    19,    42,    43,    44,    45,    46,
      47,    26,    27,    28,    29,    30,    31,     1,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    -1,    -1,    42,    43,    44,
      45,    46,    47,    -1,    -1,    19,    -1,    -1,    -1,     1,
      -1,    -1,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    38,    -1,    -1,    19,    42,    43,
      44,    45,    46,    47,    26,    27,    28,    29,    30,    31,
       1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,
      42,    43,    44,    45,    46,    47,    -1,    -1,    19,    -1,
      -1,     1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    19,
      -1,    42,    43,    44,    45,    46,    47,    27,    28,    29,
      30,    31,     1,    -1,    -1,    -1,    -1,    -1,    38,    -1,
      -1,    -1,    42,    43,    44,    45,    46,    47,    -1,    -1,
      19,    -1,    -1,     1,    -1,    -1,    -1,    -1,    27,    -1,
      -1,    -1,    31,     1,    -1,    -1,    -1,    -1,    -1,    38,
      -1,    19,    -1,    42,    43,    44,    45,    46,    47,    27,
      -1,    19,    -1,    31,     1,    -1,    -1,    -1,    -1,    27,
      38,    -1,    -1,    31,    42,    43,    44,    45,    46,    47,
      38,    -1,    19,    -1,    42,    43,    44,    45,    46,    47,
      27,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    -1,    42,    43,    44,    45,    46,
      47,     3,    -1,     5,     6,    -1,     8,     9,    -1,    11,
      12,    13,    14,     3,    16,     5,     6,    -1,     8,     9,
      -1,    11,    12,    13,    14,    -1,    16,    -1,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      -1,    -1,    32,    33,    -1,    47,    48,    -1,    -1,    -1,
      40,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    36,    -1,
      38,    -1,    -1,    -1,    42,    43,    44,    45,    46,    47,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    -1,    42,    43,    44,    45,    46,
      47
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    53,    54,     0,     3,     5,     6,     8,     9,    11,
      12,    13,    14,    16,    32,    33,    40,    47,    48,    55,
      56,    58,    60,    61,    62,    64,    65,    66,    67,    68,
      69,    70,    72,    83,    84,     1,    38,     1,    38,     1,
      38,     1,    38,     1,    48,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      38,    42,    43,    44,    45,    46,    47,    90,    91,    92,
      93,    94,    95,    96,    97,    98,     1,    35,     1,    35,
       1,    48,     1,    47,     1,    47,     1,    57,     1,    32,
      33,    34,    38,     1,    11,    36,    47,    59,     1,    35,
       1,    35,     1,    35,     1,    40,    56,     1,    56,     1,
      56,     1,    56,     1,    35,     1,    90,     1,    90,     1,
      47,    48,    71,    90,     1,    59,     1,     1,     1,    97,
       1,     1,     1,     1,     1,     1,     1,     1,    97,     1,
       1,     1,     1,     1,    90,    38,    99,    17,    18,    24,
      25,    20,    21,    22,    23,    26,    27,    28,    29,    30,
      31,     1,    47,    54,    36,    88,    89,    90,     1,    90,
      88,     1,    34,    36,    63,     1,     7,    39,     1,    39,
      39,     1,    39,    34,     1,    47,     1,    35,    39,    39,
       1,    39,    88,     1,    91,     1,    92,     1,    93,     1,
      93,     1,    94,     1,    94,     1,    94,     1,    94,     1,
      95,     1,    95,     1,    96,     1,    96,     1,    96,     1,
      97,     1,    38,    41,     1,    39,    36,     1,    39,     1,
      90,     1,    47,     4,     1,    38,     1,    90,     1,    34,
       1,    90,    73,    39,    48,    86,    87,     1,    90,    34,
      56,     1,    90,     1,    90,     1,    35,    40,     1,    47,
       1,    39,    36,     1,    90,    39,     1,    39,     1,    47,
      60,    74,    85,     1,    48,     1,    35,     1,    39,    77,
      56,     1,    47,    10,    15,    76,    78,    80,     1,    42,
      43,    44,    45,    46,    47,    82,     1,    37,    41,     1,
      79,    81,    75,    37,    54,    54
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    57,    56,    58,    58,    58,
      58,    58,    58,    59,    59,    59,    59,    59,    59,    59,
      59,    60,    60,    60,    60,    60,    60,    60,    60,    61,
      61,    61,    62,    63,    62,    62,    64,    64,    64,    64,
      64,    65,    65,    66,    66,    66,    66,    66,    67,    67,
      67,    67,    67,    67,    67,    67,    68,    69,    69,    70,
      70,    70,    70,    70,    70,    70,    70,    71,    71,    71,
      71,    71,    71,    73,    74,    75,    72,    72,    76,    76,
      77,    77,    79,    78,    78,    78,    81,    80,    80,    82,
      82,    82,    82,    82,    82,    83,    83,    85,    84,    84,
      84,    84,    84,    86,    86,    87,    87,    87,    87,    87,
      88,    88,    89,    89,    89,    89,    90,    90,    90,    90,
      91,    91,    91,    91,    92,    92,    92,    92,    92,    92,
      92,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    94,    94,    94,    94,    94,    94,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      96,    96,    96,    96,    97,    97,    97,    97,    97,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    99,    99
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     0,     4,     2,     3,     2,
       2,     3,     2,     1,     3,     3,     5,     3,     3,     5,
       2,     3,     2,     2,     2,     2,     3,     2,     2,     4,
       4,     4,     2,     0,     5,     2,     4,     2,     3,     4,
       4,     2,     2,     4,     2,     3,     4,     4,     7,     2,
       3,     4,     5,     6,     6,     7,     1,     2,     2,     8,
       2,     3,     4,     5,     6,     7,     8,     3,     4,     3,
       2,     3,     4,     0,     0,     0,    11,     2,     0,     1,
       0,     2,     0,     5,     2,     3,     0,     4,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     0,     8,     2,
       3,     4,     6,     0,     1,     2,     4,     2,     3,     4,
       0,     1,     1,     3,     3,     2,     3,     1,     3,     2,
       3,     1,     3,     2,     3,     3,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       2,     2,     2,     2,     3,     3,     1,     3,     3,     2,
       3,     3,     3,     1,     3,     3,     3,     2,     2,     2,
       3,     1,     3,     2,     2,     2,     1,     2,     2,     1,
       1,     1,     1,     1,     3,     2,     3,     3,     0,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* STATEMENTS: %empty  */
#line 357 "parser.y"
                { (yyval.voidData) = NULL; }
#line 1911 "parser.tab.c"
    break;

  case 9: /* STATEMENT: BREAK SEMI_COLON  */
#line 366 "parser.y"
                       { 
        char* b = get_break_label();
        if (!b) reportError(SEMANTIC_ERROR, "'break' Used Outside Loop/Switch", (yylsp[-1]).first_line); // DONE
        else addQuadruple(OP_GOTO, NULL, NULL, b);
    }
#line 1921 "parser.tab.c"
    break;

  case 10: /* STATEMENT: CONTINUE SEMI_COLON  */
#line 371 "parser.y"
                          {
        char* c = get_continue_label();
        if (!c) reportError(SEMANTIC_ERROR, "'continue' Used Outside Loop", (yylsp[-1]).first_line); // DONE
        else addQuadruple(OP_GOTO, NULL, NULL, c);
    }
#line 1931 "parser.tab.c"
    break;

  case 18: /* STATEMENT: DECLARATION error  */
#line 385 "parser.y"
                        { 
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1940 "parser.tab.c"
    break;

  case 19: /* STATEMENT: ASSIGNMENT error  */
#line 389 "parser.y"
                       {
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1949 "parser.tab.c"
    break;

  case 20: /* STATEMENT: FUNCTION_CALL error  */
#line 393 "parser.y"
                          {
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1958 "parser.tab.c"
    break;

  case 21: /* STATEMENT: RETURN_STATEMENT error  */
#line 397 "parser.y"
                             {
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1967 "parser.tab.c"
    break;

  case 22: /* STATEMENT: BREAK error  */
#line 401 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1976 "parser.tab.c"
    break;

  case 23: /* STATEMENT: CONTINUE error  */
#line 405 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Expected ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1985 "parser.tab.c"
    break;

  case 24: /* STATEMENT: LEFT_CURLY_BRACKET error  */
#line 409 "parser.y"
                               {
        reportError(SYNTAX_ERROR, "Expected '}' To Match '{'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 1994 "parser.tab.c"
    break;

  case 25: /* $@1: %empty  */
#line 416 "parser.y"
                       {
        if (!comingFromForLoopHeader)
        {
            enterScope(&gScopeStack);
        }
        if (pending_params_should_insert && pending_function_params) {
            Parameter* p = pending_function_params;
            while (p) {
                if (p->Name) {
                    singleEntryNode* a = lookupCurrentScope(&gScopeStack, p->Name);
                    if (a) reportError(SEMANTIC_ERROR, "Duplicate parameter name", (yylsp[0]).first_line);
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
#line 2022 "parser.tab.c"
    break;

  case 26: /* BLOCK: LEFT_CURLY_BRACKET $@1 STATEMENTS RIGHT_CURLY_BRACKET  */
#line 439 "parser.y"
                                   { exitScope(&gScopeStack); comingFromForLoopHeader = false; (yyval.voidData) = NULL; }
#line 2028 "parser.tab.c"
    break;

  case 27: /* DECLARATION: DATATYPE IDENTIFIERS  */
#line 443 "parser.y"
                         {
    type declType = datatypeStringToType((yyvsp[-1].stringData));
    bool isConst = false;

    DeclNode* cur = (yyvsp[0].declList);
    while (cur) {

      // 1) redeclaration check (same scope only)
    if (lookupCurrentScope(&gScopeStack, cur->name)) 
    {
        reportError(SEMANTIC_ERROR, "Redeclaration In The Same Scope", cur->line);
        cur = cur->next;
        continue;
    }

      // 2) default value = zero (prevents garbage)
    value initVal;
    memset(&initVal, 0, sizeof(initVal));
    bool hasInit = (cur->initExpr != NULL);

    Expression* rhs = cur->initExpr;

      // 3) if has init: type check + cast if needed
    if (hasInit) 
    {
        if (!isTypeCompatible(declType, rhs->expressionType)) 
        {
          // isTypeCompatible reports already
          hasInit = false; // treat as "not initialized"
        } 
        else if (isNumeric(declType) && isNumeric(rhs->expressionType) && declType != rhs->expressionType) 
        {
          rhs = castTo(rhs, declType);  // generates cast quadruple + returns expr of declType
        }

        if (declType == INT_TYPE) initVal.INT_Data = rhs->expressionValue.INT_Data;
        else if (declType == FLOAT_TYPE) initVal.FLOAT_Data = rhs->expressionValue.FLOAT_Data;
        else if (declType == BOOL_TYPE) initVal.BOOL_Data = rhs->expressionValue.BOOL_Data;
        else if (declType == CHAR_TYPE) initVal.CHAR_Data = rhs->expressionValue.CHAR_Data;
        else if (declType == STRING_TYPE) initVal.STRING_Data = rhs->expressionValue.STRING_Data ? strdup(rhs->expressionValue.STRING_Data) : NULL;
        
    }

      // 4) Insert variable exactly once (even if no init)
    singleEntryNode* e = createNewEntry(declType, cur->name, SYMBOL_VARIABLE, initVal, hasInit, isConst, NULL);
    insertInCurrentScope(&gScopeStack, e);

      // 5) Generate assignment quadruple only if initializer exists
    if (hasInit) 
    {
        char* op = exprToOperand(rhs);
        addQuadruple(OP_ASSN, op, NULL, cur->name);
        free(op);
    }

    cur = cur->next;
    }

    freeDeclList((yyvsp[0].declList));
    (yyval.voidData) = NULL;
    }
#line 2094 "parser.tab.c"
    break;

  case 28: /* DECLARATION: CONST DATATYPE IDENTIFIERS  */
#line 504 "parser.y"
                                 {
        type declType = datatypeStringToType((yyvsp[-1].stringData));
        bool isConst = true;
        DeclNode* cur = (yyvsp[0].declList);
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
            //updateVariableValueScoped(&gScopeStack, cur->name, declType, initVal);
            cur = cur->next;
        }
        freeDeclList((yyvsp[0].declList));
        (yyval.voidData) = NULL;
    }
#line 2131 "parser.tab.c"
    break;

  case 29: /* DECLARATION: DATATYPE error  */
#line 538 "parser.y"
                     { 
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After Datatype", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.voidData) = NULL;
    }
#line 2141 "parser.tab.c"
    break;

  case 30: /* DECLARATION: CONST error  */
#line 543 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'const'", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.voidData) = NULL;
    }
#line 2151 "parser.tab.c"
    break;

  case 31: /* DECLARATION: CONST DATATYPE error  */
#line 548 "parser.y"
                           {
        reportError(SYNTAX_ERROR, "Expected Identifier/Declarator After 'const datatype'", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.voidData) = NULL;
    }
#line 2161 "parser.tab.c"
    break;

  case 32: /* DECLARATION: DATATYPE CONST  */
#line 553 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Unexpected Datatype Before 'const'", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.voidData) = NULL;
    }
#line 2171 "parser.tab.c"
    break;

  case 33: /* IDENTIFIERS: IDENTIFIER  */
#line 561 "parser.y"
               { (yyval.declList) = makeDeclNode((yyvsp[0].stringData), NULL, (yylsp[0]).first_line); }
#line 2177 "parser.tab.c"
    break;

  case 34: /* IDENTIFIERS: IDENTIFIER EQUAL LOGICAL_EXPRESSION  */
#line 562 "parser.y"
                                          { (yyval.declList) = makeDeclNode((yyvsp[-2].stringData), (yyvsp[0].expr), (yylsp[-2]).first_line); }
#line 2183 "parser.tab.c"
    break;

  case 35: /* IDENTIFIERS: IDENTIFIERS COMMA IDENTIFIER  */
#line 563 "parser.y"
                                   { (yyval.declList) = appendDeclNode((yyvsp[-2].declList), makeDeclNode((yyvsp[0].stringData), NULL, (yylsp[0]).first_line)); }
#line 2189 "parser.tab.c"
    break;

  case 36: /* IDENTIFIERS: IDENTIFIERS COMMA IDENTIFIER EQUAL LOGICAL_EXPRESSION  */
#line 564 "parser.y"
                                                            { (yyval.declList) = appendDeclNode((yyvsp[-4].declList), makeDeclNode((yyvsp[-2].stringData), (yyvsp[0].expr), (yylsp[-2]).first_line)); }
#line 2195 "parser.tab.c"
    break;

  case 37: /* IDENTIFIERS: IDENTIFIER EQUAL error  */
#line 567 "parser.y"
                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.declList) = makeDeclNode((yyvsp[-2].stringData), NULL, (yylsp[-2]).first_line);
    }
#line 2205 "parser.tab.c"
    break;

  case 38: /* IDENTIFIERS: IDENTIFIERS COMMA error  */
#line 572 "parser.y"
                              {
        reportError(SYNTAX_ERROR, "Expected Identifier After ','", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.declList) = (yyvsp[-2].declList);
    }
#line 2215 "parser.tab.c"
    break;

  case 39: /* IDENTIFIERS: IDENTIFIERS COMMA IDENTIFIER EQUAL error  */
#line 577 "parser.y"
                                               {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.declList) = (yyvsp[-4].declList);
    }
#line 2225 "parser.tab.c"
    break;

  case 40: /* IDENTIFIERS: COMMA error  */
#line 582 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Unxpected ',' Before First Identifier/Declarator", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.declList) = NULL;
    }
#line 2235 "parser.tab.c"
    break;

  case 41: /* ASSIGNMENT: IDENTIFIER EQUAL LOGICAL_EXPRESSION  */
#line 590 "parser.y"
                                        {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[-2].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", (yylsp[-2]).first_line); // DONE
        else
        {
            if (e->isReadOnly) reportError(SEMANTIC_ERROR, "Cannot Assign To const", (yylsp[-2]).first_line); // DONE
            else if (!isTypeCompatible(e->identifierType, (yyvsp[0].expr)->expressionType)) { }  // isTypeCompatible already reports
            else 
            {
                // char* rhs = exprToOperand($3);
                // addQuadruple(OP_ASSN, rhs, NULL, $1);
                // free(rhs);
                // updateVariableValueScoped(&gScopeStack, $1, e->identifierType, $3->expressionValue);
                value assigningVal;
                memset(&assigningVal, 0, sizeof(assigningVal));
                Expression* rhs = (yyvsp[0].expr);
                if (isNumeric(e->identifierType) && isNumeric(rhs->expressionType) && e->identifierType != rhs->expressionType) 
                {
                    rhs = castTo(rhs, e->identifierType);  
                }

                if (e->identifierType == INT_TYPE) assigningVal.INT_Data = rhs->expressionValue.INT_Data;
                else if (e->identifierType == FLOAT_TYPE) assigningVal.FLOAT_Data = rhs->expressionValue.FLOAT_Data;
                else if (e->identifierType == BOOL_TYPE) assigningVal.BOOL_Data = rhs->expressionValue.BOOL_Data;
                else if (e->identifierType == CHAR_TYPE) assigningVal.CHAR_Data = rhs->expressionValue.CHAR_Data;
                else if (e->identifierType == STRING_TYPE) assigningVal.STRING_Data = rhs->expressionValue.STRING_Data ? strdup(rhs->expressionValue.STRING_Data) : NULL;
                // char* rhs = exprToOperand($3);
                // addQuadruple(OP_ASSN, rhs, NULL, $1);
                // free(rhs);
                updateVariableValueScoped(&gScopeStack, (yyvsp[-2].stringData), e->identifierType, assigningVal);
            }
        }
    }
#line 2273 "parser.tab.c"
    break;

  case 42: /* ASSIGNMENT: IDENTIFIER INCREMENT  */
#line 623 "parser.y"
                           {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[-1].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", (yylsp[-1]).first_line); // DONE
        else 
        {
            addQuadruple(OP_INC, NULL, NULL, (yyvsp[-1].stringData));
            value incrementValue;
            memset(&incrementValue, 0, sizeof(incrementValue));
            if (e->identifierType == INT_TYPE)
                incrementValue.INT_Data = e->currentValue.INT_Data + 1;
            else if (e->identifierType == FLOAT_TYPE)
                incrementValue.FLOAT_Data = e->currentValue.FLOAT_Data + 1.0f;
            else if (e->identifierType == BOOL_TYPE)
                incrementValue.BOOL_Data = boolToInt(intToBool(e->currentValue.BOOL_Data) + 1);
            updateVariableValueScoped(&gScopeStack, (yyvsp[-1].stringData), e->identifierType, incrementValue);
        }
    }
#line 2295 "parser.tab.c"
    break;

  case 43: /* ASSIGNMENT: IDENTIFIER DECREMENT  */
#line 640 "parser.y"
                           {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[-1].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", (yylsp[-1]).first_line); // DONE
        else 
        {
            addQuadruple(OP_DEC, NULL, NULL, (yyvsp[-1].stringData));
            value decrementValue;
            memset(&decrementValue, 0, sizeof(decrementValue));
            if (e->identifierType == INT_TYPE)
                decrementValue.INT_Data = e->currentValue.INT_Data - 1;
            else if (e->identifierType == FLOAT_TYPE)
                decrementValue.FLOAT_Data = e->currentValue.FLOAT_Data - 1.0f;
            else if (e->identifierType == BOOL_TYPE)
                decrementValue.BOOL_Data = boolToInt(intToBool(e->currentValue.BOOL_Data) - 1);
            updateVariableValueScoped(&gScopeStack, (yyvsp[-1].stringData), e->identifierType, decrementValue);
        }
    }
#line 2317 "parser.tab.c"
    break;

  case 44: /* ASSIGNMENT: INCREMENT IDENTIFIER  */
#line 657 "parser.y"
                           {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[0].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", (yylsp[0]).first_line); // DONE
        else 
        {
            addQuadruple(OP_INC, NULL, NULL, (yyvsp[0].stringData));
            value incrementValue;
            memset(&incrementValue, 0, sizeof(incrementValue));
            if (e->identifierType == INT_TYPE)
                incrementValue.INT_Data = e->currentValue.INT_Data + 1;
            else if (e->identifierType == FLOAT_TYPE)
                incrementValue.FLOAT_Data = e->currentValue.FLOAT_Data + 1.0f;
            else if (e->identifierType == BOOL_TYPE)
                incrementValue.BOOL_Data = boolToInt(intToBool(e->currentValue.BOOL_Data) + 1);
            updateVariableValueScoped(&gScopeStack, (yyvsp[0].stringData), e->identifierType, incrementValue);
        }
    }
#line 2339 "parser.tab.c"
    break;

  case 45: /* ASSIGNMENT: DECREMENT IDENTIFIER  */
#line 674 "parser.y"
                           {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[0].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier Is Undeclared", (yylsp[0]).first_line); // DONE
        else 
        {
            addQuadruple(OP_DEC, NULL, NULL, (yyvsp[0].stringData));
            value decrementValue;
            memset(&decrementValue, 0, sizeof(decrementValue));
            if (e->identifierType == INT_TYPE)
            {
                decrementValue.INT_Data = e->currentValue.INT_Data - 1;
            } 
            else if (e->identifierType == FLOAT_TYPE)
            {
                decrementValue.FLOAT_Data = e->currentValue.FLOAT_Data - 1.0f;
            }
            else if (e->identifierType == BOOL_TYPE)
            {
                decrementValue.BOOL_Data = boolToInt(intToBool(e->currentValue.BOOL_Data) - 1);
            }
            updateVariableValueScoped(&gScopeStack, (yyvsp[0].stringData), e->identifierType, decrementValue);
        }
    }
#line 2367 "parser.tab.c"
    break;

  case 46: /* ASSIGNMENT: IDENTIFIER EQUAL error  */
#line 699 "parser.y"
                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2376 "parser.tab.c"
    break;

  case 47: /* ASSIGNMENT: INCREMENT error  */
#line 703 "parser.y"
                      {
        reportError(SYNTAX_ERROR, "Expected Identifier After '++'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2385 "parser.tab.c"
    break;

  case 48: /* ASSIGNMENT: DECREMENT error  */
#line 707 "parser.y"
                      {
        reportError(SYNTAX_ERROR, "Expected Identifier After '--'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2394 "parser.tab.c"
    break;

  case 49: /* FUNCTION_CALL: IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET  */
#line 714 "parser.y"
                                                                    {
        singleEntryNode* fn = functionLookup(&gScopeStack, (yyvsp[-3].stringData));
        if (!fn || fn->varOrFunc != SYMBOL_FUNCTION) {
            reportError(SEMANTIC_ERROR, "Call To Undeclared Function", (yylsp[-3]).first_line); // DONE
            (yyval.expr) = makeTempExpr(INT_TYPE, (yyvsp[-3].stringData));
        }
        else 
        {
            fn->isUsed = true;

            ArgNode* a = (yyvsp[-1].argList);
            Parameter* p = fn->parameterList;
            int argIndex = 1;

            while (a && p)
            {
                if (p->Type != a->expr->expressionType)
                {
                    char buffer[256];
                    snprintf(buffer, sizeof(buffer),
                            "Type Mismatch In Function Call at argument %d. Expected %s but got %s",
                            argIndex, typeToString(p->Type), typeToString(a->expr->expressionType));
                    reportError(SEMANTIC_ERROR, buffer, (yylsp[-3]).first_line);
                }

                // emit param anyway (optional but common)
                char* op = exprToOperand(a->expr);
                addQuadruple(OP_PARM, op, NULL, NULL);
                free(op);

                a = a->next;
                p = p->Next;
                argIndex++;
            }

            // too many args
            if (a && !p)
            {
                reportError(SEMANTIC_ERROR, "Too Many Arguments In Function Call", (yylsp[-3]).first_line);

                while (a)
                {
                    char* op = exprToOperand(a->expr);
                    addQuadruple(OP_PARM, op, NULL, NULL);
                    free(op);
                    a = a->next;
                }
            }

            // too few args
            if (!a && p)
            {
                reportError(SEMANTIC_ERROR, "Too Few Arguments In Function Call", (yylsp[-3]).first_line);
            }


            // Now call
            if (fn->identifierType != VOID_TYPE) {
                char* t = createTemp();
                addQuadruple(OP_CALL, (yyvsp[-3].stringData), NULL, t);
                (yyval.expr) = makeTempExpr(fn->identifierType, t);
                free(t);
            } else {
                addQuadruple(OP_CALL, (yyvsp[-3].stringData), NULL, NULL);
                value v; memset(&v, 0, sizeof(v));
                (yyval.expr) = makeExpr(VOID_TYPE, v, NULL);
            }
        }

        freeArgList((yyvsp[-1].argList));
    }
#line 2470 "parser.tab.c"
    break;

  case 50: /* FUNCTION_CALL: IDENTIFIER LEFT_ROUND_BRACKET ARGUMENT_LIST error  */
#line 787 "parser.y"
                                                        {
        reportError(SYNTAX_ERROR, "Expected ')' In Function Call", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2479 "parser.tab.c"
    break;

  case 51: /* FUNCTION_CALL: IDENTIFIER error ARGUMENT_LIST RIGHT_ROUND_BRACKET  */
#line 791 "parser.y"
                                                         {
        reportError(SYNTAX_ERROR, "Expected '(' In Function Call", (yylsp[-2]).first_line); // DONE
        yyerrok;
    }
#line 2488 "parser.tab.c"
    break;

  case 52: /* IF_STATEMENT: IF_HEAD BLOCK  */
#line 798 "parser.y"
                                        {
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-1].codeUtils).false_label);
        free((yyvsp[-1].codeUtils).false_label);
        free((yyvsp[-1].codeUtils).end_label);
        (yyval.codeUtils).code = NULL;
    }
#line 2499 "parser.tab.c"
    break;

  case 53: /* $@2: %empty  */
#line 804 "parser.y"
                    {
        addQuadruple(OP_GOTO, NULL, NULL, (yyvsp[-1].codeUtils).end_label);
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-1].codeUtils).false_label);
    }
#line 2508 "parser.tab.c"
    break;

  case 54: /* IF_STATEMENT: IF_HEAD BLOCK $@2 ELSE BLOCK  */
#line 808 "parser.y"
               {
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-4].codeUtils).end_label);
        free((yyvsp[-4].codeUtils).false_label);
        free((yyvsp[-4].codeUtils).end_label);
        (yyval.codeUtils).code = NULL;
    }
#line 2519 "parser.tab.c"
    break;

  case 55: /* IF_STATEMENT: IF_HEAD error  */
#line 816 "parser.y"
                    {
        reportError(SYNTAX_ERROR, "Expected If Block '{ }' After 'if (...)'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2528 "parser.tab.c"
    break;

  case 56: /* IF_HEAD: IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET  */
#line 823 "parser.y"
                                                                 {
        (yyval.codeUtils).false_label = createLabel();
        (yyval.codeUtils).end_label = createLabel();
        char* cond = exprToOperand((yyvsp[-1].expr));
        addQuadruple(OP_IFFALSE, cond, NULL, (yyval.codeUtils).false_label);
        free(cond);
    }
#line 2540 "parser.tab.c"
    break;

  case 57: /* IF_HEAD: IF error  */
#line 832 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Expected '(' After 'if'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2549 "parser.tab.c"
    break;

  case 58: /* IF_HEAD: IF LEFT_ROUND_BRACKET error  */
#line 836 "parser.y"
                                  {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2558 "parser.tab.c"
    break;

  case 59: /* IF_HEAD: IF LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error  */
#line 840 "parser.y"
                                                     {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2567 "parser.tab.c"
    break;

  case 60: /* IF_HEAD: IF LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET  */
#line 844 "parser.y"
                                                      {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'if'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2576 "parser.tab.c"
    break;

  case 61: /* WHILE_STATEMENT: WHILE_HEAD BLOCK  */
#line 851 "parser.y"
                     {
        pop_loop_labels();
        addQuadruple(OP_GOTO, NULL, NULL, (yyvsp[-1].codeUtils).start_label);
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-1].codeUtils).end_label);
        free((yyvsp[-1].codeUtils).start_label);
        free((yyvsp[-1].codeUtils).end_label);
        (yyval.codeUtils).code = NULL;
    }
#line 2589 "parser.tab.c"
    break;

  case 62: /* WHILE_STATEMENT: WHILE_HEAD error  */
#line 861 "parser.y"
                       {
        reportError(SYNTAX_ERROR, "Expected While Block '{ }' After 'while'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2598 "parser.tab.c"
    break;

  case 63: /* WHILE_HEAD: WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET  */
#line 868 "parser.y"
                                                                    {
        (yyval.codeUtils).start_label = createLabel();
        (yyval.codeUtils).end_label   = createLabel();
        addQuadruple(OP_LABEL, NULL, NULL, (yyval.codeUtils).start_label);
        char* cond = exprToOperand((yyvsp[-1].expr));
        addQuadruple(OP_IFFALSE, cond, NULL, (yyval.codeUtils).end_label);
        free(cond);
        push_loop_labels((yyval.codeUtils).end_label, (yyval.codeUtils).start_label);
    }
#line 2612 "parser.tab.c"
    break;

  case 64: /* WHILE_HEAD: WHILE error  */
#line 879 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Expected '(' After 'while'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2621 "parser.tab.c"
    break;

  case 65: /* WHILE_HEAD: WHILE LEFT_ROUND_BRACKET error  */
#line 883 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2630 "parser.tab.c"
    break;

  case 66: /* WHILE_HEAD: WHILE LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error  */
#line 887 "parser.y"
                                                        {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2639 "parser.tab.c"
    break;

  case 67: /* WHILE_HEAD: WHILE LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET  */
#line 891 "parser.y"
                                                         {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'while'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2648 "parser.tab.c"
    break;

  case 68: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET SEMI_COLON  */
#line 898 "parser.y"
                                                                                                 {
        pop_loop_labels();
        char* cond = exprToOperand((yyvsp[-2].expr));
        addQuadruple(OP_IFFALSE, cond, NULL, (yyvsp[-6].codeUtils).start_label);
        free(cond);
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-6].codeUtils).end_label);
        free((yyvsp[-6].codeUtils).start_label);
        free((yyvsp[-6].codeUtils).end_label);
        (yyval.codeUtils).code = NULL;
    }
#line 2663 "parser.tab.c"
    break;

  case 69: /* REPEAT_STATEMENT: REPEAT_HEAD error  */
#line 910 "parser.y"
                        {
        reportError(SYNTAX_ERROR, "Expected Repeat Block '{ }' After 'repeat'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2672 "parser.tab.c"
    break;

  case 70: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK error  */
#line 914 "parser.y"
                              {
        reportError(SYNTAX_ERROR, "Expected 'until (...) ;' After Repeat Block", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2681 "parser.tab.c"
    break;

  case 71: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL error  */
#line 918 "parser.y"
                                    {
        reportError(SYNTAX_ERROR, "Expected '(' After 'until'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2690 "parser.tab.c"
    break;

  case 72: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET error  */
#line 922 "parser.y"
                                                       {
        reportError(SYNTAX_ERROR, "Expected Condition After '('", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2699 "parser.tab.c"
    break;

  case 73: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error  */
#line 926 "parser.y"
                                                                           {
        reportError(SYNTAX_ERROR, "Expected ')' After Condition", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2708 "parser.tab.c"
    break;

  case 74: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET  */
#line 930 "parser.y"
                                                                           {
        reportError(SYNTAX_ERROR, "Expected Condition Inside '( )' After 'until'", (yylsp[-2]).first_line); // DONE
        yyerrok;
    }
#line 2717 "parser.tab.c"
    break;

  case 75: /* REPEAT_STATEMENT: REPEAT_HEAD BLOCK UNTIL LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET error  */
#line 934 "parser.y"
                                                                                               {
        reportError(SYNTAX_ERROR, "Expected ';' After 'until (...)'", (yylsp[-2]).first_line); // DONE
        yyerrok;
    }
#line 2726 "parser.tab.c"
    break;

  case 76: /* REPEAT_HEAD: REPEAT  */
#line 941 "parser.y"
           {
        (yyval.codeUtils).start_label = createLabel();
        (yyval.codeUtils).end_label   = createLabel();
        addQuadruple(OP_LABEL, NULL, NULL, (yyval.codeUtils).start_label);
        push_loop_labels((yyval.codeUtils).end_label, (yyval.codeUtils).start_label);
    }
#line 2737 "parser.tab.c"
    break;

  case 77: /* FOR_STATEMENT: FOR_HEADER BLOCK  */
#line 950 "parser.y"
                     {
        pop_loop_labels();
        addQuadruple(OP_GOTO, NULL, NULL, (yyvsp[-1].codeUtils).incr_label);
        addQuadruple(OP_LABEL, NULL, NULL, (yyvsp[-1].codeUtils).end_label);
        free((yyvsp[-1].codeUtils).cond_label);
        free((yyvsp[-1].codeUtils).incr_label);
        free((yyvsp[-1].codeUtils).end_label);
        (yyval.codeUtils).code = NULL;
    }
#line 2751 "parser.tab.c"
    break;

  case 78: /* FOR_STATEMENT: FOR_HEADER error  */
#line 961 "parser.y"
                       {
        reportError(SYNTAX_ERROR, "Expected For Block '{ }' After 'for'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2760 "parser.tab.c"
    break;

  case 79: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT RIGHT_ROUND_BRACKET  */
#line 968 "parser.y"
                                                                                                            {
        (yyval.codeUtils).cond_label = createLabel();
        (yyval.codeUtils).incr_label = createLabel();
        (yyval.codeUtils).end_label  = createLabel();
        addQuadruple(OP_GOTO, NULL, NULL, (yyval.codeUtils).cond_label);
        addQuadruple(OP_LABEL, NULL, NULL, (yyval.codeUtils).incr_label);
        addQuadruple(OP_LABEL, NULL, NULL, (yyval.codeUtils).cond_label);
        char* cond = exprToOperand((yyvsp[-3].expr));
        addQuadruple(OP_IFFALSE, cond, NULL, (yyval.codeUtils).end_label);
        free(cond);
        push_loop_labels((yyval.codeUtils).end_label, (yyval.codeUtils).incr_label);
    }
#line 2777 "parser.tab.c"
    break;

  case 80: /* FOR_HEADER: FOR error  */
#line 982 "parser.y"
                {
        reportError(SYNTAX_ERROR, "Expected '(' After for", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2786 "parser.tab.c"
    break;

  case 81: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET error  */
#line 986 "parser.y"
                                   {
        reportError(SYNTAX_ERROR, "Expected Iterator After '('", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2795 "parser.tab.c"
    break;

  case 82: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR error  */
#line 990 "parser.y"
                                            {
        reportError(SYNTAX_ERROR, "Expected ';' After Iterator", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2804 "parser.tab.c"
    break;

  case 83: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON error  */
#line 994 "parser.y"
                                                       {
        reportError(SYNTAX_ERROR, "Expected Condition After ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2813 "parser.tab.c"
    break;

  case 84: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION error  */
#line 998 "parser.y"
                                                                          {
        reportError(SYNTAX_ERROR, "Expected ';' After Condition", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2822 "parser.tab.c"
    break;

  case 85: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON error  */
#line 1002 "parser.y"
                                                                                     {
        reportError(SYNTAX_ERROR, "Expected Loop Step After ';'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2831 "parser.tab.c"
    break;

  case 86: /* FOR_HEADER: FOR LEFT_ROUND_BRACKET ITERATOR SEMI_COLON LOGICAL_EXPRESSION SEMI_COLON ASSIGNMENT error  */
#line 1006 "parser.y"
                                                                                                {
        reportError(SYNTAX_ERROR, "Expected ')' After Loop Step", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2840 "parser.tab.c"
    break;

  case 87: /* ITERATOR: IDENTIFIER EQUAL LOGICAL_EXPRESSION  */
#line 1013 "parser.y"
                                        {
        enterScope(&gScopeStack); 
        comingFromForLoopHeader = true; 
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[-2].stringData));
        if (!e) reportError(SEMANTIC_ERROR, "Identifier is undeclared (for-init)", (yylsp[-2]).first_line);
        else if (e->isReadOnly) reportError(SEMANTIC_ERROR, "Cannot assign to const (for-init)", (yylsp[-2]).first_line);
        else if (!isTypeCompatible(e->identifierType, (yyvsp[0].expr)->expressionType)) { } // isTypeCompatible reports
        else {
            char* rhs = exprToOperand((yyvsp[0].expr));
            addQuadruple(OP_ASSN, rhs, NULL, (yyvsp[-2].stringData));
            free(rhs);
            updateVariableValueScoped(&gScopeStack, (yyvsp[-2].stringData), e->identifierType, (yyvsp[0].expr)->expressionValue);
        }
    }
#line 2859 "parser.tab.c"
    break;

  case 88: /* ITERATOR: DATATYPE IDENTIFIER EQUAL LOGICAL_EXPRESSION  */
#line 1027 "parser.y"
                                                   {
        enterScope(&gScopeStack); 
        comingFromForLoopHeader = true; 
        type declType = datatypeStringToType((yyvsp[-3].stringData));
        singleEntryNode* already = lookupCurrentScope(&gScopeStack, (yyvsp[-2].stringData));
        if (already) reportError(SEMANTIC_ERROR, "Redeclaration in same scope (for-init)", (yylsp[-2]).first_line);
        else {
            if (!isTypeCompatible(declType, (yyvsp[0].expr)->expressionType)) { } // isTypeCompatible reports
            else {
                value initVal = (yyvsp[0].expr)->expressionValue;
                singleEntryNode* e = createNewEntry(declType, (yyvsp[-2].stringData), SYMBOL_VARIABLE, initVal, true, false, NULL);
                insertInCurrentScope(&gScopeStack, e);
                char* rhs = exprToOperand((yyvsp[0].expr));
                addQuadruple(OP_ASSN, rhs, NULL, (yyvsp[-2].stringData));
                free(rhs);
                updateVariableValueScoped(&gScopeStack, (yyvsp[-2].stringData), declType, initVal);
            }
        }
    }
#line 2883 "parser.tab.c"
    break;

  case 89: /* ITERATOR: IDENTIFIER EQUAL error  */
#line 1048 "parser.y"
                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2892 "parser.tab.c"
    break;

  case 90: /* ITERATOR: DATATYPE error  */
#line 1052 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Expected Identifier After Datatype", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2901 "parser.tab.c"
    break;

  case 91: /* ITERATOR: DATATYPE IDENTIFIER error  */
#line 1056 "parser.y"
                                {
        reportError(SYNTAX_ERROR, "Expected '=' After Identifier", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 2910 "parser.tab.c"
    break;

  case 92: /* ITERATOR: DATATYPE IDENTIFIER EQUAL error  */
#line 1060 "parser.y"
                                      {
        reportError(SYNTAX_ERROR, "Expected Expression After '='", (yylsp[-2]).first_line); // DONE
        yyerrok;
    }
#line 2919 "parser.tab.c"
    break;

  case 93: /* $@3: %empty  */
#line 1067 "parser.y"
                                                                     {
        char* t  = createTemp();
        char* sw = exprToOperand((yyvsp[-1].expr));
        addQuadruple(OP_ASSN, sw, NULL, t);
        free(sw);
        current_switch_var = strdup(t);
        free(t);
        current_switch_end_label = createLabel();
        default_label = createLabel();
        push_loop_labels(current_switch_end_label, NULL);
    }
#line 2935 "parser.tab.c"
    break;

  case 94: /* $@4: %empty  */
#line 1078 "parser.y"
                       { enterScope(&gScopeStack); }
#line 2941 "parser.tab.c"
    break;

  case 95: /* $@5: %empty  */
#line 1078 "parser.y"
                                                                                                { exitScope(&gScopeStack); }
#line 2947 "parser.tab.c"
    break;

  case 96: /* SWITCH_STATEMENT: SWITCH LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET $@3 LEFT_CURLY_BRACKET $@4 CASES DEFAULT_CASE_OPT RIGHT_CURLY_BRACKET $@5  */
#line 1078 "parser.y"
                                                                                                                             {
        addQuadruple(OP_LABEL, NULL, NULL, default_label);
        addQuadruple(OP_LABEL, NULL, NULL, current_switch_end_label);
        pop_loop_labels();
        free(current_switch_var);
        current_switch_var = NULL;
        free(current_switch_end_label);
        current_switch_end_label = NULL;
        free(default_label);
        default_label = NULL;
        (yyval.codeUtils).code = NULL;
    }
#line 2964 "parser.tab.c"
    break;

  case 97: /* SWITCH_STATEMENT: SWITCH error  */
#line 1092 "parser.y"
                   {
        reportError(SYNTAX_ERROR, "Expected '(' After 'switch'", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 2973 "parser.tab.c"
    break;

  case 98: /* DEFAULT_CASE_OPT: %empty  */
#line 1099 "parser.y"
                { (yyval.voidData) = NULL; }
#line 2979 "parser.tab.c"
    break;

  case 100: /* CASES: %empty  */
#line 1104 "parser.y"
                { (yyval.voidData) = NULL; }
#line 2985 "parser.tab.c"
    break;

  case 102: /* $@6: %empty  */
#line 1109 "parser.y"
                      {
        current_case_next_label = createLabel();
        if (!current_switch_var) reportError(SEMANTIC_ERROR, "case used outside switch", (yylsp[-1]).first_line);
        else {
            char* caseOp = exprToOperand((yyvsp[0].expr));
            char* tcmp   = createTemp();
            addQuadruple(OP_EQ, current_switch_var, caseOp, tcmp);
            addQuadruple(OP_IFFALSE, tcmp, NULL, current_case_next_label);
            free(caseOp);
            free(tcmp);
        }
    }
#line 3002 "parser.tab.c"
    break;

  case 103: /* SINGLE_CASE: CASE PRIMARY_CASE $@6 COLON STATEMENTS  */
#line 1121 "parser.y"
                     {
        if (current_switch_end_label) addQuadruple(OP_GOTO, NULL, NULL, current_switch_end_label);
        addQuadruple(OP_LABEL, NULL, NULL, current_case_next_label);
        free(current_case_next_label);
        current_case_next_label = NULL;
        (yyval.voidData) = NULL;
    }
#line 3014 "parser.tab.c"
    break;

  case 104: /* SINGLE_CASE: CASE error  */
#line 1130 "parser.y"
                 {
        reportError(SYNTAX_ERROR, "Expected Case Value After 'case'", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3023 "parser.tab.c"
    break;

  case 105: /* SINGLE_CASE: CASE PRIMARY_CASE error  */
#line 1134 "parser.y"
                              {
        reportError(SYNTAX_ERROR, "Expected ':' After Case Value", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3032 "parser.tab.c"
    break;

  case 106: /* $@7: %empty  */
#line 1141 "parser.y"
                  { if (default_label) addQuadruple(OP_LABEL, NULL, NULL, default_label); }
#line 3038 "parser.tab.c"
    break;

  case 107: /* DEFAULT_CASE: DEFAULT COLON $@7 STATEMENTS  */
#line 1142 "parser.y"
               {
        if (current_switch_end_label) addQuadruple(OP_GOTO, NULL, NULL, current_switch_end_label);
        (yyval.voidData) = NULL;
    }
#line 3047 "parser.tab.c"
    break;

  case 108: /* DEFAULT_CASE: DEFAULT error  */
#line 1148 "parser.y"
                    {
        reportError(SYNTAX_ERROR, "Expected ':' After 'default'", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3056 "parser.tab.c"
    break;

  case 109: /* PRIMARY_CASE: INTVALUE  */
#line 1155 "parser.y"
             {
        value v;
        memset(&v, 0, sizeof(v));
        v.INT_Data = (yyvsp[0].intData);
        (yyval.expr) = makeExpr(INT_TYPE, v, NULL);
    }
#line 3067 "parser.tab.c"
    break;

  case 110: /* PRIMARY_CASE: CHARVALUE  */
#line 1161 "parser.y"
                {
        value v;
        memset(&v, 0, sizeof(v));
        v.CHAR_Data = (yyvsp[0].charData);
        (yyval.expr) = makeExpr(CHAR_TYPE, v, NULL);
    }
#line 3078 "parser.tab.c"
    break;

  case 111: /* PRIMARY_CASE: BOOLVALUE  */
#line 1167 "parser.y"
                {
        value v;
        memset(&v, 0, sizeof(v));
        v.BOOL_Data = (yyvsp[0].intData);
        (yyval.expr) = makeExpr(BOOL_TYPE, v, NULL);
    }
#line 3089 "parser.tab.c"
    break;

  case 112: /* PRIMARY_CASE: IDENTIFIER  */
#line 1173 "parser.y"
                 {
        singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[0].stringData));
        if (!e) {
            reportError(SEMANTIC_ERROR, "Use of undeclared identifier in case", (yylsp[0]).first_line);
            value v;
            memset(&v, 0, sizeof(v));
            (yyval.expr) = makeExpr(INT_TYPE, v, (yyvsp[0].stringData));
        }
        else (yyval.expr) = makeExpr(e->identifierType, e->currentValue, (yyvsp[0].stringData));
    }
#line 3104 "parser.tab.c"
    break;

  case 113: /* PRIMARY_CASE: FLOATVALUE  */
#line 1183 "parser.y"
                 {
        value v;
        memset(&v, 0, sizeof(v));
        v.FLOAT_Data = (yyvsp[0].floatData);
        (yyval.expr) = makeExpr(FLOAT_TYPE, v, NULL);
    }
#line 3115 "parser.tab.c"
    break;

  case 114: /* PRIMARY_CASE: STRINGVALUE  */
#line 1189 "parser.y"
                  {
        value v;
        memset(&v, 0, sizeof(v));
        v.STRING_Data = strdup((yyvsp[0].stringData));
        (yyval.expr) = makeExpr(STRING_TYPE, v, NULL);
    }
#line 3126 "parser.tab.c"
    break;

  case 115: /* RETURN_STATEMENT: RETURN  */
#line 1198 "parser.y"
           {
        if (inFunctionScope == false) reportError(SEMANTIC_ERROR, "Return used outside function", (yylsp[0]).first_line);
        if (current_function_return_type != VOID_TYPE) reportError(SEMANTIC_ERROR, "Missing return value", (yylsp[0]).first_line);
        else seen1ReturnStatement = true;
        addQuadruple(OP_RETURN, NULL, NULL, NULL);
    }
#line 3137 "parser.tab.c"
    break;

  case 116: /* RETURN_STATEMENT: RETURN LOGICAL_EXPRESSION  */
#line 1204 "parser.y"
                                {
        if (inFunctionScope == false) reportError(SEMANTIC_ERROR, "Return used outside function", (yylsp[-1]).first_line);
        if (current_function_return_type == VOID_TYPE && gScopeStack.topScope->scopeDepth != 0) reportError(SEMANTIC_ERROR, "Returning a value from void function", (yylsp[-1]).first_line);
        else if ((yyvsp[0].expr) && !isTypeCompatible(current_function_return_type, (yyvsp[0].expr)->expressionType)) { } // isTypeCompatible already reports
        else {
            seen1ReturnStatement = true;
            char* rv = exprToOperand((yyvsp[0].expr));
            addQuadruple(OP_RETURN, rv, NULL, NULL);
            free(rv);
        }
    }
#line 3153 "parser.tab.c"
    break;

  case 117: /* $@8: %empty  */
#line 1218 "parser.y"
                                                                                       {
        seen1ReturnStatement = false;
        inFunctionScope = true;
        type retType = datatypeStringToType((yyvsp[-4].stringData));
        singleEntryNode* already = lookupCurrentScope(&gScopeStack, (yyvsp[-3].stringData));
        if (already) reportError(SEMANTIC_ERROR, "Redeclaration of function in same scope", (yylsp[-3]).first_line);
        else {
            value v;
            memset(&v, 0, sizeof(v));
            singleEntryNode* fn = createNewEntry(retType, (yyvsp[-3].stringData), SYMBOL_FUNCTION, v, true, false, (yyvsp[-1].parameterList));
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
        pending_function_params = (yyvsp[-1].parameterList);
        pending_params_should_insert = true;

    }
#line 3192 "parser.tab.c"
    break;

  case 118: /* FUNCTION_DEFINITION_IMPLEMENTATION: FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST RIGHT_ROUND_BRACKET $@8 BLOCK  */
#line 1252 "parser.y"
          {
        // exitScope(&gScopeStack); /* commented to match the commented enter scope above */
        if (inFunctionScope) {
            if (current_function_return_type != VOID_TYPE && !seen1ReturnStatement) reportError(SEMANTIC_ERROR, "Missing return statement in non-void function", (yylsp[-6]).first_line);
            inFunctionScope = false;
        }
        current_function_return_type = VOID_TYPE;
    }
#line 3205 "parser.tab.c"
    break;

  case 119: /* FUNCTION_DEFINITION_IMPLEMENTATION: FUNCTION error  */
#line 1262 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Expected Datatype After 'function'", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3214 "parser.tab.c"
    break;

  case 120: /* FUNCTION_DEFINITION_IMPLEMENTATION: FUNCTION DATATYPE error  */
#line 1266 "parser.y"
                              {
        reportError(SYNTAX_ERROR, "Expected Identifier After Function Datatype", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3223 "parser.tab.c"
    break;

  case 121: /* FUNCTION_DEFINITION_IMPLEMENTATION: FUNCTION DATATYPE IDENTIFIER error  */
#line 1270 "parser.y"
                                         {
        reportError(SYNTAX_ERROR, "Expected '(' After Function Name", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3232 "parser.tab.c"
    break;

  case 122: /* FUNCTION_DEFINITION_IMPLEMENTATION: FUNCTION DATATYPE IDENTIFIER LEFT_ROUND_BRACKET PARAMETER_LIST error  */
#line 1274 "parser.y"
                                                                           {
        reportError(SYNTAX_ERROR, "Expected ')' After Parameters", (yylsp[-1]).first_line);
        yyerrok;
    }
#line 3241 "parser.tab.c"
    break;

  case 123: /* PARAMETER_LIST: %empty  */
#line 1281 "parser.y"
                { (yyval.parameterList) = NULL; }
#line 3247 "parser.tab.c"
    break;

  case 124: /* PARAMETER_LIST: PARAM_LIST_NONEMPTY  */
#line 1282 "parser.y"
                          { (yyval.parameterList) = (yyvsp[0].parameterList); }
#line 3253 "parser.tab.c"
    break;

  case 125: /* PARAM_LIST_NONEMPTY: DATATYPE IDENTIFIER  */
#line 1286 "parser.y"
                        {
        (yyval.parameterList) = makeParam(datatypeStringToType((yyvsp[-1].stringData)), (yyvsp[0].stringData));
    }
#line 3261 "parser.tab.c"
    break;

  case 126: /* PARAM_LIST_NONEMPTY: PARAM_LIST_NONEMPTY COMMA DATATYPE IDENTIFIER  */
#line 1289 "parser.y"
                                                    {
        (yyval.parameterList) = appendParam((yyvsp[-3].parameterList), makeParam(datatypeStringToType((yyvsp[-1].stringData)), (yyvsp[0].stringData)));
    }
#line 3269 "parser.tab.c"
    break;

  case 127: /* PARAM_LIST_NONEMPTY: DATATYPE error  */
#line 1294 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", (yylsp[-1]).first_line);
        yyerrok;
        (yyval.parameterList) = NULL;
    }
#line 3279 "parser.tab.c"
    break;

  case 128: /* PARAM_LIST_NONEMPTY: PARAM_LIST_NONEMPTY COMMA error  */
#line 1299 "parser.y"
                                      {
        reportError(SYNTAX_ERROR, "Expected Datatype After ',' In Parameter List", (yylsp[-1]).first_line);
        yyerrok;
        (yyval.parameterList) = (yyvsp[-2].parameterList);
    }
#line 3289 "parser.tab.c"
    break;

  case 129: /* PARAM_LIST_NONEMPTY: PARAM_LIST_NONEMPTY COMMA DATATYPE error  */
#line 1304 "parser.y"
                                               {
        reportError(SYNTAX_ERROR, "Expected Identifier After Parameter Datatype", (yylsp[-1]).first_line);
        yyerrok;
        (yyval.parameterList) = (yyvsp[-3].parameterList);
    }
#line 3299 "parser.tab.c"
    break;

  case 130: /* ARGUMENT_LIST: %empty  */
#line 1312 "parser.y"
                { (yyval.argList) = NULL; }
#line 3305 "parser.tab.c"
    break;

  case 131: /* ARGUMENT_LIST: ARGUMENTS  */
#line 1313 "parser.y"
                { (yyval.argList) = (yyvsp[0].argList); }
#line 3311 "parser.tab.c"
    break;

  case 132: /* ARGUMENTS: LOGICAL_EXPRESSION  */
#line 1317 "parser.y"
                       { (yyval.argList) = makeArgNode((yyvsp[0].expr)); }
#line 3317 "parser.tab.c"
    break;

  case 133: /* ARGUMENTS: ARGUMENTS COMMA LOGICAL_EXPRESSION  */
#line 1318 "parser.y"
                                         { (yyval.argList) = appendArgNode((yyvsp[-2].argList), makeArgNode((yyvsp[0].expr))); }
#line 3323 "parser.tab.c"
    break;

  case 134: /* ARGUMENTS: ARGUMENTS COMMA error  */
#line 1321 "parser.y"
                            {
        reportError(SYNTAX_ERROR, "Expected Expression After ',' In Arguments", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.argList) = (yyvsp[-2].argList);
    }
#line 3333 "parser.tab.c"
    break;

  case 135: /* ARGUMENTS: COMMA error  */
#line 1326 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Unexpected ',' Before First Argument", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.argList) = NULL;
    }
#line 3343 "parser.tab.c"
    break;

  case 136: /* LOGICAL_EXPRESSION: LOGICAL_EXPRESSION OR LOGICAL_TERM  */
#line 1334 "parser.y"
                                       {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_OR, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3358 "parser.tab.c"
    break;

  case 137: /* LOGICAL_EXPRESSION: LOGICAL_TERM  */
#line 1344 "parser.y"
                   { (yyval.expr) = (yyvsp[0].expr); }
#line 3364 "parser.tab.c"
    break;

  case 138: /* LOGICAL_EXPRESSION: LOGICAL_EXPRESSION OR error  */
#line 1347 "parser.y"
                                  {
        reportError(SYNTAX_ERROR, "Expected Expression After '||'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3373 "parser.tab.c"
    break;

  case 139: /* LOGICAL_EXPRESSION: OR error  */
#line 1351 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '||' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3382 "parser.tab.c"
    break;

  case 140: /* LOGICAL_TERM: LOGICAL_TERM AND EQUALITY_EXPRESSION  */
#line 1358 "parser.y"
                                         {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_AND, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3397 "parser.tab.c"
    break;

  case 141: /* LOGICAL_TERM: EQUALITY_EXPRESSION  */
#line 1368 "parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 3403 "parser.tab.c"
    break;

  case 142: /* LOGICAL_TERM: LOGICAL_TERM AND error  */
#line 1371 "parser.y"
                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '&&'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3412 "parser.tab.c"
    break;

  case 143: /* LOGICAL_TERM: AND error  */
#line 1375 "parser.y"
                {
        reportError(SYNTAX_ERROR, "Unexpected '&&' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3421 "parser.tab.c"
    break;

  case 144: /* EQUALITY_EXPRESSION: EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION  */
#line 1382 "parser.y"
                                                 {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_EQ, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3436 "parser.tab.c"
    break;

  case 145: /* EQUALITY_EXPRESSION: EQUALITY_EXPRESSION NEQ RELATIONAL_EXPRESSION  */
#line 1392 "parser.y"
                                                    {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_NEQ, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3451 "parser.tab.c"
    break;

  case 146: /* EQUALITY_EXPRESSION: RELATIONAL_EXPRESSION  */
#line 1402 "parser.y"
                            { (yyval.expr) = (yyvsp[0].expr); }
#line 3457 "parser.tab.c"
    break;

  case 147: /* EQUALITY_EXPRESSION: EQUALITY_EXPRESSION EQ error  */
#line 1405 "parser.y"
                                   {
        reportError(SYNTAX_ERROR, "Expected Expression After '=='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3466 "parser.tab.c"
    break;

  case 148: /* EQUALITY_EXPRESSION: EQUALITY_EXPRESSION NEQ error  */
#line 1409 "parser.y"
                                    {
        reportError(SYNTAX_ERROR, "Expected Expression After '!='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3475 "parser.tab.c"
    break;

  case 149: /* EQUALITY_EXPRESSION: EQ error  */
#line 1413 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '==' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3484 "parser.tab.c"
    break;

  case 150: /* EQUALITY_EXPRESSION: NEQ error  */
#line 1417 "parser.y"
                {
        reportError(SYNTAX_ERROR, "Unexpected '!=' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3493 "parser.tab.c"
    break;

  case 151: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION ST ADDITIVE_EXPRESSION  */
#line 1424 "parser.y"
                                                 {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_LT, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3508 "parser.tab.c"
    break;

  case 152: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION GT ADDITIVE_EXPRESSION  */
#line 1434 "parser.y"
                                                   {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_GT, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3523 "parser.tab.c"
    break;

  case 153: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION SE ADDITIVE_EXPRESSION  */
#line 1444 "parser.y"
                                                   {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_LTE, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3538 "parser.tab.c"
    break;

  case 154: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION GE ADDITIVE_EXPRESSION  */
#line 1454 "parser.y"
                                                   {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_GTE, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3553 "parser.tab.c"
    break;

  case 155: /* RELATIONAL_EXPRESSION: ADDITIVE_EXPRESSION  */
#line 1464 "parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 3559 "parser.tab.c"
    break;

  case 156: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION ST error  */
#line 1467 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Expression After '<'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3568 "parser.tab.c"
    break;

  case 157: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION GT error  */
#line 1471 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Expression After '>'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3577 "parser.tab.c"
    break;

  case 158: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION SE error  */
#line 1475 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Expression After '<='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3586 "parser.tab.c"
    break;

  case 159: /* RELATIONAL_EXPRESSION: RELATIONAL_EXPRESSION GE error  */
#line 1479 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Expression After '>='", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3595 "parser.tab.c"
    break;

  case 160: /* RELATIONAL_EXPRESSION: ST error  */
#line 1483 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '<' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3604 "parser.tab.c"
    break;

  case 161: /* RELATIONAL_EXPRESSION: GT error  */
#line 1487 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '>' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3613 "parser.tab.c"
    break;

  case 162: /* RELATIONAL_EXPRESSION: SE error  */
#line 1491 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '<=' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3622 "parser.tab.c"
    break;

  case 163: /* RELATIONAL_EXPRESSION: GE error  */
#line 1495 "parser.y"
               {
        reportError(SYNTAX_ERROR, "Unexpected '>=' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3631 "parser.tab.c"
    break;

  case 164: /* ADDITIVE_EXPRESSION: ADDITIVE_EXPRESSION PLUS MULTIPLICATIVE_EXPRESSION  */
#line 1502 "parser.y"
                                                       {
        type rt = arithmeticResultType((yyvsp[-2].expr)->expressionType, (yyvsp[0].expr)->expressionType);
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_ADD, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(rt, t);
        free(t);
    }
#line 3647 "parser.tab.c"
    break;

  case 165: /* ADDITIVE_EXPRESSION: ADDITIVE_EXPRESSION MINUS MULTIPLICATIVE_EXPRESSION  */
#line 1513 "parser.y"
                                                          {
        type rt = arithmeticResultType((yyvsp[-2].expr)->expressionType, (yyvsp[0].expr)->expressionType);
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_SUB, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(rt, t);
        free(t);
    }
#line 3663 "parser.tab.c"
    break;

  case 166: /* ADDITIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION  */
#line 1524 "parser.y"
                                { (yyval.expr) = (yyvsp[0].expr); }
#line 3669 "parser.tab.c"
    break;

  case 167: /* ADDITIVE_EXPRESSION: ADDITIVE_EXPRESSION PLUS error  */
#line 1527 "parser.y"
                                     {
        reportError(SYNTAX_ERROR, "Expected Expression After '+'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3678 "parser.tab.c"
    break;

  case 168: /* ADDITIVE_EXPRESSION: ADDITIVE_EXPRESSION MINUS error  */
#line 1531 "parser.y"
                                      {
        reportError(SYNTAX_ERROR, "Expected Expression After '-'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3687 "parser.tab.c"
    break;

  case 169: /* ADDITIVE_EXPRESSION: PLUS error  */
#line 1535 "parser.y"
                 {
        reportError(SYNTAX_ERROR, "Unexpected '+' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3696 "parser.tab.c"
    break;

  case 170: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION MULTIPLY EXPONENT_EXPRESSION  */
#line 1542 "parser.y"
                                                           {
        type rt = arithmeticResultType((yyvsp[-2].expr)->expressionType, (yyvsp[0].expr)->expressionType);
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_MUL, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(rt, t);
        free(t);
    }
#line 3712 "parser.tab.c"
    break;

  case 171: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION DIVIDE EXPONENT_EXPRESSION  */
#line 1553 "parser.y"
                                                           {
        type rt = FLOAT_TYPE;
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_DIV, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(rt, t);
        free(t);
    }
#line 3728 "parser.tab.c"
    break;

  case 172: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION MODULO EXPONENT_EXPRESSION  */
#line 1564 "parser.y"
                                                           {
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_MOD, a1, a2, t);
        free(a1);
        free(a2);
        (yyval.expr) = makeTempExpr(INT_TYPE, t);
        free(t);
    }
#line 3743 "parser.tab.c"
    break;

  case 173: /* MULTIPLICATIVE_EXPRESSION: EXPONENT_EXPRESSION  */
#line 1574 "parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 3749 "parser.tab.c"
    break;

  case 174: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION MULTIPLY error  */
#line 1577 "parser.y"
                                               {
        reportError(SYNTAX_ERROR, "Expected Expression After '*'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3758 "parser.tab.c"
    break;

  case 175: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION DIVIDE error  */
#line 1581 "parser.y"
                                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '/'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3767 "parser.tab.c"
    break;

  case 176: /* MULTIPLICATIVE_EXPRESSION: MULTIPLICATIVE_EXPRESSION MODULO error  */
#line 1585 "parser.y"
                                             {
        reportError(SYNTAX_ERROR, "Expected Expression After '%'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3776 "parser.tab.c"
    break;

  case 177: /* MULTIPLICATIVE_EXPRESSION: MULTIPLY error  */
#line 1589 "parser.y"
                     {
        reportError(SYNTAX_ERROR, "Unexpected '*' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3785 "parser.tab.c"
    break;

  case 178: /* MULTIPLICATIVE_EXPRESSION: DIVIDE error  */
#line 1593 "parser.y"
                   {
        reportError(SYNTAX_ERROR, "Unexpected '/' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3794 "parser.tab.c"
    break;

  case 179: /* MULTIPLICATIVE_EXPRESSION: MODULO error  */
#line 1597 "parser.y"
                   {
        reportError(SYNTAX_ERROR, "Unexpected '%' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3803 "parser.tab.c"
    break;

  case 180: /* EXPONENT_EXPRESSION: EXPONENT_EXPRESSION POWER UNARY_EXPRESSION  */
#line 1604 "parser.y"
                                               {
        type resultType = (((yyvsp[-2].expr) && (yyvsp[-2].expr)->expressionType == FLOAT_TYPE) || ((yyvsp[0].expr) && (yyvsp[0].expr)->expressionType == FLOAT_TYPE)) ? FLOAT_TYPE : INT_TYPE;
        char* a1 = exprToOperand((yyvsp[-2].expr));
        char* a2 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_EXP, a1, a2, t);
        value v;
        memset(&v, 0, sizeof(v));
        (yyval.expr) = makeExpr(resultType, v, t);
        free(a1);
        free(a2);
        free(t);
    }
#line 3821 "parser.tab.c"
    break;

  case 181: /* EXPONENT_EXPRESSION: UNARY_EXPRESSION  */
#line 1617 "parser.y"
                       { (yyval.expr) = (yyvsp[0].expr); }
#line 3827 "parser.tab.c"
    break;

  case 182: /* EXPONENT_EXPRESSION: EXPONENT_EXPRESSION POWER error  */
#line 1620 "parser.y"
                                      {
        reportError(SYNTAX_ERROR, "Expected Expression After '**'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3836 "parser.tab.c"
    break;

  case 183: /* EXPONENT_EXPRESSION: POWER error  */
#line 1624 "parser.y"
                  {
        reportError(SYNTAX_ERROR, "Unexpected '**' At Start Of Expression", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3845 "parser.tab.c"
    break;

  case 184: /* UNARY_EXPRESSION: MINUS UNARY_EXPRESSION  */
#line 1631 "parser.y"
                                        {
        char* a1 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_UMIN, a1, NULL, t);
        free(a1);
        (yyval.expr) = makeTempExpr((yyvsp[0].expr)->expressionType, t);
        free(t);
    }
#line 3858 "parser.tab.c"
    break;

  case 185: /* UNARY_EXPRESSION: NOT UNARY_EXPRESSION  */
#line 1639 "parser.y"
                           {
        char* a1 = exprToOperand((yyvsp[0].expr));
        char* t  = createTemp();
        addQuadruple(OP_NOT, a1, NULL, t);
        free(a1);
        (yyval.expr) = makeTempExpr(BOOL_TYPE, t);
        free(t);
    }
#line 3871 "parser.tab.c"
    break;

  case 186: /* UNARY_EXPRESSION: PRIMARY_EXPRESSION  */
#line 1647 "parser.y"
                         { (yyval.expr) = (yyvsp[0].expr); }
#line 3877 "parser.tab.c"
    break;

  case 187: /* UNARY_EXPRESSION: NOT error  */
#line 1650 "parser.y"
                {
        reportError(SYNTAX_ERROR, "Expected Expression After '!'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3886 "parser.tab.c"
    break;

  case 188: /* UNARY_EXPRESSION: MINUS error  */
#line 1654 "parser.y"
                               {
        reportError(SYNTAX_ERROR, "Expected Expression After Unary '-'", (yylsp[-1]).first_line); // DONE
        yyerrok;
    }
#line 3895 "parser.tab.c"
    break;

  case 189: /* PRIMARY_EXPRESSION: INTVALUE  */
#line 1661 "parser.y"
             {
        value v;
        memset(&v, 0, sizeof(v));
        v.INT_Data = (yyvsp[0].intData);
        (yyval.expr) = makeExpr(INT_TYPE, v, NULL);
    }
#line 3906 "parser.tab.c"
    break;

  case 190: /* PRIMARY_EXPRESSION: FLOATVALUE  */
#line 1667 "parser.y"
                 {
        value v;
        memset(&v, 0, sizeof(v));
        v.FLOAT_Data = (yyvsp[0].floatData);
        (yyval.expr) = makeExpr(FLOAT_TYPE, v, NULL);
    }
#line 3917 "parser.tab.c"
    break;

  case 191: /* PRIMARY_EXPRESSION: CHARVALUE  */
#line 1673 "parser.y"
                {
        value v;
        memset(&v, 0, sizeof(v));
        v.CHAR_Data = (yyvsp[0].charData);
        (yyval.expr) = makeExpr(CHAR_TYPE, v, NULL);
    }
#line 3928 "parser.tab.c"
    break;

  case 192: /* PRIMARY_EXPRESSION: BOOLVALUE  */
#line 1679 "parser.y"
                {
        value v;
        memset(&v, 0, sizeof(v));
        v.BOOL_Data = (yyvsp[0].intData);
        (yyval.expr) = makeExpr(BOOL_TYPE, v, NULL);
    }
#line 3939 "parser.tab.c"
    break;

  case 193: /* PRIMARY_EXPRESSION: STRINGVALUE  */
#line 1685 "parser.y"
                  {
        value v;
        memset(&v, 0, sizeof(v));
        v.STRING_Data = strdup((yyvsp[0].stringData));
        (yyval.expr) = makeExpr(STRING_TYPE, v, NULL);
    }
#line 3950 "parser.tab.c"
    break;

  case 194: /* PRIMARY_EXPRESSION: LEFT_ROUND_BRACKET LOGICAL_EXPRESSION RIGHT_ROUND_BRACKET  */
#line 1691 "parser.y"
                                                                {
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 3958 "parser.tab.c"
    break;

  case 195: /* PRIMARY_EXPRESSION: IDENTIFIER PRIMARY_SUFFIX  */
#line 1694 "parser.y"
                                {
        if ((yyvsp[0].argList) != NULL) {
            singleEntryNode* fn = functionLookup(&gScopeStack, (yyvsp[-1].stringData));
            if (!fn || fn->varOrFunc != SYMBOL_FUNCTION) {
                reportError(SEMANTIC_ERROR, "Call to undeclared function", (yylsp[-1]).first_line);
                (yyval.expr) = makeTempExpr(INT_TYPE, (yyvsp[-1].stringData));
            }
            else {
                fn->isUsed = true;
                ArgNode* a = (yyvsp[0].argList);
                while (a) {
                    char* op = exprToOperand(a->expr);
                    addQuadruple(OP_PARM, op, NULL, NULL);
                    free(op);
                    a = a->next;
                }
                if (fn->identifierType != VOID_TYPE) {
                    char* t = createTemp();
                    addQuadruple(OP_CALL, (yyvsp[-1].stringData), NULL, t);
                    (yyval.expr) = makeTempExpr(fn->identifierType, t);
                    free(t);
                }
                else {
                    addQuadruple(OP_CALL, (yyvsp[-1].stringData), NULL, NULL);
                    value v;
                    memset(&v, 0, sizeof(v));
                    (yyval.expr) = makeExpr(VOID_TYPE, v, NULL);
                }
            }
            freeArgList((yyvsp[0].argList));
        }
        else {
            singleEntryNode* e = lookupAllScopes(&gScopeStack, (yyvsp[-1].stringData));
            if (!e) {
                reportError(SEMANTIC_ERROR, "Use of undeclared identifier", (yylsp[-1]).first_line);
                value v;
                memset(&v, 0, sizeof(v));
                (yyval.expr) = makeExpr(INT_TYPE, v, (yyvsp[-1].stringData));
            }
            else 
            {
                (yyval.expr) = makeExpr(e->identifierType, e->currentValue, (yyvsp[-1].stringData));
                e->isUsed = true;
            }
        }
    }
#line 4009 "parser.tab.c"
    break;

  case 196: /* PRIMARY_EXPRESSION: LEFT_ROUND_BRACKET error RIGHT_ROUND_BRACKET  */
#line 1742 "parser.y"
                                                   {
        reportError(SYNTAX_ERROR, "Expected Expression Inside '( )'", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.expr) = NULL;
    }
#line 4019 "parser.tab.c"
    break;

  case 197: /* PRIMARY_EXPRESSION: LEFT_ROUND_BRACKET LOGICAL_EXPRESSION error  */
#line 1747 "parser.y"
                                                  {
        reportError(SYNTAX_ERROR, "Expected ')'", (yylsp[-1]).first_line); // DONE
        yyerrok;
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 4029 "parser.tab.c"
    break;

  case 198: /* PRIMARY_SUFFIX: %empty  */
#line 1755 "parser.y"
              { (yyval.argList) = NULL; }
#line 4035 "parser.tab.c"
    break;

  case 199: /* PRIMARY_SUFFIX: LEFT_ROUND_BRACKET ARGUMENT_LIST RIGHT_ROUND_BRACKET  */
#line 1756 "parser.y"
                                                           { (yyval.argList) = (yyvsp[-1].argList); }
#line 4041 "parser.tab.c"
    break;


#line 4045 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1759 "parser.y"


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
