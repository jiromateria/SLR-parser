#pragma once 
#include "tokens.hpp"
#include <vector>
#include <stack>
#include <iostream>
#include <iomanip>

enum Grammar{
    SLR,
    LALR,
    NONE
};

// Нетерминалы
enum class Nonterminals : std::size_t {
    E = 0,
    T = 1,
    F = 2,
    COUNT = 3
};

// Терминалы расположены также как и в таблицах
enum class Terminals : std::size_t{
    Id      = 0,
    Plus    = 1,
    Minus   = 2,
    Mul     = 3,
    Div     = 4,
    LParen  = 5,
    RParen  = 6,
    End     = 7,   // $
    COUNT   = 8
};

enum class ActionType {
    SHIFT,      //Перенос
    REDUCE,     //Свертка
    ACCEPT,     //Принятие
    ERROR       //Ошибка
};


struct ActionEntry {
    ActionType type;
    int value;          //Для SHIFT - новое состояние, для REDUCE - продукция

    constexpr ActionEntry(ActionType t, int v) noexcept: type{t}, value{v} {}
};

struct Production {
    Nonterminals left;     // Левый нетерминал
    int right_len;         // Длина правой части

    constexpr Production(Nonterminals l, int r) noexcept
        : left{l}, right_len{r} {}
};

Terminals get_terminal(const Token& token) noexcept;


template <Grammar G>
class SyntaxAnalyzer {};

template <>
class SyntaxAnalyzer<SLR> {
    private:

        // Константы для ACTION и GOTO
        static inline constexpr int NUM_STATES = 16;           // Количество состояний LR(0) - автомата / SLR таблицы
        static inline constexpr int NUM_TERMINALS = static_cast<int>(Terminals::COUNT);         // id, +, -, *, /, (, ), $
        static inline constexpr int NUM_NONTERMINALS = static_cast<int>(Nonterminals::COUNT);   // E, T, F
        static inline constexpr int NUM_PRODUCTIONS = 8;       // Количсевто продукций
        static constexpr int GOTO_ERROR = -1;

        // Таблицы ACTION, GOTO и ПРОДУКЦИИ
        static const ActionEntry ACTION[NUM_STATES][NUM_TERMINALS];
        static const int GOTO[NUM_STATES][NUM_NONTERMINALS];
        static const Production PRODUCTIONS[NUM_PRODUCTIONS];

        // Символ дна стека
        static inline constexpr std::size_t STACK_BOTTOM_MARKER = 100;

        //Стеки и буффер
        std::vector<Token> buffer_;         // буффер токенов 
        std::stack<int>    state_stack_;    // стек состояний 
        std::stack<size_t> symbol_stack_;   // стек символов (Terminals + Nonterminals)
        std::size_t pos_;

        bool shift(int next_state);
        bool reduce(int prod_id);
        int current_state() const { return state_stack_.top(); }

    public:
        std::vector<int> reductions;        // номера продукций в порядке применения
        
        bool parse(const std::vector<Token>& tokens);
        void reset();

        const std::vector<Token>& get_buffer() const { return buffer_; }
        std::size_t get_pos() const { return pos_; }
        
        std::vector<int> get_states() const {
            std::vector<int> res;
            std::stack<int> tmp = state_stack_;
            while (!tmp.empty()) { res.insert(res.begin(), tmp.top()); tmp.pop(); }
            return res;
        }
        
        std::vector<std::size_t> get_symbols() const {
            std::vector<std::size_t> res;
            std::stack<std::size_t> tmp = symbol_stack_;
            while (!tmp.empty()) { res.insert(res.begin(), tmp.top()); tmp.pop(); }
            return res;
        }

        std::string symbol_to_string(size_t sym) const;
};

