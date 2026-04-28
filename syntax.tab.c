
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 32 "syntax.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int yylex(void);
void yyerror(const char* s);
extern FILE* yyin;
extern int line_num;
extern int prev_col;

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

#define SEM_HASH_SIZE 211
#define MAX_PENDING_IDS 128
#define MAX_QUADS 2000
#define MAX_STACK 256

typedef enum {
    SYM_VAR = 0,
    SYM_CONST = 1,
    SYM_ARRAY = 2,
    SYM_TEMP = 3
} SymbolKind;

typedef struct SemSymbol {
    char name[MAX_NAME_LEN];
    SymbolKind kind;
    DataType type;
    int array_size;
    int initialized;
    struct SemSymbol* next;
} SemSymbol;

typedef struct {
    char op[16];
    char arg1[MAX_NAME_LEN];
    char arg2[MAX_NAME_LEN];
    char res[MAX_NAME_LEN];
} Quad;

typedef struct {
    int bz_index;// branch si oui  
    int br_index;// sauti 
    int has_else;// a un bloc else
} IfContext;

typedef struct {
    int start_index;
    int bz_index;
} WhileContext;

typedef struct {
    char iter_name[MAX_NAME_LEN];
    DataType iter_type;
    int start_index;
    int bz_index;
} ForContext;

enum {
    CMP_GT = 1,
    CMP_LT,
    CMP_GE,
    CMP_LE,
    CMP_EQ,
    CMP_NE
};

static SemSymbol* semtab[SEM_HASH_SIZE];
static Quad quads[MAX_QUADS];
static int quad_count = 0;
static int temp_count = 0;
static int semantic_errors = 0;

static char pending_ids[MAX_PENDING_IDS][MAX_NAME_LEN];
static int pending_ids_count = 0;
static int define_has_init = 0;
static ExprAttr define_init_value;

static IfContext if_stack[MAX_STACK];
static int if_top = -1;

static WhileContext while_stack[MAX_STACK];
static int while_top = -1;

static ForContext for_stack[MAX_STACK];
static int for_top = -1;

static unsigned int sem_hash(const char* s) {
    unsigned int h = 0;
    while (*s) {
        h = (h * 31u) + (unsigned char)(*s++);
    }
    return h % SEM_HASH_SIZE;
}

static const char* type_to_str(DataType t) {
    switch (t) {
        case TYPE_INTEGER: return "integer";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        default: return "invalid";
    }
}

static void semantic_error(const char* entity, const char* fmt, ...) {
    va_list args;
    char msg[256];

    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    fprintf(stderr,
            "Erreur_semantique, ligne %d, colonne %d, entite %s: %s\n",
            line_num,
            prev_col,
            (entity && entity[0] != '\0') ? entity : "?",
            msg);
    semantic_errors++;
}

static int is_numeric(DataType t) {
    return (t == TYPE_INTEGER || t == TYPE_FLOAT);
}

static int can_assign(DataType dst, DataType src) {
    if (dst == TYPE_FLOAT && src == TYPE_INTEGER) {
        return 1;
    }
    return dst == src;
}

