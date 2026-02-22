#include "parser.hpp"
#include <cstring>
#include <iomanip> 

// Для терминалов
const char* term_names[] = {
    "id",     // Id
    "+",      // Plus
    "-",      // Minus
    "*",      // Mul
    "/",      // Div
    "(",      // LParen
    ")",      // RParen
    "$",      // End
    "?"       // COUNT (не используется)
};

// Для нетерминалов
const char* nonterm_names[] = {
    "E",      // E
    "T",      // T
    "F"       // F
};

// Для токенов (буфера) – почти то же, что терминалы, но добавим num и id отдельно
const char* token_names[] = {
    "$",      // END
    "num",    // NUM_INT
    "float",  // NUM_FLOAT
    "+",      // PLUS
    "-",      // MINUS
    "*",      // MULTIPLY
    "/",      // DIVIDE
    "(",      // LPAREN
    ")",      // RPAREN
    "id",     // ID
    "err"     // ERROR
};

//      Таблицы для SLR           
// Константы для ACTION
constexpr auto SHIFT =  ActionType::SHIFT;
constexpr auto REDUCE = ActionType::REDUCE;
constexpr auto ACCEPT = ActionType::ACCEPT;
constexpr auto ER =     ActionType::ERROR;

const ActionEntry SyntaxAnalyzer<SLR>::ACTION
    [SyntaxAnalyzer<SLR>::NUM_STATES]
    [SyntaxAnalyzer<SLR>::NUM_TERMINALS] =
{
    //         id           +           -           *           /           (           )           $        
    /* 0  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 1  */{{ER,0},     {SHIFT,6},  {SHIFT,7},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {ACCEPT,0}   },
    /* 2  */{{ER,0},     {REDUCE,3}, {REDUCE,3}, {SHIFT,8},  {SHIFT,9},  {ER,0},     {REDUCE,3}, {REDUCE,3}   },
    /* 3  */{{ER,0},     {REDUCE,6}, {REDUCE,6}, {REDUCE,6}, {REDUCE,6}, {ER,0},     {REDUCE,6}, {REDUCE,6},  },
    /* 4  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 5  */{{ER,0},     {REDUCE,7}, {REDUCE,7}, {REDUCE,7}, {REDUCE,7}, {ER,0},     {REDUCE,7}, {REDUCE,7},  },
    /* 6  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 7  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 8  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 9  */{{SHIFT,5},  {ER,0},     {ER,0},     {ER,0},     {ER,0},     {SHIFT,4},  {ER,0},     {ER,0}       },
    /* 10 */{{ER,0},     {SHIFT,6},  {SHIFT,7},  {ER,0},     {ER,0},     {ER,0},     {SHIFT,15}, {ER,0}       }, 
    /* 11 */{{ER,0},     {REDUCE,1}, {REDUCE,1}, {SHIFT,8},  {SHIFT,9},  {ER,0},     {REDUCE,1}, {REDUCE,1},  },
    /* 12 */{{ER,0},     {REDUCE,2}, {REDUCE,2}, {SHIFT,8},  {SHIFT,9},  {ER,0},     {REDUCE,2}, {REDUCE,2},  },
    /* 13 */{{ER,0},     {REDUCE,4}, {REDUCE,4}, {REDUCE,4}, {REDUCE,4}, {ER,0},     {REDUCE,4}, {REDUCE,4},  },
    /* 14 */{{ER,0},     {REDUCE,5}, {REDUCE,5}, {REDUCE,5}, {REDUCE,5}, {ER,0},     {REDUCE,5}, {REDUCE,5},  },
    /* 15 */{{ER,0},     {REDUCE,8}, {REDUCE,8}, {REDUCE,8}, {REDUCE,8}, {ER,0},     {REDUCE,8}, {REDUCE,8},  }
} ;
      
