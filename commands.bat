@echo off
flex lex.l && gcc lex.yy.c -o lexical_analyzer.exe && lexical_analyzer.exe test2.txt
bison -d syntax.y && flex lex.l && gcc -DPARSER_MODE syntax.tab.c lex.yy.c -o syntax_parser.exe && syntax_parser.exe test2.txt
pause
