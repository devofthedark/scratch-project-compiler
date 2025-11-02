#include "LanguageErrors.hpp"

#include <format>

SyntaxError::SyntaxError(std::string msg, int line) : line(line), message(std::move(msg)) {}

const char *SyntaxError::what() const noexcept {
    if (cached.empty()) {
        cached = std::format("Syntax Error at line {}: {}", line, message);
    }
    return cached.c_str();
}