static SemSymbol* sem_lookup(const char* name) {// nhowsso ida yesxisti deja  
    unsigned int idx = sem_hash(name);
    SemSymbol* cur = semtab[idx];

    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

static SemSymbol* sem_insert(const char* name, SymbolKind kind, DataType type, int array_size, int initialized) {// insertion dans la table de symbole  
    unsigned int idx;
    SemSymbol* s;

    if (sem_lookup(name) != NULL) {
        semantic_error(name, "identificateur deja declare");
        return NULL;
    }

    idx = sem_hash(name);
    s = (SemSymbol*)malloc(sizeof(SemSymbol));
    if (!s) {
        fprintf(stderr, "Erreur memoire\n");
        exit(1);
    }

    strncpy(s->name, name, MAX_NAME_LEN - 1);
    s->name[MAX_NAME_LEN - 1] = '\0';
    s->kind = kind;
    s->type = type;
    s->array_size = array_size;
    s->initialized = initialized;
    s->next = semtab[idx];
    semtab[idx] = s;
    return s;
}

static void free_sem_table(void) {
    int i;
    SemSymbol* cur;
    SemSymbol* nxt;

    for (i = 0; i < SEM_HASH_SIZE; i++) {
        cur = semtab[i];
        while (cur) {
            nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        semtab[i] = NULL;
    }
}

static ExprAttr make_invalid_expr(void) {
    ExprAttr e;
    e.place[0] = '\0';
    e.type = TYPE_INVALID;
    e.valid = 0;
    return e;
}

static ExprAttr make_expr(const char* place, DataType type) {
    ExprAttr e;
    strncpy(e.place, place, MAX_NAME_LEN - 1);
    e.place[MAX_NAME_LEN - 1] = '\0';
    e.type = type;
    e.valid = 1;
    return e;
}

static LValueAttr make_invalid_lvalue(void) {
    LValueAttr l;
    l.name[0] = '\0';
    l.type = TYPE_INVALID;
    l.valid = 0;
    l.is_const = 0;
    l.is_array_elem = 0;
    l.index_place[0] = '\0';
    return l;
}

static int emit_quad(const char* op, const char* arg1, const char* arg2, const char* res) {
    int idx = quad_count;

    if (quad_count >= MAX_QUADS) {
        fprintf(stderr, "Erreur: trop de quadruplets\n");
        exit(1);
    }

    strncpy(quads[idx].op, op ? op : "", sizeof(quads[idx].op) - 1);
    quads[idx].op[sizeof(quads[idx].op) - 1] = '\0';

    strncpy(quads[idx].arg1, arg1 ? arg1 : "", sizeof(quads[idx].arg1) - 1);
    quads[idx].arg1[sizeof(quads[idx].arg1) - 1] = '\0';

    strncpy(quads[idx].arg2, arg2 ? arg2 : "", sizeof(quads[idx].arg2) - 1);
    quads[idx].arg2[sizeof(quads[idx].arg2) - 1] = '\0';

    strncpy(quads[idx].res, res ? res : "", sizeof(quads[idx].res) - 1);
    quads[idx].res[sizeof(quads[idx].res) - 1] = '\0';

    quad_count++;
    return idx;
}

static int next_quad(void) {//pour les boucles 
    return quad_count;
}

static void patch_quad_target(int index, int target) {
    char buff[32];

    if (index < 0 || index >= quad_count) {
        return;
    }

    snprintf(buff, sizeof(buff), "%d", target);
    strncpy(quads[index].res, buff, sizeof(quads[index].res) - 1);
    quads[index].res[sizeof(quads[index].res) - 1] = '\0';
}

static ExprAttr new_temp(DataType t) {
    char name[MAX_NAME_LEN];

    snprintf(name, sizeof(name), "_t%d", temp_count++);
    sem_insert(name, SYM_TEMP, t, 0, 1);
    return make_expr(name, t);// tretourner une ex t1= b1 + b2
}

static void print_quads(void) {
    int i;

    printf("\n================ QUADRUPLETS ================\n");
    printf("%-6s %-10s %-14s %-14s %-14s\n", "Index", "Op", "Arg1", "Arg2", "Res");
    printf("---------------------------------------------------------------\n");

    for (i = 0; i < quad_count; i++) {
        printf("%-6d %-10s %-14s %-14s %-14s\n",
               i,
               quads[i].op,
               quads[i].arg1,
               quads[i].arg2,
               quads[i].res);
    }
}

static void pending_reset(void) {
    pending_ids_count = 0;
}

static void pending_add(const char* id) {// crier une lisset temp des idf dans une dec muliple 
    int i;

    for (i = 0; i < pending_ids_count; i++) {
        if (strcmp(pending_ids[i], id) == 0) {
            semantic_error(id, "identificateur duplique dans la meme declaration");
            return;
        }
    }

    if (pending_ids_count >= MAX_PENDING_IDS) {
        semantic_error(id, "trop d'identificateurs dans une declaration");
        return;
    }

    strncpy(pending_ids[pending_ids_count], id, MAX_NAME_LEN - 1);
    pending_ids[pending_ids_count][MAX_NAME_LEN - 1] = '\0';
    pending_ids_count++;
}

static void begin_define_decl(const char* first_id) {
    pending_reset();
    pending_add(first_id);
    define_has_init = 0;
    define_init_value = make_invalid_expr();
}

static void commit_define_scalar(DataType type) {// commiter la declaration d'une variable simple
    int i;
    SemSymbol* s;

    for (i = 0; i < pending_ids_count; i++) {
        s = sem_insert(pending_ids[i], SYM_VAR, type, 0, define_has_init ? 1 : 0);

        if (define_has_init && s) {
            if (!can_assign(type, define_init_value.type)) {
                semantic_error(pending_ids[i],
                               "incompatibilite de type a l'initialisation (%s <- %s)",
                               type_to_str(type),
                               type_to_str(define_init_value.type));
            } else {
                emit_quad("=", define_init_value.place, "", pending_ids[i]);
            }
        }
    }

    pending_reset();
    define_has_init = 0;
    define_init_value = make_invalid_expr();
}

static int parse_integer_literal(const char* text, int* out) {
    char buff[64];
    char* endptr;
    long v;

    if (!text || !out) {
        return 0;
    }

    if (text[0] == '(') {
        size_t n = strlen(text);
        if (n < 3 || text[n - 1] != ')') {
            return 0;
        }

        if (n - 2 >= sizeof(buff)) {
            return 0;
        }

        strncpy(buff, text + 1, n - 2);
        buff[n - 2] = '\0';
        v = strtol(buff, &endptr, 10);
    } else {
        v = strtol(text, &endptr, 10);
    }

    if (*endptr != '\0') {
        return 0;
    }

    *out = (int)v;
    return 1;
}

static void commit_define_array(DataType type, const char* size_text) {// commiter la declaration d'un tableau
    int size;

    if (pending_ids_count != 1) {
        semantic_error("define", "declaration de tableau invalide");
        pending_reset();
        return;
    }

    if (!parse_integer_literal(size_text, &size) || size <= 0) {
        semantic_error(pending_ids[0], "taille du tableau doit etre une constante entiere positive");
        pending_reset();
        return;
    }

    sem_insert(pending_ids[0], SYM_ARRAY, type, size, 1);
    pending_reset();
}

static void declare_constant(const char* name, DataType type, ExprAttr lit) {//
    SemSymbol* s = sem_insert(name, SYM_CONST, type, 0, 1);

    if (!s) {
        return;
    }

    if (!can_assign(type, lit.type)) {
        semantic_error(name,
                       "incompatibilite de type pour constante (%s <- %s)",
                       type_to_str(type),
                       type_to_str(lit.type));
        return;
    }

    emit_quad("=", lit.place, "", name);
}

static ExprAttr build_binary_numeric(const char* op, ExprAttr a, ExprAttr b) {
    ExprAttr r;

    if (!a.valid || !b.valid) {
        return make_invalid_expr();
    }

    if (!is_numeric(a.type) || !is_numeric(b.type)) {
        semantic_error(op, "operandes non numeriques");
        return make_invalid_expr();
    }

    r = new_temp((a.type == TYPE_FLOAT || b.type == TYPE_FLOAT) ? TYPE_FLOAT : TYPE_INTEGER);
    emit_quad(op, a.place, b.place, r.place);
    return r;
}

static const char* cmp_to_quad_op(int cmp) {
    switch (cmp) {
        case CMP_GT: return "GT";
        case CMP_LT: return "LT";
        case CMP_GE: return "GE";
        case CMP_LE: return "LE";
        case CMP_EQ: return "EQ";
        case CMP_NE: return "NE";
        default: return "EQ";
    }
}

static ExprAttr build_comparison(ExprAttr a, int cmp, ExprAttr b) {
    ExprAttr r;

    if (!a.valid || !b.valid) {
        return make_invalid_expr();
    }

    if (!is_numeric(a.type) || !is_numeric(b.type)) {
        semantic_error("condition", "comparaison entre operandes non numeriques");
        return make_invalid_expr();
    }

    r = new_temp(TYPE_BOOL);
    emit_quad(cmp_to_quad_op(cmp), a.place, b.place, r.place);
    return r;
}

static ExprAttr build_logic(const char* op, ExprAttr a, ExprAttr b) {
    ExprAttr r;

    if (!a.valid || !b.valid) {
        return make_invalid_expr();
    }

    if (a.type != TYPE_BOOL || b.type != TYPE_BOOL) {
        semantic_error(op, "operandes logiques doivent etre booleens");
        return make_invalid_expr();
    }

    r = new_temp(TYPE_BOOL);
    emit_quad(op, a.place, b.place, r.place);
    return r;
}

static ExprAttr build_not(ExprAttr a) {
    ExprAttr r;

    if (!a.valid) {
        return make_invalid_expr();
    }

    if (a.type != TYPE_BOOL) {
        semantic_error("NON", "operande de NON doit etre booleen");
        return make_invalid_expr();
    }

    r = new_temp(TYPE_BOOL);
    emit_quad("NOT", a.place, "", r.place);
    return r;
}

static void push_if_context(int bz_index) {// empiler le contexte d'un if pour gerer les branches et les else
    if (if_top + 1 >= MAX_STACK) {
        fprintf(stderr, "Erreur interne: pile if pleine\n");
        exit(1);
    }

    if_top++;
    if_stack[if_top].bz_index = bz_index;
    if_stack[if_top].br_index = -1;
    if_stack[if_top].has_else = 0;
}

static IfContext* current_if_context(void) {
    if (if_top < 0) {
        return NULL;
    }
    return &if_stack[if_top];
}

static IfContext pop_if_context(void) {//
    IfContext c;

    if (if_top < 0) {
        c.bz_index = -1;
        c.br_index = -1;
        c.has_else = 0;
        return c;
    }

    c = if_stack[if_top];
    if_top--;
    return c;
}

static void push_while_context(int start_idx, int bz_idx) {// empiler le contexte d'une boucle while pour gerer les sauts de retour et les branches de fin de boucle
    if (while_top + 1 >= MAX_STACK) {
        fprintf(stderr, "Erreur interne: pile while pleine\n");
        exit(1);
    }

    while_top++;
    while_stack[while_top].start_index = start_idx;
    while_stack[while_top].bz_index = bz_idx;
}

static WhileContext pop_while_context(void) {
    WhileContext c;

    if (while_top < 0) {
        c.start_index = -1;
        c.bz_index = -1;
        return c;
    }

    c = while_stack[while_top];
    while_top--;
    return c;
}

static void push_for_context(const char* iter_name, DataType iter_type, int start_idx, int bz_idx) {
    if (for_top + 1 >= MAX_STACK) {
        fprintf(stderr, "Erreur interne: pile for pleine\n");
        exit(1);
    }

    for_top++;
    strncpy(for_stack[for_top].iter_name, iter_name, MAX_NAME_LEN - 1);
    for_stack[for_top].iter_name[MAX_NAME_LEN - 1] = '\0';
    for_stack[for_top].iter_type = iter_type;
    for_stack[for_top].start_index = start_idx;
    for_stack[for_top].bz_index = bz_idx;
}

static ForContext pop_for_context(void) {
    ForContext c;

    if (for_top < 0) {
        c.iter_name[0] = '\0';
        c.iter_type = TYPE_INVALID;
        c.start_index = -1;
        c.bz_index = -1;
        return c;
    }

    c = for_stack[for_top];
    for_top--;
    return c;
}



/* Line 189 of yacc.c  */
#line 681 "syntax.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
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



/* Line 209 of yacc.c  */
#line 738 "syntax.tab.c"

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

/* Line 214 of yacc.c  */
#line 639 "syntax.y"

    char* str;
    int ival;
    ExprAttr expr;
    LValueAttr lval;



/* Line 214 of yacc.c  */
#line 815 "syntax.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 827 "syntax.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   159

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNRULES -- Number of states.  */
#define YYNSTATES  159

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,    15,    16,    19,    20,    25,    33,    39,
      44,    52,    54,    58,    59,    62,    64,    66,    68,    70,
      74,    75,    78,    81,    84,    87,    90,    93,    96,   100,
     102,   107,   108,   119,   120,   121,   125,   126,   127,   137,
     138,   148,   153,   158,   160,   164,   166,   168,   172,   174,
     178,   180,   185,   187,   191,   195,   197,   199,   201,   203,
     205,   207,   211,   215,   217,   221,   225,   227,   229,   234,
     236
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,     7,     3,    42,     9,    43,    54,    10,
      43,    62,     8,    42,    -1,    -1,    54,    55,    -1,    -1,
      11,     3,    56,    57,    -1,    12,     3,    43,    60,    41,
      61,    42,    -1,    45,    58,    43,    60,    42,    -1,    43,
      60,    59,    42,    -1,    43,    48,    60,    42,     4,    49,
      42,    -1,     3,    -1,    58,    45,     3,    -1,    -1,    41,
      61,    -1,    13,    -1,    14,    -1,     4,    -1,     5,    -1,
      46,    63,    47,    -1,    -1,    63,    64,    -1,    65,    42,
      -1,    67,    42,    -1,    72,    42,    -1,    74,    42,    -1,
      76,    42,    -1,    77,    42,    -1,    66,    40,    85,    -1,
       3,    -1,     3,    48,    85,    49,    -1,    -1,    15,    50,
      80,    51,    16,    43,    68,    62,    69,    18,    -1,    -1,
      -1,    17,    70,    62,    -1,    -1,    -1,    19,    20,    71,
      50,    80,    51,    73,    62,    21,    -1,    -1,    22,     3,
      23,    85,    24,    85,    75,    62,    25,    -1,    23,    50,
       3,    51,    -1,    26,    50,    78,    51,    -1,    79,    -1,
      78,    44,    79,    -1,     6,    -1,    85,    -1,    80,    28,
      81,    -1,    81,    -1,    81,    27,    82,    -1,    82,    -1,
      29,    50,    80,    51,    -1,    83,    -1,    50,    80,    51,
      -1,    85,    84,    85,    -1,    34,    -1,    35,    -1,    30,
      -1,    31,    -1,    32,    -1,    33,    -1,    85,    36,    86,
      -1,    85,    37,    86,    -1,    86,    -1,    86,    38,    87,
      -1,    86,    39,    87,    -1,    87,    -1,     3,    -1,     3,
      48,    85,    49,    -1,    61,    -1,    50,    85,    51,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   671,   671,   686,   688,   693,   692,   698,   707,   711,
     715,   723,   728,   737,   741,   749,   753,   760,   765,   773,
     776,   778,   782,   783,   784,   785,   786,   787,   791,   813,
     835,   866,   865,   883,   891,   890,   904,   911,   910,   935,
     934,   988,  1007,  1011,  1012,  1016,  1021,  1030,  1034,  1041,
    1045,  1052,  1056,  1063,  1067,  1074,  1078,  1082,  1086,  1090,
    1094,  1101,  1105,  1109,  1116,  1120,  1124,  1131,  1147,  1168,
    1172
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDF", "CONST_INTEGER", "CONST_FLOAT",
  "STRING", "MC_BEGIN_PROJECT", "MC_END_PROJECT", "MC_SETUP", "MC_RUN",
  "MC_DEFINE", "MC_CONST", "MC_TYPE_INTEGER", "MC_TYPE_FLOAT", "MC_IF",
  "MC_THEN", "MC_ELSE", "MC_ENDIF", "MC_LOOP", "MC_WHILE", "MC_ENDLOOP",
  "MC_FOR", "MC_IN", "MC_TO", "MC_ENDFOR", "MC_OUT", "OP_AND", "OP_OR",
  "OP_NON", "OP_GE", "OP_LE", "OP_EQ", "OP_NE", "OP_GT", "OP_LT",
  "OP_PLUS", "OP_MINUS", "OP_MULT", "OP_DIV", "OP_AFFECT", "SEP_EQUAL",
  "SEP_SEMICOLON", "SEP_COLON", "SEP_COMMA", "SEP_PIPE", "SEP_LBRACE",
  "SEP_RBRACE", "SEP_LBRACKET", "SEP_RBRACKET", "SEP_LPAREN", "SEP_RPAREN",
  "$accept", "program", "declarations", "declaration", "$@1",
  "define_rest", "id_list_tail", "define_init_opt", "type", "literal",
  "block", "statements", "statement", "assign_stmt", "lvalue", "if_stmt",
  "$@2", "opt_else", "$@3", "while_mark", "while_stmt", "$@4", "for_stmt",
  "$@5", "input_stmt", "output_stmt", "output_args", "output_arg",
  "condition", "condition_and", "condition_not", "condition_atom",
  "comp_op", "expr", "term", "factor", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    53,    54,    54,    56,    55,    55,    57,    57,
      57,    58,    58,    59,    59,    60,    60,    61,    61,    62,
      63,    63,    64,    64,    64,    64,    64,    64,    65,    66,
      66,    68,    67,    69,    70,    69,    71,    73,    72,    75,
      74,    76,    77,    78,    78,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    84,    84,    84,
      84,    85,    85,    85,    86,    86,    86,    87,    87,    87,
      87
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,    11,     0,     2,     0,     4,     7,     5,     4,
       7,     1,     3,     0,     2,     1,     1,     1,     1,     3,
       0,     2,     2,     2,     2,     2,     2,     2,     3,     1,
       4,     0,    10,     0,     0,     3,     0,     0,     9,     0,
       9,     4,     4,     1,     3,     1,     1,     3,     1,     3,
       1,     4,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     3,     3,     1,     1,     4,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,     0,     0,     3,     0,     0,
       0,     0,     4,     0,     5,     0,    20,     0,     0,     0,
       0,     0,     0,     0,     6,    15,    16,     0,    29,     0,
       0,     0,     0,     0,    19,    21,     0,     0,     0,     0,
       0,     0,     0,     2,     0,    13,    11,     0,     0,     0,
       0,    36,     0,     0,     0,    22,     0,    23,    24,    25,
      26,    27,     0,     0,     0,     0,     0,    17,    18,     0,
      67,     0,    69,     0,    63,    66,     0,     0,     0,    48,
      50,    52,     0,     0,     0,     0,    45,     0,    43,    46,
      28,     0,    14,     9,     0,    12,     7,     0,     0,     0,
       0,    30,     0,     0,     0,     0,     0,     0,     0,     0,
      57,    58,    59,    60,    55,    56,     0,     0,     0,    41,
       0,    42,     0,     8,     0,    70,    61,    62,    64,    65,
       0,    53,    47,     0,    49,    54,     0,     0,    44,     0,
      68,    51,    31,    37,    39,    10,     0,     0,     0,    33,
       0,     0,    34,     0,    38,    40,     0,    32,    35
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     8,    12,    18,    24,    47,    64,    27,    72,
      17,    20,    35,    36,    37,    38,   146,   153,   156,    83,
      39,   147,    40,   148,    41,    42,    87,    88,    78,    79,
      80,    81,   116,    82,    74,    75
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int16 yypact[] =
{
      19,    39,    45,    -3,   -65,    48,    27,   -65,    79,    60,
     104,   107,   -65,    65,   -65,    71,   -65,   108,   -18,    46,
      14,    73,     6,   114,   -65,   -65,   -65,    77,    72,    74,
      99,   118,    75,    76,   -65,   -65,    80,    83,    85,    86,
      87,    88,    89,   -65,    46,    91,   -65,    59,    61,    -1,
       5,   -65,   110,   131,     8,   -65,    -1,   -65,   -65,   -65,
     -65,   -65,    93,    61,    94,    46,   134,   -65,   -65,    96,
      92,    -1,   -65,    15,    47,   -65,    95,     5,   -13,   112,
     -65,   -65,    64,    97,    -1,    90,   -65,    36,   -65,    69,
      69,   138,   -65,   -65,   101,   -65,   -65,    -1,    -5,    -1,
      -1,   -65,    -1,    -1,     5,   -10,    42,     5,   128,     5,
     -65,   -65,   -65,   -65,   -65,   -65,    -1,     5,    26,   -65,
       8,   -65,   100,   -65,    32,   -65,    47,    47,   -65,   -65,
      -4,   -65,   112,   103,   -65,    69,     2,    -1,   -65,   106,
     -65,   -65,   -65,   -65,    69,   -65,    65,    65,    65,   133,
     130,   127,   -65,   135,   -65,   -65,    65,   -65,   -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -21,   -42,
     -64,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,    34,   -61,    49,
      50,   -65,   -65,   -49,     9,    10
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      73,    45,    70,    67,    68,    89,    69,    90,    70,    67,
      68,    70,    67,    68,    86,   107,   105,    28,   107,    25,
      26,    92,    98,    62,   107,    22,     1,    23,   106,    29,
     107,    99,   100,    30,    76,   118,    31,    32,   108,     5,
      33,   131,     3,   130,    94,     4,   125,   141,   124,    71,
     137,    99,   100,   143,    44,    77,   136,     6,    71,    25,
      26,    34,    99,   100,   101,    67,    68,   135,    99,   100,
       7,    89,   110,   111,   112,   113,   114,   115,    99,   100,
     120,   140,   149,   150,   151,   102,   103,   121,   144,     9,
      10,    11,   158,   125,   110,   111,   112,   113,   114,   115,
      99,   100,    65,    13,    66,    99,   100,    14,   126,   127,
      15,    16,   128,   129,    19,    43,    21,    46,    48,    51,
      49,    52,    55,    56,    50,    53,    54,    57,    58,    59,
      60,    61,    63,    84,    85,    91,    93,    95,    96,   109,
      97,   119,   122,   123,   133,   104,   142,   117,   145,   139,
     152,   154,   155,   157,   138,     0,   132,     0,     0,   134
};

static const yytype_int16 yycheck[] =
{
      49,    22,     3,     4,     5,    54,    48,    56,     3,     4,
       5,     3,     4,     5,     6,    28,    77,     3,    28,    13,
      14,    63,    71,    44,    28,    43,     7,    45,    77,    15,
      28,    36,    37,    19,    29,    84,    22,    23,    51,    42,
      26,    51,     3,   104,    65,     0,    51,    51,    97,    50,
      24,    36,    37,    51,    48,    50,   117,     9,    50,    13,
      14,    47,    36,    37,    49,     4,     5,   116,    36,    37,
      43,   120,    30,    31,    32,    33,    34,    35,    36,    37,
      44,    49,   146,   147,   148,    38,    39,    51,   137,    10,
      11,    12,   156,    51,    30,    31,    32,    33,    34,    35,
      36,    37,    43,    43,    45,    36,    37,     3,    99,   100,
       3,    46,   102,   103,    43,    42,     8,     3,    41,    20,
      48,     3,    42,    40,    50,    50,    50,    42,    42,    42,
      42,    42,    41,    23,     3,    42,    42,     3,    42,    27,
      48,    51,     4,    42,    16,    50,    43,    50,    42,    49,
      17,    21,    25,    18,   120,    -1,   107,    -1,    -1,   109
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    53,     3,     0,    42,     9,    43,    54,    10,
      11,    12,    55,    43,     3,     3,    46,    62,    56,    43,
      63,     8,    43,    45,    57,    13,    14,    60,     3,    15,
      19,    22,    23,    26,    47,    64,    65,    66,    67,    72,
      74,    76,    77,    42,    48,    60,     3,    58,    41,    48,
      50,    20,     3,    50,    50,    42,    40,    42,    42,    42,
      42,    42,    60,    41,    59,    43,    45,     4,     5,    61,
       3,    50,    61,    85,    86,    87,    29,    50,    80,    81,
      82,    83,    85,    71,    23,     3,     6,    78,    79,    85,
      85,    42,    61,    42,    60,     3,    42,    48,    85,    36,
      37,    49,    38,    39,    50,    80,    85,    28,    51,    27,
      30,    31,    32,    33,    34,    35,    84,    50,    85,    51,
      44,    51,     4,    42,    85,    51,    86,    86,    87,    87,
      80,    51,    81,    16,    82,    85,    80,    24,    79,    49,
      49,    51,    43,    51,    85,    42,    68,    73,    75,    62,
      62,    62,    17,    69,    21,    25,    70,    18,    62
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 675 "syntax.y"
    {
          if (semantic_errors == 0) {
              printf("Analyse syntaxique et semantique reussie.\n");
          } else {
              printf("Analyse syntaxique terminee avec %d erreur(s) semantiques.\n", semantic_errors);
          }
          print_quads();
          free((yyvsp[(2) - (11)].str));
      ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 693 "syntax.y"
    {
          begin_define_decl((yyvsp[(2) - (2)].str));
          free((yyvsp[(2) - (2)].str));
      ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 699 "syntax.y"
    {
          declare_constant((yyvsp[(2) - (7)].str), (DataType)(yyvsp[(4) - (7)].ival), (yyvsp[(6) - (7)].expr));
          free((yyvsp[(2) - (7)].str));
      ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 708 "syntax.y"
    {
          commit_define_scalar((DataType)(yyvsp[(4) - (5)].ival));
      ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 712 "syntax.y"
    {
          commit_define_scalar((DataType)(yyvsp[(2) - (4)].ival));
      ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 716 "syntax.y"
    {
          commit_define_array((DataType)(yyvsp[(3) - (7)].ival), (yyvsp[(5) - (7)].str));
          free((yyvsp[(5) - (7)].str));
      ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 724 "syntax.y"
    {
          pending_add((yyvsp[(1) - (1)].str));
          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 729 "syntax.y"
    {
          pending_add((yyvsp[(3) - (3)].str));
          free((yyvsp[(3) - (3)].str));
      ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 737 "syntax.y"
    {
          define_has_init = 0;
          define_init_value = make_invalid_expr();
      ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 742 "syntax.y"
    {
          define_has_init = 1;
          define_init_value = (yyvsp[(2) - (2)].expr);
      ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 750 "syntax.y"
    {
          (yyval.ival) = TYPE_INTEGER;
      ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 754 "syntax.y"
    {
          (yyval.ival) = TYPE_FLOAT;
      ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 761 "syntax.y"
    {
          (yyval.expr) = make_expr((yyvsp[(1) - (1)].str), TYPE_INTEGER);
          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 766 "syntax.y"
    {
          (yyval.expr) = make_expr((yyvsp[(1) - (1)].str), TYPE_FLOAT);
          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 792 "syntax.y"
    {
          if ((yyvsp[(1) - (3)].lval).valid && (yyvsp[(3) - (3)].expr).valid) {
              if ((yyvsp[(1) - (3)].lval).is_const) {
                  semantic_error((yyvsp[(1) - (3)].lval).name, "affectation interdite sur une constante");
              } else if (!can_assign((yyvsp[(1) - (3)].lval).type, (yyvsp[(3) - (3)].expr).type)) {
                  semantic_error((yyvsp[(1) - (3)].lval).name,
                                 "incompatibilite de type dans affectation (%s <- %s)",
                                 type_to_str((yyvsp[(1) - (3)].lval).type),
                                 type_to_str((yyvsp[(3) - (3)].expr).type));
              } else {
                  if ((yyvsp[(1) - (3)].lval).is_array_elem) {
                      emit_quad("STORE", (yyvsp[(3) - (3)].expr).place, (yyvsp[(1) - (3)].lval).index_place, (yyvsp[(1) - (3)].lval).name);
                  } else {
                      emit_quad("=", (yyvsp[(3) - (3)].expr).place, "", (yyvsp[(1) - (3)].lval).name);
                  }
              }
          }
      ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 814 "syntax.y"
    {
          SemSymbol* s = sem_lookup((yyvsp[(1) - (1)].str));

          if (!s) {
              semantic_error((yyvsp[(1) - (1)].str), "identificateur non declare");
              (yyval.lval) = make_invalid_lvalue();
          } else if (s->kind == SYM_ARRAY) {
              semantic_error((yyvsp[(1) - (1)].str), "utilisation du tableau sans indice");
              (yyval.lval) = make_invalid_lvalue();
          } else {
              (yyval.lval) = make_invalid_lvalue();
              strncpy((yyval.lval).name, (yyvsp[(1) - (1)].str), MAX_NAME_LEN - 1);
              (yyval.lval).name[MAX_NAME_LEN - 1] = '\0';
              (yyval.lval).type = s->type;
              (yyval.lval).valid = 1;
              (yyval.lval).is_const = (s->kind == SYM_CONST);
              (yyval.lval).is_array_elem = 0;
          }

          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 836 "syntax.y"
    {
          SemSymbol* s = sem_lookup((yyvsp[(1) - (4)].str));

          if (!s) {
              semantic_error((yyvsp[(1) - (4)].str), "tableau non declare");
              (yyval.lval) = make_invalid_lvalue();
          } else if (s->kind != SYM_ARRAY) {
              semantic_error((yyvsp[(1) - (4)].str), "indice applique a un identificateur non tableau");
              (yyval.lval) = make_invalid_lvalue();
          } else if (!(yyvsp[(3) - (4)].expr).valid || (yyvsp[(3) - (4)].expr).type != TYPE_INTEGER) {
              semantic_error((yyvsp[(1) - (4)].str), "indice du tableau doit etre de type integer");
              (yyval.lval) = make_invalid_lvalue();
          } else {
              (yyval.lval) = make_invalid_lvalue();
              strncpy((yyval.lval).name, (yyvsp[(1) - (4)].str), MAX_NAME_LEN - 1);
              (yyval.lval).name[MAX_NAME_LEN - 1] = '\0';
              (yyval.lval).type = s->type;
              (yyval.lval).valid = 1;
              (yyval.lval).is_const = 0;
              (yyval.lval).is_array_elem = 1;
              strncpy((yyval.lval).index_place, (yyvsp[(3) - (4)].expr).place, MAX_NAME_LEN - 1);
              (yyval.lval).index_place[MAX_NAME_LEN - 1] = '\0';
          }

          free((yyvsp[(1) - (4)].str));
      ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 866 "syntax.y"
    {
          if ((yyvsp[(3) - (6)].expr).valid && (yyvsp[(3) - (6)].expr).type != TYPE_BOOL) {
              semantic_error("if", "la condition doit etre booleenne");
          }
          push_if_context(emit_quad("BZ", (yyvsp[(3) - (6)].expr).valid ? (yyvsp[(3) - (6)].expr).place : "", "", "?"));
      ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 873 "syntax.y"
    {
          IfContext c = pop_if_context();
          if (c.has_else && c.br_index >= 0) {
              patch_quad_target(c.br_index, next_quad());
          }
      ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 883 "syntax.y"
    {
          IfContext* c = current_if_context();
          if (c) {
              c->has_else = 0;
              patch_quad_target(c->bz_index, next_quad());
          }
      ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 891 "syntax.y"
    {
          IfContext* c = current_if_context();
          if (c) {
              c->has_else = 1;
              c->br_index = emit_quad("BR", "", "", "?");
              patch_quad_target(c->bz_index, next_quad());
          }
      ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 904 "syntax.y"
    {
          (yyval.ival) = next_quad();
      ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 911 "syntax.y"
    {
          if ((yyvsp[(5) - (6)].expr).valid && (yyvsp[(5) - (6)].expr).type != TYPE_BOOL) {
              semantic_error("while", "la condition doit etre booleenne");
          }
          push_while_context((yyvsp[(3) - (6)].ival), emit_quad("BZ", (yyvsp[(5) - (6)].expr).valid ? (yyvsp[(5) - (6)].expr).place : "", "", "?"));
      ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 918 "syntax.y"
    {
          char target[32];
          WhileContext c = pop_while_context();

          if (c.start_index >= 0) {
              snprintf(target, sizeof(target), "%d", c.start_index);
              emit_quad("BR", "", "", target);
          }

          if (c.bz_index >= 0) {
              patch_quad_target(c.bz_index, next_quad());
          }
      ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 935 "syntax.y"
    {
          SemSymbol* it = sem_lookup((yyvsp[(2) - (6)].str));
          ExprAttr cmp_tmp;
          int start_idx;
          int bz_idx;

          if (!it) {
              semantic_error((yyvsp[(2) - (6)].str), "variable de boucle non declaree");
          } else if (it->kind == SYM_CONST) {
              semantic_error((yyvsp[(2) - (6)].str), "variable de boucle ne peut pas etre constante");
          } else if (it->kind == SYM_ARRAY) {
              semantic_error((yyvsp[(2) - (6)].str), "variable de boucle ne peut pas etre un tableau");
          }

          if ((yyvsp[(4) - (6)].expr).valid && (yyvsp[(4) - (6)].expr).type != TYPE_INTEGER) {
              semantic_error((yyvsp[(2) - (6)].str), "borne initiale du for doit etre integer");
          }

          if ((yyvsp[(6) - (6)].expr).valid && (yyvsp[(6) - (6)].expr).type != TYPE_INTEGER) {
              semantic_error((yyvsp[(2) - (6)].str), "borne finale du for doit etre integer");
          }

          emit_quad("=", (yyvsp[(4) - (6)].expr).valid ? (yyvsp[(4) - (6)].expr).place : "0", "", (yyvsp[(2) - (6)].str));

          start_idx = next_quad();
          cmp_tmp = new_temp(TYPE_BOOL);
          emit_quad("LE", (yyvsp[(2) - (6)].str), (yyvsp[(6) - (6)].expr).valid ? (yyvsp[(6) - (6)].expr).place : "0", cmp_tmp.place);
          bz_idx = emit_quad("BZ", cmp_tmp.place, "", "?");

          push_for_context((yyvsp[(2) - (6)].str), TYPE_INTEGER, start_idx, bz_idx);
          free((yyvsp[(2) - (6)].str));
      ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 968 "syntax.y"
    {
          char target[32];
          ForContext c = pop_for_context();
          ExprAttr inc_tmp = new_temp(c.iter_type == TYPE_INVALID ? TYPE_INTEGER : c.iter_type);

          emit_quad("ADD", c.iter_name, "1", inc_tmp.place);
          emit_quad("=", inc_tmp.place, "", c.iter_name);

          if (c.start_index >= 0) {
              snprintf(target, sizeof(target), "%d", c.start_index);
              emit_quad("BR", "", "", target);
          }

          if (c.bz_index >= 0) {
              patch_quad_target(c.bz_index, next_quad());
          }
      ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 989 "syntax.y"
    {
          SemSymbol* s = sem_lookup((yyvsp[(3) - (4)].str));

          if (!s) {
              semantic_error((yyvsp[(3) - (4)].str), "identificateur non declare pour input");
          } else if (s->kind == SYM_CONST) {
              semantic_error((yyvsp[(3) - (4)].str), "input interdit sur constante");
          } else if (s->kind == SYM_ARRAY) {
              semantic_error((yyvsp[(3) - (4)].str), "input direct sur tableau non autorise");
          } else {
              emit_quad("IN", "", "", (yyvsp[(3) - (4)].str));
          }

          free((yyvsp[(3) - (4)].str));
      ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1017 "syntax.y"
    {
          emit_quad("OUT_STR", (yyvsp[(1) - (1)].str), "", "");
          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1022 "syntax.y"
    {
          if ((yyvsp[(1) - (1)].expr).valid) {
              emit_quad("OUT", (yyvsp[(1) - (1)].expr).place, "", "");
          }
      ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1031 "syntax.y"
    {
          (yyval.expr) = build_logic("OR", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1035 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1042 "syntax.y"
    {
          (yyval.expr) = build_logic("AND", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 1046 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 1053 "syntax.y"
    {
          (yyval.expr) = build_not((yyvsp[(3) - (4)].expr));
      ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 1057 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 1064 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(2) - (3)].expr);
      ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 1068 "syntax.y"
    {
          (yyval.expr) = build_comparison((yyvsp[(1) - (3)].expr), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1075 "syntax.y"
    {
          (yyval.ival) = CMP_GT;
      ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 1079 "syntax.y"
    {
          (yyval.ival) = CMP_LT;
      ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 1083 "syntax.y"
    {
          (yyval.ival) = CMP_GE;
      ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 1087 "syntax.y"
    {
          (yyval.ival) = CMP_LE;
      ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 1091 "syntax.y"
    {
          (yyval.ival) = CMP_EQ;
      ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 1095 "syntax.y"
    {
          (yyval.ival) = CMP_NE;
      ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1102 "syntax.y"
    {
          (yyval.expr) = build_binary_numeric("ADD", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1106 "syntax.y"
    {
          (yyval.expr) = build_binary_numeric("SUB", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1110 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1117 "syntax.y"
    {
          (yyval.expr) = build_binary_numeric("MUL", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 1121 "syntax.y"
    {
          (yyval.expr) = build_binary_numeric("DIV", (yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr));
      ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 1125 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 1132 "syntax.y"
    {
          SemSymbol* s = sem_lookup((yyvsp[(1) - (1)].str));

          if (!s) {
              semantic_error((yyvsp[(1) - (1)].str), "identificateur non declare");
              (yyval.expr) = make_invalid_expr();
          } else if (s->kind == SYM_ARRAY) {
              semantic_error((yyvsp[(1) - (1)].str), "tableau utilise sans indice");
              (yyval.expr) = make_invalid_expr();
          } else {
              (yyval.expr) = make_expr((yyvsp[(1) - (1)].str), s->type);
          }

          free((yyvsp[(1) - (1)].str));
      ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 1148 "syntax.y"
    {
          SemSymbol* s = sem_lookup((yyvsp[(1) - (4)].str));

          if (!s) {
              semantic_error((yyvsp[(1) - (4)].str), "tableau non declare");
              (yyval.expr) = make_invalid_expr();
          } else if (s->kind != SYM_ARRAY) {
              semantic_error((yyvsp[(1) - (4)].str), "indice applique a un identificateur non tableau");
              (yyval.expr) = make_invalid_expr();
          } else if (!(yyvsp[(3) - (4)].expr).valid || (yyvsp[(3) - (4)].expr).type != TYPE_INTEGER) {
              semantic_error((yyvsp[(1) - (4)].str), "indice du tableau doit etre de type integer");
              (yyval.expr) = make_invalid_expr();
          } else {
              ExprAttr t = new_temp(s->type);
              emit_quad("LOAD", (yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].expr).place, t.place);
              (yyval.expr) = t;
          }

          free((yyvsp[(1) - (4)].str));
      ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 1169 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(1) - (1)].expr);
      ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 1173 "syntax.y"
    {
          (yyval.expr) = (yyvsp[(2) - (3)].expr);
      ;}
    break;



/* Line 1455 of yacc.c  */
#line 2823 "syntax.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



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
		      yytoken, &yylval);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1178 "syntax.y"


void yyerror(const char* s) {
    fprintf(stderr, "Erreur_syntaxique, ligne %d, colonne %d: %s\n", line_num, prev_col, s);
}

int main(int argc, char** argv) {
    FILE* f;
    int parse_status;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier_source>\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", argv[1]);
        return 1;
    }

    yyin = f;
    parse_status = yyparse();

    fclose(f);
    free_sem_table();

    if (parse_status != 0 || semantic_errors > 0) {
        return 1;
    }

    return 0;
}

