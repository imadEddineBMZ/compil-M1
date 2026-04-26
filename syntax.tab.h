
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* "%code requires" blocks.  */

/* Line 1676 of yacc.c  */
#line 1 "syntax.y"

#ifndef PROLANG_SHARED_TYPES_H
#define PROLANG_SHARED_TYPES_H

#define MAX_NAME_LEN 64

typedef enum {
    TYPE_INVALID = -1,
    TYPE_INTEGER = 0,
    TYPE_FLOAT = 1,
    TYPE_BOOL = 2
} DataType;

typedef struct {
    char place[MAX_NAME_LEN];
    DataType type;
    int valid;
} ExprAttr;

typedef struct {
    char name[MAX_NAME_LEN];
    DataType type;
    int valid;
    int is_const;
    int is_array_elem;
    char index_place[MAX_NAME_LEN];
} LValueAttr;

#endif



/* Line 1676 of yacc.c  */
#line 73 "syntax.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDF = 258,
     CONST_INTEGER = 259,
     CONST_FLOAT = 260,
     STRING = 261,
     MC_BEGIN_PROJECT = 262,
     MC_END_PROJECT = 263,
     MC_SETUP = 264,
     MC_RUN = 265,
     MC_DEFINE = 266,
     MC_CONST = 267,
     MC_TYPE_INTEGER = 268,
     MC_TYPE_FLOAT = 269,
     MC_IF = 270,
     MC_THEN = 271,
     MC_ELSE = 272,
     MC_ENDIF = 273,
     MC_LOOP = 274,
     MC_WHILE = 275,
     MC_ENDLOOP = 276,
     MC_FOR = 277,
     MC_IN = 278,
     MC_TO = 279,
     MC_ENDFOR = 280,
     MC_OUT = 281,
     OP_AND = 282,
     OP_OR = 283,
     OP_NON = 284,
     OP_GE = 285,
     OP_LE = 286,
     OP_EQ = 287,
     OP_NE = 288,
     OP_GT = 289,
     OP_LT = 290,
     OP_PLUS = 291,
     OP_MINUS = 292,
     OP_MULT = 293,
     OP_DIV = 294,
     OP_AFFECT = 295,
     SEP_EQUAL = 296,
     SEP_SEMICOLON = 297,
     SEP_COLON = 298,
     SEP_COMMA = 299,
     SEP_PIPE = 300,
     SEP_LBRACE = 301,
     SEP_RBRACE = 302,
     SEP_LBRACKET = 303,
     SEP_RBRACKET = 304,
     SEP_LPAREN = 305,
     SEP_RPAREN = 306
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 639 "syntax.y"

    char* str;
    int ival;
    ExprAttr expr;
    LValueAttr lval;



/* Line 1676 of yacc.c  */
#line 150 "syntax.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


