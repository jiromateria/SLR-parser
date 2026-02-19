#pragma once

#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

enum TokenType {
    END = 0,            // Конец 
    NUM_INT,            // Целое число
    NUM_FLOAT,          // Вещественное число
    PLUS,               // '+'
    MINUS,              // '-'
    MULTIPLY,           // '*'
    DIVIDE,             // '/'
    LPAREN,             // '('
    RPAREN,             // ')'
    ID,                 // Переменная (строчные или заглавные латинсие буквы)
    ERROR               // Неизвестный символ
};

// Хеш-таблица для ID
using SymbTable = std::unordered_map< std::string, int >;

// Тип для значений токенов
using TokenValue = std::variant<std::monostate, int, double>;

class Token {
    public:
    TokenType type;
    TokenValue value;

    Token(TokenType t, TokenValue v) : type(t), value(v) {}

    bool operator == (const Token& other) const {
        return (type == other.type) && (value == other.value);
    };
};

