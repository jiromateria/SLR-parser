cmake_minimum_required(VERSION 3.14)
project(SLR_Parser VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Поиск Flex ===
find_package(FLEX REQUIRED)
FLEX_TARGET(Lexer src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)

# === Подключение Catch2 ===
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# === Исполняемый файл: Парсер ===
add_executable(SLR_parser.out
    src/main.cpp
    src/globals.cpp
    ${FLEX_Lexer_OUTPUTS}
)
target_include_directories(SLR_parser.out PRIVATE src /usr/include)

# === Исполняемый файл: Тесты ===
add_executable(run_tests
    test/test_lexer.cpp
    test/main.cpp
    src/globals.cpp
    ${FLEX_Lexer_OUTPUTS}
)
target_link_libraries(run_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(run_tests PRIVATE src test /usr/include)

# === Информация ===
message(STATUS "Flex найден: ${FLEX_EXECUTABLE}")
message(STATUS "Папка сборки: ${CMAKE_CURRENT_BINARY_DIR}")