========================================
ANALYSEUR LEXICAL pour ProLang
========================================

Ce projet contient l'analyseur lexical pour le langage ProLang développé avec FLEX.

FICHIERS:
---------
- lex.l              : Spécification FLEX de l'analyseur lexical
- test.txt           : Fichier test contenant un programme ProLang exemple
- commands.bat       : Script pour compiler et exécuter l'analyseur
- README.txt         : Ce fichier

PRÉREQUIS:
----------
- FLEX (GnuWin32 ou équivalent)
- GCC (MinGW ou équivalent)

Les deux doivent être dans le PATH système.

COMPILATION ET EXÉCUTION:
------------------------
Méthode 1 (Recommandée):
   Double-cliquez sur commands.bat

Méthode 2 (Manuel):
   1. Ouvrir un terminal dans le dossier projet/
   2. Générer le code C:
      flex lex.l
   3. Compiler:
      gcc lex.yy.c -o lexical_analyzer.exe
   4. Exécuter:
      lexical_analyzer.exe test.txt

TOKENS RECONNUS:
----------------
1. Mots-clés du langage:
   - Structure: BeginProject, EndProject, Setup, Run
   - Déclarations: define, const
   - Types: integer, float
   - Conditions: if, then, else, endIf
   - Boucles: loop, while, endloop, for, in, to, endfor
   - I/O: in, out
   - Logiques: AND, OR, NON

2. Opérateurs:
   - Arithmétiques: +, -, *, /
   - Comparaison: <, >, <=, >=, ==, !=
   - Affectation: ←
   - Logiques: AND, OR, NON

3. Séparateurs:
   ; : , | { } [ ] ( )

4. Constantes:
   - Entières: 5, (-10), (+15)
     Limites: [-32768, 32767]
   - Réelles: 3.14, (-2.5), (+1.0)

5. Identificateurs:
   - Commence par une lettre
   - Contient lettres, chiffres, underscore
   - Maximum 14 caractères
   - Ne se termine pas par _
   - Pas de __ consécutifs

6. Chaînes de caractères:
   Entre guillemets: "Bonjour"

7. Commentaires:
   - Une ligne: %% commentaire
   - Multi-lignes: //* commentaire *//

GESTION D'ERREURS:
------------------
L'analyseur détecte et affiche:
- Identificateurs invalides (trop longs, format incorrect)
- Constantes hors limites
- Caractères non reconnus
- Commentaires non fermés
- Position exacte (ligne, colonne)

EXEMPLE DE SORTIE:
------------------
MC_BEGIN_PROJECT     : BeginProject        [Ligne: 1, Colonne: 1]
IDF                  : M1_IV_2526          [Ligne: 1, Colonne: 14]
SEP_SEMICOLON        : ;                   [Ligne: 1, Colonne: 24]
...

AUTEUR:
-------
Projet de compilation - Master 1 IV
Université des Sciences et de la Technologie Houari Boumediene
Année universitaire: 2025/2026
