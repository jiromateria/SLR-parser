cmake_minimum_required(VERSION 3.10)
project(SLR_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(Catch2 REQUIRED)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

file(MAKE_DIRECTORY ${GENERATED_DIR})

FLEX_TARGET(MyLexer ${SRC_DIR}/lexer.l ${GENERATED_DIR}/lex.yy.cc)

set(COMMON_SOURCES
    ${SRC_DIR}/parser.cpp
    ${SRC_DIR}/globals.cpp
    ${FLEX_MyLexer_OUTPUTS}
)


add_executable(compiler
    ${SRC_DIR}/main.cpp
    ${COMMON_SOURCES}
)

target_include_directories(compiler
    PRIVATE
        ${INCLUDE_DIR}
        ${GENERATED_DIR}
)


add_executable(unit_tests
    ${TEST_DIR}/test_parser.cpp
    ${TEST_DIR}/test_lexer.cpp
    ${COMMON_SOURCES}
)

target_include_directories(unit_tests
    PRIVATE
        ${INCLUDE_DIR}
        ${GENERATED_DIR}
)

target_compile_definitions(unit_tests PRIVATE NDEBUG)   
target_link_libraries(unit_tests Catch2::Catch2WithMain)

enable_testing()
add_test(NAME all_tests COMMAND unit_tests)

message(STATUS "Flex найден: ${FLEX_EXECUTABLE}")
message(STATUS "Папка сборки: ${CMAKE_BINARY_DIR}")
message(STATUS "Генерируемые файлы: ${GENERATED_DIR}")