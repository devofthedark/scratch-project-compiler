#pragma once
#include "Statement.hpp"
#include "exceptions/LanguageErrors.hpp"
class Expression : virtual public ASTNode {
public:
    virtual std::unique_ptr<Expression> make_expression_compat(
        const std::string &sprite_name,
        StatementSubstitution &statememts_added);
    virtual std::unique_ptr<Expression> conv_name(const std::set<std::string> &args);
    [[nodiscard]] virtual const std::string &str_if_string_literal() const {
        throw TypeError("Expected literal string");
    }
};
