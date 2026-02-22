#include <catch2/catch_all.hpp>
#include <sstream>
#include "parser.hpp"
#include "globals.hpp"
#include <FlexLexer.h>

/**
 * Вспомогательная функция: разбирает входную строку и возвращает пару (парсер, успех).
 */
static std::pair<SyntaxAnalyzer<SLR>, bool> parse_string(const std::string& input) {
    resetGlobals();                                
    std::istringstream iss(input);                 
    yyFlexLexer lexer(&iss);                         
    std::vector<Token> tokens;
    int token_type;
    while ((token_type = lexer.yylex()) != 0) {     
        tokens.emplace_back(static_cast<TokenType>(token_type), yylval);
    }
    tokens.emplace_back(END, std::monostate{});     
    SyntaxAnalyzer<SLR> parser;
    bool success = parser.parse(tokens);             
    return {std::move(parser), success};             
}

TEST_CASE("Парсер на конкретных строках", "[parser][string]") {
    // Корректные выражения
    REQUIRE(parse_string("a + b").second == true);
    REQUIRE(parse_string("(x + 5) * 3.14").second == true);
    REQUIRE(parse_string("a * (b - c) / d").second == true);
    REQUIRE(parse_string("((a))").second == true);
    REQUIRE(parse_string("a + b + c + d + e + f + g + h + i + j").second == true);
    REQUIRE(parse_string("12 + a - 3.5 * (b + 8)").second == true);

    // Некорректные выражения
    REQUIRE(parse_string("").second == false);               // пустая строка
    REQUIRE(parse_string("a +").second == false);            // недостающий операнд
    REQUIRE(parse_string("(a + b").second == false);         // незакрытая скобка
    REQUIRE(parse_string("a * * b").second == false);        // два оператора подряд
    REQUIRE(parse_string("-a").second == false);             // унарный минус не поддерживается
    REQUIRE(parse_string("+").second == false);              // только оператор
    REQUIRE(parse_string("(a) b").second == false);          // два выражения подряд
    REQUIRE(parse_string("a + - b").second == false);        // унарный минус после бинарного
}

TEST_CASE("Последовательность свёрток для корректных выражений", "[parser][reductions]") {
    SECTION("Одиночный идентификатор 'a'") {
        auto [parser, success] = parse_string("a");
        REQUIRE(success == true);
        // Ожидаем: F->id (7), T->F (6), E->T (3)
        std::vector<int> expected = {7, 6, 3};
        REQUIRE(parser.reductions == expected);
    }

    SECTION("Сложение 'a + b'") {
        auto [parser, success] = parse_string("a + b");
        REQUIRE(success == true);
        // Реальная последовательность из отладочного вывода:
        // 7:F->id, 6:T->F, 3:E->T, 7:F->id, 6:T->F, 1:E->E+T
        std::vector<int> expected = {7, 6, 3, 7, 6, 1};
        REQUIRE(parser.reductions == expected);
    }

    SECTION("Умножение 'a * b'") {
        auto [parser, success] = parse_string("a * b");
        REQUIRE(success == true);
        // Реальная последовательность:
        // 7:F->id, 6:T->F, 7:F->id, 4:T->T*F, 3:E->T
        std::vector<int> expected = {7, 6, 7, 4, 3};
        REQUIRE(parser.reductions == expected);
    }
}

TEST_CASE("Последовательность свёрток для некорректных выражений", "[parser][reductions]") {
    SECTION("Пропущен операнд 'a +'") {
        auto [parser, success] = parse_string("a +");
        REQUIRE(success == false);
        // Должны успеть свернуть 'a'
        std::vector<int> expected = {7, 6, 3};
        REQUIRE(parser.reductions == expected);
    }
}