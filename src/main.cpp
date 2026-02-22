#include "parser.hpp"
#include "globals.hpp"
#include <FlexLexer.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

int main(int argc, char* argv[]) {
    resetGlobals();

    std::unique_ptr<std::ifstream> file_stream;
    std::istream* input_stream = &std::cin;

    if (argc > 1) {
        file_stream = std::make_unique<std::ifstream>(argv[1]);
        if (!file_stream->is_open()) {
            std::cerr << "Cannot open file: " << argv[1] << std::endl;
            return 1;
        }
        input_stream = file_stream.get();
    }

    // Создаём лексер и перенаправляем его входной поток
    yyFlexLexer lexer(input_stream, &std::cout); 

    // Читаем все токены
    std::vector<Token> tokens;
    int token_type;
    while ((token_type = lexer.yylex()) != 0) {
        if (token_type == ERROR) {
            std::cerr << "Lexical error: unknown symbol encountered.\n";
            return 1;
        }
        tokens.emplace_back(static_cast<TokenType>(token_type), yylval);
    }
    tokens.emplace_back(END, std::monostate{});   // добавляем маркер конца

    // Запускаем синтаксический анализ
    SyntaxAnalyzer<SLR> parser;
    bool success = parser.parse(tokens);

    // Выводим результат
    std::cout << (success ? "Parsing succeeded.\n" : "Parsing failed.\n");
    return success ? 0 : 1;
}