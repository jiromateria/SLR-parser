#include <catch2/catch_all.hpp>
#include <sstream>
#include <FlexLexer.h>
#include "tokens.hpp"
#include "globals.hpp"

//================================
//| TECT 1 |  ОБРАБОТКА ТОКЕНОВ  |
//================================
TEST_CASE("Лексер распознает числа, операторы, индетификаторы", "lexer") {
    resetGlobals();

    yyFlexLexer lexer;
    std::string input = "42 + - * / ()67.812 Rahdan";
    std::istringstream iss(input);
    lexer.switch_streams(&iss, nullptr);

    SECTION("Последовательность токенов") {

        // INT(42) -> NUM_INT
        REQUIRE(lexer.yylex() == NUM_INT);
        CHECK(std::get<int>(yylval) == 42);

        // + -> PLUS
        REQUIRE(lexer.yylex() == PLUS);

        // - -> MINUS
        REQUIRE(lexer.yylex() == MINUS);

        // * -> MULTIPLY
        REQUIRE(lexer.yylex() == MULTIPLY);

        // / -> DIVIDE
        REQUIRE(lexer.yylex() == DIVIDE);

        // ( -> LPAREN
        REQUIRE(lexer.yylex() == LPAREN);

        // ) -> RPAREN
        REQUIRE(lexer.yylex() == RPAREN);

        // FLOAT(67.812) -> NUM_FLOAT , нужно проверять через Approx т.к. вещественное
        REQUIRE(lexer.yylex() == NUM_FLOAT);
        CHECK(std::get<double>(yylval) == Catch::Approx(67.812));

        // ID, нужно также проверить запись в хеш-таблицу
        REQUIRE(lexer.yylex() == ID);
        CHECK(std::get<int>(yylval) == 0);
        auto it = symbtable.find("Rahdan");
        REQUIRE(it != symbtable.end());
        CHECK(it->second == 0);

        // END
        REQUIRE(lexer.yylex() == END);
    }
}

//====================
//| TECT 2 |  ERROR  |
//====================
TEST_CASE("Лексер возвращает ERROR на неизвестный символ", "lexer") {
    resetGlobals();

    yyFlexLexer lexer;
    std::istringstream iss("@ CSЩ");
    lexer.switch_streams(&iss, nullptr);

    // ERROR на @
    REQUIRE(lexer.yylex() == ERROR);
    CHECK(std::holds_alternative<std::monostate>(yylval));

    // Читается ID - CS(проверяем только, что оно прочиталось: остально в другом тесте)
    REQUIRE(lexer.yylex() == ID);

    // Ошибка на Щ
    REQUIRE(lexer.yylex() == ERROR);
    CHECK(std::holds_alternative<std::monostate>(yylval));
}

//==========================
//| TECT 3 |  ХЕШ ТАБЛИЦА  |
//==========================
TEST_CASE("Заполнение хеш_таблицы символов", "lexer") {
    resetGlobals();

    yyFlexLexer lexer;
    std::istringstream iss("Sber Sber Compilers");
    lexer.switch_streams(&iss, nullptr);

    // Sber -> symbtable
        REQUIRE(lexer.yylex() == ID);
        REQUIRE(symbtable.size() == 1);
        CHECK(std::get<int>(yylval) == 0);
        auto it = symbtable.find("Sber");
        REQUIRE(it != symbtable.end());
        CHECK(it->second == 0);

    // Повторяем Sber -> symbtable (ничего не должно поменяться)
        REQUIRE(lexer.yylex() == ID);
        REQUIRE(symbtable.size() == 1);
        CHECK(std::get<int>(yylval) == 0);
        it = symbtable.find("Sber");
        REQUIRE(it != symbtable.end());
        CHECK(it->second == 0);

    // Compiler -> symbtable
        REQUIRE(lexer.yylex() == ID);
        REQUIRE(symbtable.size() == 2);
        CHECK(std::get<int>(yylval) == 1);
        it = symbtable.find("Compilers");
        REQUIRE(it != symbtable.end());
        CHECK(it->second == 1);
}
