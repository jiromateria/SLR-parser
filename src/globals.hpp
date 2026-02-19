#pragma once
#include "tokens.hpp"

// Переменная для хранения значений токенов
extern TokenValue yylval;

// Хеш-таблица символов
extern SymbTable symbtable;

// Счетчик для id для хеш таблицы
extern unsigned int symbID;

// Отчистка глобальных переменных
void resetGlobals();