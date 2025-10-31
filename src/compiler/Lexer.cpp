#include "Lexer.hpp"
#include <set>
#include <fstream>

const std::set<std::string> keywords = {"if", "else", "while", "num", "str", "fn", "return"};
const std::set<std::string> operators = {
    "+", "-", "*", "/", "=", "==", "!=", ">=", "<=", ">", "<", "->", "&&", "||", "%"
};
const std::set<char> punctuation = {'(', ')', '{', '}', ';', ','};

bool is_operator_char(char c) {
    return std::string("+-*/=<>!%|&").find(c) != std::string::npos;
}

std::vector<Token> tokenize(const std::string& source_file) {
    std::vector<Token> tokens;
    std::ifstream file(source_file);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + source_file);
    }
    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        std::string current_token;
        for (size_t i = 0; i < line.length(); ++i) {
            current_token.clear();
            char c = line[i];
            if (isspace(c)) {
                continue;
            }

            // Number (integer or float)
            if (isdigit(c) || (c == '.' && i + 1 < line.length() && isdigit(line[i + 1]))) {
                bool hasDot = false;

                while (i < line.length() && (isdigit(line[i]) || line[i] == '.')) {
                    if (line[i] == '.') {
                        if (hasDot) break;  // only allow one dot
                        hasDot = true;
                    }
                    current_token += line[i++];
                }

                tokens.push_back({TokenType::LITERAL_NUMBER, current_token, line_number});
                i--;
                continue;
            }

            // Identifier or keyword
            if (isalpha(c) || c == '_') {
                while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) {
                    current_token += line[i++];
                }

                if (keywords.find(current_token) != keywords.end()) {
                    if (current_token == "if") {
                        tokens.push_back({TokenType::IF, current_token, line_number});
                    } else if (current_token == "else") {
                        tokens.push_back({TokenType::ELSE, current_token, line_number});
                    } else if (current_token == "while") {
                        tokens.push_back({TokenType::WHILE, current_token, line_number});
                    } else if (current_token == "num") {
                        tokens.push_back({TokenType::NUM, current_token, line_number});
                    } else if (current_token == "str") {
                        tokens.push_back({TokenType::STR, current_token, line_number});
                    } else if (current_token == "fn") {
                        tokens.push_back({TokenType::FN, current_token, line_number});
                    } else if (current_token == "return") {
                        tokens.push_back({TokenType::RETURN, current_token, line_number});
                    }
                } else {
                    tokens.push_back({TokenType::IDENTIFIER, current_token, line_number});
                }
                i--;
                continue;
            }

            // Operator
            if (is_operator_char(c)) {
                current_token += c;
                while (i + 1 < line.length() && is_operator_char(line[i + 1])) {
                    current_token += line[++i];
                }

                if (operators.find(current_token) != operators.end()) {
                    if (current_token == "+") {
                        tokens.push_back({TokenType::PLUS, current_token, line_number});
                    } else if (current_token == "-") {
                        tokens.push_back({TokenType::MINUS, current_token, line_number});
                    } else if (current_token == "*") {
                        tokens.push_back({TokenType::MULTIPLY, current_token, line_number});
                    } else if (current_token == "/") {
                        tokens.push_back({TokenType::DIVIDE, current_token, line_number});
                    } else if (current_token == "=") {
                        tokens.push_back({TokenType::ASSIGN, current_token, line_number});
                    } else if (current_token == "==") {
                        tokens.push_back({TokenType::EQUALS, current_token, line_number});
                    } else if (current_token == "!=") {
                        tokens.push_back({TokenType::NOT_EQUALS, current_token, line_number});
                    } else if (current_token == ">=") {
                        tokens.push_back({TokenType::GREATER_EQUAL, current_token, line_number});
                    } else if (current_token == "<=") {
                        tokens.push_back({TokenType::LESS_EQUAL, current_token, line_number});
                    } else if (current_token == ">") {
                        tokens.push_back({TokenType::GREATER_THAN, current_token, line_number});
                    } else if (current_token == "<") {
                        tokens.push_back({TokenType::LESS_THAN, current_token, line_number});
                    } else if (current_token == "->") {
                        tokens.push_back({TokenType::ARROW, current_token, line_number});
                    } else if (current_token == "&&") {
                        tokens.push_back({TokenType::AND, current_token, line_number});
                    } else if (current_token == "||") {
                        tokens.push_back({TokenType::OR, current_token, line_number});
                    } else if (current_token == "%") {
                        tokens.push_back({TokenType::MOD, current_token, line_number});
                    }
                } else {
                    throw std::runtime_error("Unknown operator: " + current_token);
                }
                continue;
            }

            // Punctuation  
            if (punctuation.count(c)) {
                current_token += c;
                if (c == ';') {
                    tokens.push_back({TokenType::SEMICOLON, current_token, line_number});
                } else if (c == ',') {
                    tokens.push_back({TokenType::COMMA, current_token, line_number});
                } else if (c == '(') {
                    tokens.push_back({TokenType::LPAREN, current_token, line_number});
                } else if (c == ')') {
                    tokens.push_back({TokenType::RPAREN, current_token, line_number});
                } else if (c == '{') {
                    tokens.push_back({TokenType::LBRACE, current_token, line_number});
                } else if (c == '}') {
                    tokens.push_back({TokenType::RBRACE, current_token, line_number});
                }
                continue;
            }

            throw std::runtime_error("Unknown character \"" + std::string(1, c) + "\" at line " + std::to_string(line_number));
        }
    }
    file.close();
    return tokens;
}

int get_operator_precedence(TokenType type) {
    // higher number means higher precedence
    switch (type) {
        case TokenType::OR: return 1;          // ||
        case TokenType::AND: return 2;         // &&
        case TokenType::EQUALS:                // ==, !=
        case TokenType::NOT_EQUALS: return 3;
        case TokenType::GREATER_THAN:          // >, <, >=, <=
        case TokenType::LESS_THAN:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS_EQUAL: return 4;
        case TokenType::PLUS:                   // +, -
        case TokenType::MINUS: return 5;
        case TokenType::MULTIPLY:               // *, /, %
        case TokenType::DIVIDE:
        case TokenType::MOD: return 6;
        default: return 0; // Non-operators
    }
}