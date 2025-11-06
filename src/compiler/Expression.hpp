#pragma once
#include "ASTNode.hpp"
#include "Statement.hpp"
class Expression : virtual public ASTNode {
    virtual std::unique_ptr<Expression> make_expression_compat(
        StatementSubstitution &statememts_added);
};