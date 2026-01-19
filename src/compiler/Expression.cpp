#include "Expression.hpp"

std::unique_ptr<Expression> Expression::make_expression_compat(
    const std::string& sprite_name,
    StatementSubstitution &statememts_added) {
    (void) sprite_name;
    (void) statememts_added;
    return nullptr;
}

std::unique_ptr<Expression> Expression::conv_name(const std::set<std::string> &args) {
    (void) args;
    return nullptr;
}
