cmake_minimum_required(VERSION 4.0)
project(Quokka0_1 C)

set(CMAKE_C_STANDARD 11)

add_library(quokka_lexer
    src/lexer.c)

target_include_directories(quokka_lexer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(quokka
    src/main.c)

target_link_libraries(quokka
        quokka_lexer
)

add_executable(lexer_test
        src/tests/lexer_test.c
)

target_link_libraries(lexer_test
        quokka_lexer
)