const int SyntaxAnalyzer<SLR>::GOTO
    [SyntaxAnalyzer<SLR>::NUM_STATES]
    [SyntaxAnalyzer<SLR>::NUM_NONTERMINALS] = 
{
    //            E       T       F     (-1 - ОШИБКА)
    /* 0  */    {  1,     2,      3 },
    /* 1  */    { -1,    -1,     -1 },    
    /* 2  */    { -1,    -1,     -1 },
    /* 3  */    { -1,    -1,     -1 },
    /* 4  */    { 10,     2,      3 },
    /* 5  */    { -1,    -1,     -1 },
    /* 6  */    { -1,    11,      3 },
    /* 7  */    { -1,    12,      3 },
    /* 8  */    { -1,    -1,     13 },
    /* 9  */    { -1,    -1,     14 },
    /* 10 */    { -1,    -1,     -1 },
    /* 11 */    { -1,    -1,     -1 },
    /* 12 */    { -1,    -1,     -1 },
    /* 13 */    { -1,    -1,     -1 },
    /* 14 */    { -1,    -1,     -1 },
    /* 15 */    { -1,    -1,     -1 }
};

// Константы для PRODUCER
constexpr auto NT_E = Nonterminals::E;
constexpr auto NT_T = Nonterminals::T;
constexpr auto NT_F = Nonterminals::F;

const Production SyntaxAnalyzer<SLR>::PRODUCTIONS[SyntaxAnalyzer<SLR>::NUM_PRODUCTIONS] = 
{
    { NT_E, 3 },  // 1: E -> E + T
    { NT_E, 3 },  // 2: E -> E - T
    { NT_E, 1 },  // 3: E -> T
    { NT_T, 3 },  // 4: T -> T * F
    { NT_T, 3 },  // 5: T -> T / F
    { NT_T, 1 },  // 6: T -> F
    { NT_F, 1 },  // 7: F -> id
    { NT_F, 3 }   // 8: F -> ( E )
};


//     Маппинг Token->Terminals   
Terminals get_terminal(const Token& token) noexcept {
    switch (token.type) {
        case ID:
        case NUM_INT:
        case NUM_FLOAT:
            return Terminals::Id;
        case PLUS:      return Terminals::Plus;
        case MINUS:     return Terminals::Minus;
        case MULTIPLY:  return Terminals::Mul;
        case DIVIDE:    return Terminals::Div;
        case LPAREN:    return Terminals::LParen;
        case RPAREN:    return Terminals::RParen;
        case END:       return Terminals::End;
        case ERROR:
        default:        return Terminals::COUNT;
    }
}

// Методы SLR

std::string SyntaxAnalyzer<SLR>::symbol_to_string(size_t sym) const {
    if (sym == STACK_BOTTOM_MARKER)
        return "#";

    const size_t term_count = NUM_TERMINALS;
    if (sym < term_count) {
        static const char* term_names[] = {"id", "+", "-", "*", "/", "(", ")", "$"};
        return term_names[sym];
    } else if (sym >= term_count + 1 && sym < term_count + 1 + static_cast<size_t>(Nonterminals::COUNT)) {
        size_t nt_index = sym - term_count - 1;
        static const char* nonterm_names[] = {"E", "T", "F"};
        return nonterm_names[nt_index];
    } else {
        return "?";
    }
}

void SyntaxAnalyzer<SLR>::reset() {
    buffer_.clear();

    while (!state_stack_.empty()) state_stack_.pop();
    while (!symbol_stack_.empty()) symbol_stack_.pop();

    state_stack_.push(0);
    symbol_stack_.push(STACK_BOTTOM_MARKER);
    pos_ = 0;
    reductions.clear();
}

bool SyntaxAnalyzer<SLR>::shift(int next_state) {
    state_stack_.push(next_state);
    return true;
}

