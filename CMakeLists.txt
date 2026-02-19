cmake_minimum_required(VERSION 3.26)
project(quokka C)

set(CMAKE_C_STANDARD 11)

# Core library with lexer
add_library(quokka_lexer src/lexer.c)
target_include_directories(quokka_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Core library with AST, parser, and validator
add_library(quokka_core
        src/ast.c
        src/parser.c
        src/validator.c
)
target_include_directories(quokka_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(quokka_core quokka_lexer)

# Main executable
add_executable(quokka src/main.c)
target_link_libraries(quokka quokka_core quokka_lexer)

# Lexer test executable
add_executable(lexer_test src/tests/lexer_test.c)
target_link_libraries(lexer_test quokka_core quokka_lexer)

# Parser test executable
add_executable(parser_test src/tests/parser_test.c)
target_link_libraries(parser_test quokka_core quokka_lexer)
