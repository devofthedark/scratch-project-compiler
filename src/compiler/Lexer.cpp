#include "Lexer.hpp"

#include <fstream>
#include <set>

static const std::set<std::string> KEYWORDS = {"if", "else", "while", "num", "str", "fn", "return"};
static const std::set<std::string> OPERATORS = {
    "+", "-", "*", "/", "=", "==", "!=", ">=", "<=", ">", "<", "->", "&&", "||", "%"};
static const std::set<char> PUNCTUATION = {'(', ')', '{', '}', ';', ','};
namespace {
bool is_operator_char(char chr) {
    return std::string("+-*/=<>!%|&").find(chr) != std::string::npos;
}
} // namespace
// TODO: fix this mess of a function
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
std::vector<Token> tokenize(const std::string &source_file) {
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
            char chr = line[i];
            if (isspace(chr) != 0) {
                continue;
            }

            // Number (integer or float)
            if ((isdigit(chr) != 0)
                || (chr == '.' && i + 1 < line.length() && (isdigit(line[i + 1]) != 0))) {
                bool has_dot = false;

                while (i < line.length() && ((isdigit(line[i]) != 0) || line[i] == '.')) {
                    if (line[i] == '.') {
                        if (has_dot) {
                            break; // only allow one dot
                        }
                        has_dot = true;
                    }
                    current_token += line[i++];
                }

                tokens.push_back({TokenType::LITERAL_NUMBER, current_token, line_number});
                i--;
                continue;
            }

            // Identifier or keyword
            if ((isalpha(chr) != 0) || chr == '_') {
                while (i < line.length() && ((isalnum(line[i]) != 0) || line[i] == '_')) {
                    current_token += line[i++];
                }

                if (KEYWORDS.contains(current_token)) {
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
            if (is_operator_char(chr)) {
                current_token += chr;
                while (i + 1 < line.length() && is_operator_char(line[i + 1])) {
                    current_token += line[++i];
                }

                if (OPERATORS.contains(current_token)) {
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
            if (PUNCTUATION.contains(chr)) {
                current_token += chr;
                if (chr == ';') {
                    tokens.push_back({TokenType::SEMICOLON, current_token, line_number});
                } else if (chr == ',') {
                    tokens.push_back({TokenType::COMMA, current_token, line_number});
                } else if (chr == '(') {
                    tokens.push_back({TokenType::LPAREN, current_token, line_number});
                } else if (chr == ')') {
                    tokens.push_back({TokenType::RPAREN, current_token, line_number});
                } else if (chr == '{') {
                    tokens.push_back({TokenType::LBRACE, current_token, line_number});
                } else if (chr == '}') {
                    tokens.push_back({TokenType::RBRACE, current_token, line_number});
                }
                continue;
            }

            throw std::runtime_error("Unknown character \"" + std::string(1, chr) + "\" at line "
                                     + std::to_string(line_number));
        }
    }
    file.close();
    return tokens;
}

int get_operator_precedence(TokenType type) {
    // higher number means higher precedence
    static constexpr int PRECEDENCE_OR = 1;
    static constexpr int PRECEDENCE_AND = 2;
    static constexpr int PRECEDENCE_EQUALS = 3;
    static constexpr int PRECEDENCE_COMPARISON = 4;
    static constexpr int PRECEDENCE_ADDITIVE = 5;
    static constexpr int PRECEDENCE_MULTIPLICATIVE = 6;

    switch (type) {
        case TokenType::OR:
            return PRECEDENCE_OR; // ||
        case TokenType::AND:
            return PRECEDENCE_AND; // &&
        case TokenType::EQUALS: // ==, !=
        case TokenType::NOT_EQUALS:
            return PRECEDENCE_EQUALS;
        case TokenType::GREATER_THAN: // >, <, >=, <=
        case TokenType::LESS_THAN:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS_EQUAL:
            return PRECEDENCE_COMPARISON;
        case TokenType::PLUS: // +, -
        case TokenType::MINUS:
            return PRECEDENCE_ADDITIVE;
        case TokenType::MULTIPLY: // *, /, %
        case TokenType::DIVIDE:
        case TokenType::MOD:
            return PRECEDENCE_MULTIPLICATIVE;
        default:
            return 0; // Non-operators
    }
}