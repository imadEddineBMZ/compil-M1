%code requires {
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
}

%{
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

%}

%union {
    char* str;
    int ival;
    ExprAttr expr;
    LValueAttr lval;
}

%token <str> IDF CONST_INTEGER CONST_FLOAT STRING

%token MC_BEGIN_PROJECT MC_END_PROJECT MC_SETUP MC_RUN
%token MC_DEFINE MC_CONST MC_TYPE_INTEGER MC_TYPE_FLOAT
%token MC_IF MC_THEN MC_ELSE MC_ENDIF
%token MC_LOOP MC_WHILE MC_ENDLOOP
%token MC_FOR MC_IN MC_TO MC_ENDFOR
%token MC_OUT

%token OP_AND OP_OR OP_NON
%token OP_GE OP_LE OP_EQ OP_NE OP_GT OP_LT
%token OP_PLUS OP_MINUS OP_MULT OP_DIV OP_AFFECT

%token SEP_EQUAL SEP_SEMICOLON SEP_COLON SEP_COMMA SEP_PIPE
%token SEP_LBRACE SEP_RBRACE SEP_LBRACKET SEP_RBRACKET SEP_LPAREN SEP_RPAREN

%type <ival> type comp_op while_mark
%type <expr> literal expr term factor condition condition_and condition_not condition_atom
%type <lval> lvalue

%start program

%%

program
    : MC_BEGIN_PROJECT IDF SEP_SEMICOLON
      MC_SETUP SEP_COLON declarations
      MC_RUN SEP_COLON block
      MC_END_PROJECT SEP_SEMICOLON
      {
          if (semantic_errors == 0) {
              printf("Analyse syntaxique et semantique reussie.\n");
          } else {
              printf("Analyse syntaxique terminee avec %d erreur(s) semantiques.\n", semantic_errors);
          }
          print_quads();
          free($2);
      }
    ;

declarations
    :
    | declarations declaration
    ;

declaration
    : MC_DEFINE IDF
      {
          begin_define_decl($2);
          free($2);
      }
      define_rest
    | MC_CONST IDF SEP_COLON type SEP_EQUAL literal SEP_SEMICOLON
      {
          declare_constant($2, (DataType)$4, $6);
          free($2);
      }
    ;


define_rest
    : SEP_PIPE id_list_tail SEP_COLON type SEP_SEMICOLON
      {
          commit_define_scalar((DataType)$4);
      }
    | SEP_COLON type define_init_opt SEP_SEMICOLON
      {
          commit_define_scalar((DataType)$2);
      }
    | SEP_COLON SEP_LBRACKET type SEP_SEMICOLON CONST_INTEGER SEP_RBRACKET SEP_SEMICOLON
      {
          commit_define_array((DataType)$3, $5);
          free($5);
      }
    ;

id_list_tail
    : IDF
      {
          pending_add($1);
          free($1);
      }
    | id_list_tail SEP_PIPE IDF
      {
          pending_add($3);
          free($3);
      }
    ;

define_init_opt
    :
      {
          define_has_init = 0;
          define_init_value = make_invalid_expr();
      }
    | SEP_EQUAL literal
      {
          define_has_init = 1;
          define_init_value = $2;
      }
    ;

type
    : MC_TYPE_INTEGER
      {
          $$ = TYPE_INTEGER;
      }
    | MC_TYPE_FLOAT
      {
          $$ = TYPE_FLOAT;
      }
    ;

literal
    : CONST_INTEGER
      {
          $$ = make_expr($1, TYPE_INTEGER);
          free($1);
      }
    | CONST_FLOAT
      {
          $$ = make_expr($1, TYPE_FLOAT);
          free($1);
      }
    ;

block
    : SEP_LBRACE statements SEP_RBRACE
    ;

statements
    :
    | statements statement
    ;

statement
    : assign_stmt SEP_SEMICOLON
    | if_stmt SEP_SEMICOLON
    | while_stmt SEP_SEMICOLON
    | for_stmt SEP_SEMICOLON
    | input_stmt SEP_SEMICOLON
    | output_stmt SEP_SEMICOLON
    ;

assign_stmt
    : lvalue OP_AFFECT expr
      {
          if ($1.valid && $3.valid) {
              if ($1.is_const) {
                  semantic_error($1.name, "affectation interdite sur une constante");
              } else if (!can_assign($1.type, $3.type)) {
                  semantic_error($1.name,
                                 "incompatibilite de type dans affectation (%s <- %s)",
                                 type_to_str($1.type),
                                 type_to_str($3.type));
              } else {
                  if ($1.is_array_elem) {
                      emit_quad("STORE", $3.place, $1.index_place, $1.name);
                  } else {
                      emit_quad("=", $3.place, "", $1.name);
                  }
              }
          }
      }
    ;

