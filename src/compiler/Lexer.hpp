#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    IF,
    ELSE,
    WHILE,
    FN,
    RETURN,
    NUM,
    STR,
    // Identifiers and Literals
    IDENTIFIER,
    LITERAL_NUMBER,
    LITERAL_STRING,
    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUALS,
    NOT_EQUALS,
    GREATER_THAN,
    LESS_THAN,
    GREATER_EQUAL,
    LESS_EQUAL,
    ARROW,
    // Logical Operators
    AND,
    OR,
    MOD,
    // Delimiters
    SEMICOLON,
    COMMA,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};
std::vector<Token> tokenize(const std::string& source_file);
int get_operator_precedence(TokenType type);