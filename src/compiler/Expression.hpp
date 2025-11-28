#pragma once
#include "Statement.hpp"
class Expression : virtual public ASTNode {
public:
    virtual std::unique_ptr<Expression> make_expression_compat(
        StatementSubstitution &statememts_added);
    virtual std::unique_ptr<Expression> conv_name(const std::set<std::string> &args);
    virtual std::string str_if_string_literal() {
        throw std::runtime_error("Not a string literal");
    }
};