lvalue
    : IDF
      {
          SemSymbol* s = sem_lookup($1);

          if (!s) {
              semantic_error($1, "identificateur non declare");
              $$ = make_invalid_lvalue();
          } else if (s->kind == SYM_ARRAY) {
              semantic_error($1, "utilisation du tableau sans indice");
              $$ = make_invalid_lvalue();
          } else {
              $$ = make_invalid_lvalue();
              strncpy($$.name, $1, MAX_NAME_LEN - 1);
              $$.name[MAX_NAME_LEN - 1] = '\0';
              $$.type = s->type;
              $$.valid = 1;
              $$.is_const = (s->kind == SYM_CONST);
              $$.is_array_elem = 0;
          }

          free($1);
      }
    | IDF SEP_LBRACKET expr SEP_RBRACKET
      {
          SemSymbol* s = sem_lookup($1);

          if (!s) {
              semantic_error($1, "tableau non declare");
              $$ = make_invalid_lvalue();
          } else if (s->kind != SYM_ARRAY) {
              semantic_error($1, "indice applique a un identificateur non tableau");
              $$ = make_invalid_lvalue();
          } else if (!$3.valid || $3.type != TYPE_INTEGER) {
              semantic_error($1, "indice du tableau doit etre de type integer");
              $$ = make_invalid_lvalue();
          } else {
              $$ = make_invalid_lvalue();
              strncpy($$.name, $1, MAX_NAME_LEN - 1);
              $$.name[MAX_NAME_LEN - 1] = '\0';
              $$.type = s->type;
              $$.valid = 1;
              $$.is_const = 0;
              $$.is_array_elem = 1;
              strncpy($$.index_place, $3.place, MAX_NAME_LEN - 1);
              $$.index_place[MAX_NAME_LEN - 1] = '\0';
          }

          free($1);
      }
    ;

if_stmt
    : MC_IF SEP_LPAREN condition SEP_RPAREN MC_THEN SEP_COLON
      {
          if ($3.valid && $3.type != TYPE_BOOL) {
              semantic_error("if", "la condition doit etre booleenne");
          }
          push_if_context(emit_quad("BZ", $3.valid ? $3.place : "", "", "?"));
      }
      block opt_else MC_ENDIF
      {
          IfContext c = pop_if_context();
          if (c.has_else && c.br_index >= 0) {
              patch_quad_target(c.br_index, next_quad());
          }
      }
    ;

opt_else
    :
      {
          IfContext* c = current_if_context();
          if (c) {
              c->has_else = 0;
              patch_quad_target(c->bz_index, next_quad());
          }
      }
    | MC_ELSE
      {
          IfContext* c = current_if_context();
          if (c) {
              c->has_else = 1;
              c->br_index = emit_quad("BR", "", "", "?");
              patch_quad_target(c->bz_index, next_quad());
          }
      }
      block
    ;

while_mark
    :
      {
          $$ = next_quad();
      }
    ;

while_stmt
    : MC_LOOP MC_WHILE while_mark SEP_LPAREN condition SEP_RPAREN
      {
          if ($5.valid && $5.type != TYPE_BOOL) {
              semantic_error("while", "la condition doit etre booleenne");
          }
          push_while_context($3, emit_quad("BZ", $5.valid ? $5.place : "", "", "?"));
      }
      block MC_ENDLOOP
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
      }
    ;

for_stmt
    : MC_FOR IDF MC_IN expr MC_TO expr
      {
          SemSymbol* it = sem_lookup($2);
          ExprAttr cmp_tmp;
          int start_idx;
          int bz_idx;

          if (!it) {
              semantic_error($2, "variable de boucle non declaree");
          } else if (it->kind == SYM_CONST) {
              semantic_error($2, "variable de boucle ne peut pas etre constante");
          } else if (it->kind == SYM_ARRAY) {
              semantic_error($2, "variable de boucle ne peut pas etre un tableau");
          }

          if ($4.valid && $4.type != TYPE_INTEGER) {
              semantic_error($2, "borne initiale du for doit etre integer");
          }

          if ($6.valid && $6.type != TYPE_INTEGER) {
              semantic_error($2, "borne finale du for doit etre integer");
          }

          emit_quad("=", $4.valid ? $4.place : "0", "", $2);

          start_idx = next_quad();
          cmp_tmp = new_temp(TYPE_BOOL);
          emit_quad("LE", $2, $6.valid ? $6.place : "0", cmp_tmp.place);
          bz_idx = emit_quad("BZ", cmp_tmp.place, "", "?");

          push_for_context($2, TYPE_INTEGER, start_idx, bz_idx);
          free($2);
      }
      block MC_ENDFOR
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
      }
    ;

