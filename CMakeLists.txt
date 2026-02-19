cmake_minimum_required(VERSION 3.14)
project(SLR_Parser VERSION 1.0 LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Поиск Flex (для lexer.l) ---
find_package(FLEX REQUIRED)

# --- Генерация lex.yy.cc из lexer.l ---
FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)

# --- Подключение Catch2 через FetchContent ---
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# --- Исполняемый файл: Парсер (SLR_parser.out) ---
add_executable(SLR_parser.out
    src/main.cpp
    src/globals.cpp
    ${FLEX_Lexer_OUTPUTS}  # lex.yy.cc
)

# Добавляем пути для заголовков
target_include_directories(SLR_parser.out PRIVATE src)

# --- Исполняемый файл: Тесты ---
add_executable(run_tests
    test/test_lexer.cpp
    test/main.cpp           # Должен содержать CATCH_CONFIG_MAIN
    src/globals.cpp
    ${FLEX_Lexer_OUTPUTS}   # lex.yy.cc (нужен для тестов лексера)
)

# Подключаем Catch2 и заголовки
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(run_tests PRIVATE src test)

# --- Вывод информации ---
message(STATUS "Flex найден: ${FLEX_EXECUTABLE}")
message(STATUS "Папка сборки: ${CMAKE_CURRENT_BINARY_DIR}")