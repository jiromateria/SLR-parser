%option c++
%option noyywrap

%{
#include "tokens.hpp"
#include <cstdlib> 
#include "globals.hpp"
%}


DIGIT    [0-9]
LETTER   [a-zA-Z]

%%
    /*Вещественное число*/
{DIGIT}+"."{DIGIT}+ {
    yylval = atof(yytext);
    return NUM_FLOAT;
}

    /*Целое число*/
{DIGIT}+ {
    yylval = atoi(yytext);
    return NUM_INT;
}

    /*Идентификаторы*/
{LETTER}+ {
    auto [it, inserted] = symbtable.try_emplace(yytext, symbID);

    if(inserted)
        symbID++;

    yylval = it->second;
    return ID;
}

[ \t\r\n]+ { /*Игнорируем пробелы*/}

"+" { yylval = std::monostate{}; return PLUS; }
"-" { yylval = std::monostate{}; return MINUS; }
"*" { yylval = std::monostate{}; return MULTIPLY; }
"/" { yylval = std::monostate{}; return DIVIDE; }
"(" { yylval = std::monostate{}; return LPAREN; }
")" { yylval = std::monostate{}; return RPAREN; }

. { yylval = std::monostate{}; return ERROR;}
%%