input_stmt
    : MC_IN SEP_LPAREN IDF SEP_RPAREN
      {
          SemSymbol* s = sem_lookup($3);

          if (!s) {
              semantic_error($3, "identificateur non declare pour input");
          } else if (s->kind == SYM_CONST) {
              semantic_error($3, "input interdit sur constante");
          } else if (s->kind == SYM_ARRAY) {
              semantic_error($3, "input direct sur tableau non autorise");
          } else {
              emit_quad("IN", "", "", $3);
          }

          free($3);
      }
    ;

output_stmt
    : MC_OUT SEP_LPAREN output_args SEP_RPAREN
    ;

output_args
    : output_arg
    | output_args SEP_COMMA output_arg
    ;

output_arg
    : STRING
      {
          emit_quad("OUT_STR", $1, "", "");
          free($1);
      }
    | expr
      {
          if ($1.valid) {
              emit_quad("OUT", $1.place, "", "");
          }
      }
    ;

condition
    : condition OP_OR condition_and
      {
          $$ = build_logic("OR", $1, $3);
      }
    | condition_and
      {
          $$ = $1;
      }
    ;

condition_and
    : condition_and OP_AND condition_not
      {
          $$ = build_logic("AND", $1, $3);
      }
    | condition_not
      {
          $$ = $1;
      }
    ;

condition_not
    : OP_NON SEP_LPAREN condition SEP_RPAREN
      {
          $$ = build_not($3);
      }
    | condition_atom
      {
          $$ = $1;
      }
    ;

condition_atom
    : SEP_LPAREN condition SEP_RPAREN
      {
          $$ = $2;
      }
    | expr comp_op expr
      {
          $$ = build_comparison($1, $2, $3);
      }
    ;

comp_op
    : OP_GT
      {
          $$ = CMP_GT;
      }
    | OP_LT
      {
          $$ = CMP_LT;
      }
    | OP_GE
      {
          $$ = CMP_GE;
      }
    | OP_LE
      {
          $$ = CMP_LE;
      }
    | OP_EQ
      {
          $$ = CMP_EQ;
      }
    | OP_NE
      {
          $$ = CMP_NE;
      }
    ;

expr
    : expr OP_PLUS term
      {
          $$ = build_binary_numeric("ADD", $1, $3);
      }
    | expr OP_MINUS term
      {
          $$ = build_binary_numeric("SUB", $1, $3);
      }
    | term
      {
          $$ = $1;
      }
    ;

term
    : term OP_MULT factor
      {
          $$ = build_binary_numeric("MUL", $1, $3);
      }
    | term OP_DIV factor
      {
          $$ = build_binary_numeric("DIV", $1, $3);
      }
    | factor
      {
          $$ = $1;
      }
    ;

factor
    : IDF
      {
          SemSymbol* s = sem_lookup($1);

          if (!s) {
              semantic_error($1, "identificateur non declare");
              $$ = make_invalid_expr();
          } else if (s->kind == SYM_ARRAY) {
              semantic_error($1, "tableau utilise sans indice");
              $$ = make_invalid_expr();
          } else {
              $$ = make_expr($1, s->type);
          }

          free($1);
      }
    | IDF SEP_LBRACKET expr SEP_RBRACKET
      {
          SemSymbol* s = sem_lookup($1);

          if (!s) {
              semantic_error($1, "tableau non declare");
              $$ = make_invalid_expr();
          } else if (s->kind != SYM_ARRAY) {
              semantic_error($1, "indice applique a un identificateur non tableau");
              $$ = make_invalid_expr();
          } else if (!$3.valid || $3.type != TYPE_INTEGER) {
              semantic_error($1, "indice du tableau doit etre de type integer");
              $$ = make_invalid_expr();
          } else {
              ExprAttr t = new_temp(s->type);
              emit_quad("LOAD", $1, $3.place, t.place);
              $$ = t;
          }

          free($1);
      }
    | literal
      {
          $$ = $1;
      }
    | SEP_LPAREN expr SEP_RPAREN
      {
          $$ = $2;
      }
    ;

%%

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