bool SyntaxAnalyzer<SLR>::reduce(int prod_id) {
    if(prod_id < 1 || prod_id > NUM_PRODUCTIONS) {
        std::cerr << "Incolid production ID: " << prod_id << "\n";
        return false;
    }

    reductions.push_back(prod_id);
    const auto& prod = PRODUCTIONS[prod_id - 1];

    std::size_t real_symbols = symbol_stack_.size() - 1;  // -1 = маркер
    if (prod.right_len > static_cast<int>(real_symbols)) {
        std::cerr << "Stack is empty during reduce: need " << prod.right_len 
                  << ", have " << real_symbols << " (excluding marker)\n";
        return false;
    }
    
    // Pop элементов стека (right_len раз)
    for (int i = 0; i < prod.right_len; ++i) {
        state_stack_.pop();
        symbol_stack_.pop();
    }
    
    // GOTO: переход по нетерминалу
    int new_state = GOTO[current_state()][static_cast<std::size_t>(prod.left)];
    if (new_state == GOTO_ERROR) {
        std::cerr << "GOTO error for nonterminal " 
                  << static_cast<int>(prod.left) << "\n";
        return false;
    }
    
    state_stack_.push(new_state);
    symbol_stack_.push(static_cast<std::size_t>(prod.left) + NUM_TERMINALS + 1);
    return true;
}

bool SyntaxAnalyzer<SLR>::parse(const std::vector<Token>& tokens) {
    reset();
    buffer_ = tokens;
    if (buffer_.empty() || buffer_.back().type != END)
        buffer_.push_back(Token{END, std::monostate{}});

    #ifndef NDEBUG
    std::cout << "\n| Step | State Stack      | Symbol Stack     | Buffer                | Action   |\n";
    std::cout << "|------|------------------|------------------|-----------------------|----------|\n";
    #endif

    int step = 0;
    while (true) {
        Token token = (pos_ < buffer_.size()) ? buffer_[pos_] : Token{END, std::monostate{}};
        int state = current_state();
        std::size_t idx = static_cast<std::size_t>(get_terminal(token));
        
        if (state >= NUM_STATES || idx >= NUM_TERMINALS) return false;
        
        const auto& entry = ACTION[state][idx];
        std::string action = "";
        
    #ifndef NDEBUG
    auto states = get_states();
    auto symbols = get_symbols();

    // Состояния
    std::string state_str;
    for (size_t i = 0; i < states.size(); ++i) {
        if (i > 0) state_str += " ";
        state_str += std::to_string(states[i]);
    }
    std::cout << "| " << std::setw(4) << step << " | "
              << std::left << std::setw(16) << state_str << " | ";

    // Символы (через symbol_to_string)
    std::string sym_str;
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (i > 0) sym_str += " ";
        sym_str += symbol_to_string(symbols[i]);
    }
    std::cout << std::left << std::setw(16) << sym_str << " | ";

    // Буфер (токены)
    std::string buf_str;
    for (size_t i = pos_; i < buffer_.size(); ++i) {
        if (i > 0) buf_str += " ";
        if (i == pos_) buf_str += ">";
        static const char* token_names[] = {"$", "num", "float", "+", "-", "*", "/", "(", ")", "id", "err"};
        buf_str += token_names[buffer_[i].type];
    }
    std::cout << std::left << std::setw(21) << buf_str << " | ";
    #endif

        switch (entry.type) {
            case SHIFT:
                symbol_stack_.push(idx);
                shift(entry.value);
                ++pos_;
                action = "SHIFT " + std::to_string(entry.value);
                break;
            case REDUCE:
                if (!reduce(entry.value)) return false;
                action = "REDUCE " + std::to_string(entry.value);
                break;
            case ACCEPT:
                action = "ACCEPT";
                #ifndef NDEBUG
                std::cout << std::setw(8) << action << " |\n";
                std::cout << "\nSUCCESS!\n";
                #endif
                return true;
            case ER:
                action = "ERROR";
                #ifndef NDEBUG
                std::cout << std::setw(8) << action << " |\n";
                std::cout << "\nFAILED!\n";
                #endif
                return false;
        }
        
        #ifndef NDEBUG
        std::cout << std::setw(8) << action << " |\n";
        #endif
        step++;
    }
}

