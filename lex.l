%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PARSER_MODE
#include "syntax.tab.h"
#endif

int line_num = 1;
int col_num = 1;
int prev_col = 1;

#ifndef PARSER_MODE
#define HASH_SIZE 211

typedef struct Symbol {
    char name[64];
    char nature[16];
    int first_line;
    int occurrences;
    struct Symbol* next;
} Symbol;

static Symbol* symtab[HASH_SIZE];

static unsigned int hash_symbol(const char* s) {
    unsigned int h = 0;
    while (*s) {
        h = (h * 31u) + (unsigned char)(*s++);
    }
    return h % HASH_SIZE;
}

static Symbol* find_symbol(const char* name, const char* nature) {
    unsigned int idx = hash_symbol(name);
    Symbol* cur = symtab[idx];
    while (cur) {
        if (strcmp(cur->name, name) == 0 && strcmp(cur->nature, nature) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

static void insert_symbol(const char* name, const char* nature) {
    unsigned int idx;
    Symbol* s;

    s = find_symbol(name, nature);
    if (s) {
        s->occurrences++;
        return;
    }

    idx = hash_symbol(name);
    s = (Symbol*)malloc(sizeof(Symbol));
    if (!s) {
        fprintf(stderr, "Erreur memoire table des symboles\n");
        exit(1);
    }

    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    strncpy(s->nature, nature, sizeof(s->nature) - 1);
    s->nature[sizeof(s->nature) - 1] = '\0';
    s->first_line = line_num;
    s->occurrences = 1;
    s->next = symtab[idx];
    symtab[idx] = s;
}

static void print_symbol_table(void) {
    int i;
    Symbol* cur;

    printf("\n========================================\n");
    printf("      TABLE DES SYMBOLES (HACHAGE)\n");
    printf("========================================\n");
    printf("%-18s %-14s %-10s %-10s\n", "Nom", "Nature", "1ere_ligne", "Occur");
    printf("--------------------------------------------------------\n");

    for (i = 0; i < HASH_SIZE; i++) {
        cur = symtab[i];
        while (cur) {
            printf("%-18s %-14s %-10d %-10d\n", cur->name, cur->nature, cur->first_line, cur->occurrences);
            cur = cur->next;
        }
    }
}

static void free_symbol_table(void) {
    int i;
    Symbol* cur;
    Symbol* nxt;

    for (i = 0; i < HASH_SIZE; i++) {
        cur = symtab[i];
        while (cur) {
            nxt = cur->next;
            free(cur);
            cur = nxt;
        }
        symtab[i] = NULL;
    }
}

#define REGISTER_SYMBOL(NAME, NATURE) insert_symbol(NAME, NATURE)
#else
#define REGISTER_SYMBOL(NAME, NATURE) do { } while (0)
#endif

void print_token(const char* token_type, const char* value) {
    printf("%-20s : %-20s [Ligne: %d, Colonne: %d]\n", token_type, value, line_num, prev_col);
}

void lexical_error(const char* msg, const char* entity) {
    printf("Erreur_lexicale, ligne %d, colonne %d, %s: %s\n", line_num, prev_col, msg, entity);
}

#ifdef PARSER_MODE
static char* dup_yytext(const char* s) {
    size_t n = strlen(s);
    char* p = (char*)malloc(n + 1);
    if (!p) {
        fprintf(stderr, "Erreur memoire\n");
        exit(1);
    }
    strcpy(p, s);
    return p;
}

#define EMIT_TOKEN(TOK, NAME) return TOK
#define SET_TEXT() do { yylval.str = dup_yytext(yytext); } while (0)
#else
#define EMIT_TOKEN(TOK, NAME) do { print_token(NAME, yytext); } while (0)
#define SET_TEXT() do { } while (0)
#endif

int validate_identifier(const char* id) {
    int len = strlen(id);
    int i;
    
    if (len > 14 || len == 0) return 0;
    
    if (!((id[0] >= 'a' && id[0] <= 'z') || (id[0] >= 'A' && id[0] <= 'Z'))) return 0;
    
    if (id[len - 1] == '_') return 0;
    
    for (i = 0; i < len - 1; i++) {
        if (id[i] == '_' && id[i+1] == '_') return 0;
    }
    
    /* Vérifier que tous les caractères sont valides */
    for (i = 0; i < len; i++) {
        if (!((id[i] >= 'a' && id[i] <= 'z') || 
              (id[i] >= 'A' && id[i] <= 'Z') || 
              (id[i] >= '0' && id[i] <= '9') || 
              id[i] == '_')) {
            return 0;
        }
    }
    
    return 1;
}

int validate_integer(const char* num) {
    long val = atol(num);
    if (val < -32768 || val > 32767) {
        return 0;
    }
    return 1;
}

%}

DIGIT       [0-9]
LETTER      [a-zA-Z]
UNDERSCORE  "_"

%x COMMENT_MULTI

%%

"%%".*                  { prev_col = col_num; col_num += yyleng; /* Commentaire une ligne - ignoré */ }

"//*"                   { prev_col = col_num; col_num += yyleng; BEGIN(COMMENT_MULTI); }
<COMMENT_MULTI>"*//"    { col_num += yyleng; BEGIN(INITIAL); }
<COMMENT_MULTI>"*/"     { col_num += yyleng; BEGIN(INITIAL); }
<COMMENT_MULTI>\n       { line_num++; col_num = 1; }
<COMMENT_MULTI>.        { col_num++; }
<COMMENT_MULTI><<EOF>>  { lexical_error("Commentaire non fermé", "/*"); return 0; }

"BeginProject"      { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_BEGIN_PROJECT, "MC_BEGIN_PROJECT"); }
"EndProject"        { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_END_PROJECT, "MC_END_PROJECT"); }
"Setup"             { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_SETUP, "MC_SETUP"); }
"Run"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_RUN, "MC_RUN"); }
"define"            { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_DEFINE, "MC_DEFINE"); }
"const"             { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_CONST, "MC_CONST"); }

