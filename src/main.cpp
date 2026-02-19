// main.cpp
#include <iostream>
#include <FlexLexer.h>   // для yyFlexLexer
#include "tokens.hpp"    // типы токенов и yylval
#include "globals.hpp"


int main() {
    // Создаём экземпляр лексера (по умолчанию читает из std::cin)
    yyFlexLexer lexer;

    int token;
    Token abc(static_cast<TokenType>(lexer.yylex()), yylval);
    Token agua(PLUS,0);
    abc = agua;
    std::cout << (abc.type == static_cast<int>(PLUS)) <<"\n";
    // Цикл разбора до тех пор, пока не вернётся 0 (END)
    while ((token = lexer.yylex()) != 0) {
        std::cout << "Token: ";
        switch (token) {
            case NUM_INT:
                std::cout << "NUM_INT, value = " << std::get<int>(yylval);
                break;
            case NUM_FLOAT:
                std::cout << "NUM_FLOAT, value = " << std::get<double>(yylval);
                break;
            case ID:
                std::cout << "ID, table index = " << std::get<int>(yylval);
                break;
            case PLUS:
                std::cout << "PLUS";
                break;
            case MINUS:
                std::cout << "MINUS";
                break;
            case MULTIPLY:
                std::cout << "MULTIPLY";
                break;
            case DIVIDE:
                std::cout << "DIVIDE";
                break;
            case LPAREN:
                std::cout << "LPAREN";
                break;
            case RPAREN:
                std::cout << "RPAREN";
                break;
            case ERROR:
                std::cout << "ERROR (unexpected character)";
                break;
            default:
                std::cout << "UNKNOWN TOKEN TYPE";
        }
        std::cout << std::endl;
    }

    std::cout << "END OF INPUT" << std::endl;
    return 0;
}