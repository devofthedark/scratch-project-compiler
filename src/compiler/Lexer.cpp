#include "Lexer.hpp"

#include <format>
#include <fstream>
#include <set>
#include <stdexcept>

#include "exceptions/LanguageErrors.hpp"

static const std::set<std::string> KEYWORDS = {"if", "else", "while", "num", "str", "fn", "return"};
static const std::set<std::string> OPERATORS = {
    "+", "-", "*", "/", "=", "==", "!=", ">=", "<=", ">", "<", "->", "&&", "||", "%", "!"};
static const std::set<char> PUNCTUATION = {'(', ')', '{', '}', ';', ','};
namespace {
bool is_operator_char(char chr) {
    return std::string("+-*/=<>!%|&").find(chr) != std::string::npos;
}
Token operator_to_token(const std::string &cur_token, int line_number) {
    if (cur_token == "+") {
        return {.type = TokenType::PLUS, .value = cur_token, .line = line_number};
    }
    if (cur_token == "-") {
        return {.type = TokenType::MINUS, .value = cur_token, .line = line_number};
    }
    if (cur_token == "*") {
        return {.type = TokenType::MULTIPLY, .value = cur_token, .line = line_number};
    }
    if (cur_token == "/") {
        return {.type = TokenType::DIVIDE, .value = cur_token, .line = line_number};
    }
    if (cur_token == "=") {
        return {.type = TokenType::ASSIGN, .value = cur_token, .line = line_number};
    }
    if (cur_token == "==") {
        return {.type = TokenType::EQUALS_TEST, .value = cur_token, .line = line_number};
    }
    if (cur_token == "!=") {
        return {.type = TokenType::NOT_EQUALS, .value = cur_token, .line = line_number};
    }
    if (cur_token == ">=") {
        return {.type = TokenType::GREATER_EQUAL, .value = cur_token, .line = line_number};
    }
    if (cur_token == "<=") {
        return {.type = TokenType::LESS_EQUAL, .value = cur_token, .line = line_number};
    }
    if (cur_token == ">") {
        return {.type = TokenType::GREATER_THAN, .value = cur_token, .line = line_number};
    }
    if (cur_token == "<") {
        return {.type = TokenType::LESS_THAN, .value = cur_token, .line = line_number};
    }
    if (cur_token == "->") {
        return {.type = TokenType::ARROW, .value = cur_token, .line = line_number};
    }
    if (cur_token == "&&") {
        return {.type = TokenType::AND, .value = cur_token, .line = line_number};
    }
    if (cur_token == "||") {
        return {.type = TokenType::OR, .value = cur_token, .line = line_number};
    }
    if (cur_token == "%") {
        return {.type = TokenType::MOD, .value = cur_token, .line = line_number};
    }
    if (cur_token == "!") {
        return {.type = TokenType::NOT, .value = cur_token, .line = line_number};
    }
    throw SyntaxError(std::format("Unknown Operator \"{}\"", cur_token), line_number);
}

Token keyiden_to_token(const std::string &cur_token, int line_number) {
    if (cur_token == "if") {
        return {.type = TokenType::IF, .value = cur_token, .line = line_number};
    }
    if (cur_token == "else") {
        return {.type = TokenType::ELSE, .value = cur_token, .line = line_number};
    }
    if (cur_token == "while") {
        return {.type = TokenType::WHILE, .value = cur_token, .line = line_number};
    }
    if (cur_token == "num") {
        return {.type = TokenType::NUM, .value = cur_token, .line = line_number};
    }
    if (cur_token == "str") {
        return {.type = TokenType::STR, .value = cur_token, .line = line_number};
    }
    if (cur_token == "fn") {
        return {.type = TokenType::FN, .value = cur_token, .line = line_number};
    }
    if (cur_token == "return") {
        return {.type = TokenType::RETURN, .value = cur_token, .line = line_number};
    }
    return {.type = TokenType::IDENTIFIER, .value = cur_token, .line = line_number};
}

Token punctuation_to_token(char cur_token, int line_number) {
    switch (cur_token) {
        case ';':
            return {.type = TokenType::SEMICOLON,
                    .value = std::string(1, cur_token),
                    .line = line_number};
        case '(':
            return {.type = TokenType::LPAREN,
                    .value = std::string(1, cur_token),
                    .line = line_number};
        case ')':
            return {.type = TokenType::RPAREN,
                    .value = std::string(1, cur_token),
                    .line = line_number};
        case '{':
            return {.type = TokenType::LBRACE,
                    .value = std::string(1, cur_token),
                    .line = line_number};
        case '}':
            return {.type = TokenType::RBRACE,
                    .value = std::string(1, cur_token),
                    .line = line_number};
        default:
            throw std::runtime_error(
                std::format("How... How'd you even get here? ({}:{})", __FILE_NAME__, __LINE__));
    }
}

void parse_number(std::vector<Token> &tokens,
                  const std::string &line,
                  size_t &pos,
                  int line_number) {
    std::string current_token;
    bool has_dot = false;

    while (pos < line.length() && ((isdigit(line[pos]) != 0) || line[pos] == '.')) {
        if (line[pos] == '.') {
            if (has_dot) {
                break; // only allow one dot
            }
            has_dot = true;
        }
        current_token += line[pos++];
    }

    tokens.push_back({TokenType::LITERAL_NUMBER, current_token, line_number});
    pos--;
}

void parse_identifier(std::vector<Token> &tokens,
                      const std::string &line,
                      size_t &pos,
                      int line_number) {
    std::string current_token;
    while (pos < line.length() && ((isalnum(line[pos]) != 0) || line[pos] == '_')) {
        current_token += line[pos++];
    }
    tokens.push_back(keyiden_to_token(current_token, line_number));
    pos--;
}

void parse_operator(std::vector<Token> &tokens,
                    const std::string &line,
                    size_t &pos,
                    int line_number) {
    std::string current_token;
    current_token += line[pos];
    while (pos + 1 < line.length() && is_operator_char(line[pos + 1])) {
        current_token += line[++pos];
    }
    tokens.push_back(operator_to_token(current_token, line_number));
}

void process_line(std::vector<Token> &tokens, std::string line, int line_number) {
    std::string current_token;
    for (size_t pos = 0; pos < line.length(); ++pos) {
        current_token.clear();
        char chr = line[pos];
        if (isspace(chr) != 0) {
            continue;
        }

        // Number (integer or float)
        if ((isdigit(chr) != 0)
            || (chr == '.' && pos + 1 < line.length() && (isdigit(line[pos + 1]) != 0))) {
            parse_number(tokens, line, pos, line_number);
            continue;
        }

        // Identifier or keyword
        if ((isalpha(chr) != 0) || chr == '_') {
            parse_identifier(tokens, line, pos, line_number);
            continue;
        }

        // Operator
        if (is_operator_char(chr)) {
            parse_operator(tokens, line, pos, line_number);
            continue;
        }

        // Punctuation
        if (PUNCTUATION.contains(chr)) {
            tokens.push_back(punctuation_to_token(chr, line_number));
            continue;
        }
        throw SyntaxError(std::format("Unknown character \"{}\"", chr), line_number);
    }
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
        process_line(tokens, line, line_number);
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
    static constexpr int PRECEDENCE_NOT = 7;

    switch (type) {
        case TokenType::OR:
            return PRECEDENCE_OR; // ||
        case TokenType::AND:
            return PRECEDENCE_AND; // &&
        case TokenType::EQUALS_TEST: // ==, !=
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
        case TokenType::NOT:
            return PRECEDENCE_NOT;
        default:
            return 0; // Non-operators
    }
}