"integer"           { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_TYPE_INTEGER, "MC_TYPE_INTEGER"); }
"float"             { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_TYPE_FLOAT, "MC_TYPE_FLOAT"); }

"if"                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_IF, "MC_IF"); }
"then"              { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_THEN, "MC_THEN"); }
"else"              { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_ELSE, "MC_ELSE"); }
"endIf"             { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_ENDIF, "MC_ENDIF"); }

"loop"              { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_LOOP, "MC_LOOP"); }
"while"             { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_WHILE, "MC_WHILE"); }
"endloop"           { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_ENDLOOP, "MC_ENDLOOP"); }
"for"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_FOR, "MC_FOR"); }
"in"                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_IN, "MC_IN"); }
"to"                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_TO, "MC_TO"); }
"endfor"            { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_ENDFOR, "MC_ENDFOR"); }

"out"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(MC_OUT, "MC_OUT"); }

"AND"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_AND, "OP_AND"); }
"OR"                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_OR, "OP_OR"); }
"NON"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_NON, "OP_NON"); }

">="                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_GE, "OP_GE"); }
"<="                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_LE, "OP_LE"); }
"=="                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_EQ, "OP_EQ"); }
"!="                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_NE, "OP_NE"); }
">"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_GT, "OP_GT"); }
"<"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_LT, "OP_LT"); }

"+"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_PLUS, "OP_PLUS"); }
"-"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_MINUS, "OP_MINUS"); }
"*"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_MULT, "OP_MULT"); }
"/"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_DIV, "OP_DIV"); }

"<--"               { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_AFFECT, "OP_AFFECT"); }
"<-"                { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(OP_AFFECT, "OP_AFFECT"); }

"="                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_EQUAL, "SEP_EQUAL"); }
";"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_SEMICOLON, "SEP_SEMICOLON"); }
":"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_COLON, "SEP_COLON"); }
","                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_COMMA, "SEP_COMMA"); }
"|"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_PIPE, "SEP_PIPE"); }
"{"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_LBRACE, "SEP_LBRACE"); }
"}"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_RBRACE, "SEP_RBRACE"); }
"["                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_LBRACKET, "SEP_LBRACKET"); }
"]"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_RBRACKET, "SEP_RBRACKET"); }
"("                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_LPAREN, "SEP_LPAREN"); }
")"                 { prev_col = col_num; col_num += yyleng; EMIT_TOKEN(SEP_RPAREN, "SEP_RPAREN"); }


"("[-+]{DIGIT}+"."{DIGIT}*")"  { 
    prev_col = col_num; 
    col_num += yyleng; 
    REGISTER_SYMBOL(yytext, "CONST_FLOAT");
    SET_TEXT();
    EMIT_TOKEN(CONST_FLOAT, "CONST_FLOAT");
}

{DIGIT}+"."{DIGIT}*         { 
    prev_col = col_num; 
    col_num += yyleng; 
    REGISTER_SYMBOL(yytext, "CONST_FLOAT");
    SET_TEXT();
    EMIT_TOKEN(CONST_FLOAT, "CONST_FLOAT");
}

"("[-+]{DIGIT}+")"          { 
    prev_col = col_num; 
    col_num += yyleng; 
    char num[100];
    strncpy(num, yytext+2, strlen(yytext)-3);
    num[strlen(yytext)-3] = '\0';
    if (yytext[1] == '-') {
        char temp[100];
        sprintf(temp, "-%s", num);
        strcpy(num, temp);
    }
    if (validate_integer(num)) {
        REGISTER_SYMBOL(yytext, "CONST_INTEGER");
        SET_TEXT();
        EMIT_TOKEN(CONST_INTEGER, "CONST_INTEGER");
    } else {
        lexical_error("Constante entière hors limite [-32768, 32767]", yytext);
#ifdef PARSER_MODE
        return 0;
#endif
    }
}

{DIGIT}+                    { 
    prev_col = col_num; 
    col_num += yyleng; 
    if (validate_integer(yytext)) {
        REGISTER_SYMBOL(yytext, "CONST_INTEGER");
        SET_TEXT();
        EMIT_TOKEN(CONST_INTEGER, "CONST_INTEGER");
    } else {
        lexical_error("Constante entière hors limite [-32768, 32767]", yytext);
#ifdef PARSER_MODE
        return 0;
#endif
    }
}

\"([^\"\n]*)\"              { 
    prev_col = col_num; 
    col_num += yyleng; 
    SET_TEXT();
    EMIT_TOKEN(STRING, "STRING");
}

    /* === IDENTIFICATEURS === */
{LETTER}({LETTER}|{DIGIT}|{UNDERSCORE})* { 
    prev_col = col_num; 
    col_num += yyleng; 
    if (validate_identifier(yytext)) {
        REGISTER_SYMBOL(yytext, "IDF");
        SET_TEXT();
        EMIT_TOKEN(IDF, "IDF");
    } else {
        if (strlen(yytext) > 14) {
            lexical_error("Identificateur trop long (max 14 caractères)", yytext);
        } else if (yytext[strlen(yytext)-1] == '_') {
            lexical_error("Identificateur ne peut pas se terminer par _", yytext);
        } else if (strstr(yytext, "__") != NULL) {
            lexical_error("Identificateur ne peut pas contenir __ consécutifs", yytext);
        } else {
            lexical_error("Identificateur invalide", yytext);
        }
#ifdef PARSER_MODE
        return 0;
#endif
    }
}

\n                          { line_num++; col_num = 1; }
[ \t\r]+                    { col_num += yyleng; }

.                           { 
    prev_col = col_num; 
    lexical_error("Caractère non reconnu", yytext); 
    col_num += yyleng; 
#ifdef PARSER_MODE
    return 0;
#endif
}

%%

int yywrap() {
    return 1;
}

#ifndef PARSER_MODE
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <fichier_source>\n", argv[0]);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 1;
    }
    
    yyin = file;
    
    printf("\n========================================\n");
    printf("   ANALYSE LEXICALE - ProLang\n");
    printf("========================================\n\n");
    printf("%-20s : %-20s [Position]\n", "TYPE TOKEN", "VALEUR");
    printf("--------------------------------------------------------\n");
    
    yylex();

    print_symbol_table();
    
    printf("\n========================================\n");
    printf("Analyse lexicale terminee!\n");
    printf("========================================\n\n");
    
    free_symbol_table();
    fclose(file);
    return 0;
}
#endif
